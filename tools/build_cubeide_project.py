#!/usr/bin/env python3
"""Build an STM32CubeIDE project from the command line, the way CubeIDE does.

Reads the project's .project and .cproject, resolves linked folders, collects
the source folders and include paths of the chosen build configuration, then
compiles and links the firmware with arm-none-eabi-gcc.

It exists so that CI can check that every project in this repository still
builds; CubeIDE itself is not needed.

Usage:
    tools/build_cubeide_project.py <project-dir> [--config Debug] [--out build]
"""

import argparse
import os
import re
import subprocess
import sys
import xml.etree.ElementTree as ET

MCU_FLAGS = ["-mcpu=cortex-m4", "-mfpu=fpv4-sp-d16", "-mfloat-abi=hard", "-mthumb"]
C_FLAGS = ["-std=gnu11", "-g3", "-O0", "-ffunction-sections", "-fdata-sections",
           "-Wall", "--specs=nano.specs"]
LD_FLAGS = ["--specs=nosys.specs", "--specs=nano.specs", "-Wl,--gc-sections", "-static",
            "-Wl,--start-group", "-lc", "-lm", "-Wl,--end-group"]


def parse_linked_resources(project_dir):
    """Return ({project-relative path: filesystem path}, {virtual folder names})."""
    tree = ET.parse(os.path.join(project_dir, ".project"))
    links, virtual = {}, set()
    for link in tree.getroot().iter("link"):
        name = link.findtext("name")
        uri = link.findtext("locationURI") or link.findtext("location")
        if not name or not uri:
            continue
        if uri.startswith("virtual:"):
            virtual.add(name)
            continue
        m = re.match(r"PARENT-(\d+)-PROJECT_LOC/(.*)", uri)
        if m:
            base = project_dir
            for _ in range(int(m.group(1))):
                base = os.path.dirname(base)
            target = os.path.join(base, m.group(2))
        elif uri.startswith("PROJECT_LOC/"):
            target = os.path.join(project_dir, uri[len("PROJECT_LOC/"):])
        else:
            target = uri
        links[name] = os.path.normpath(target)
    return links, virtual


def source_roots(project_dir, links, virtual, name):
    """Filesystem folders behind a source entry; a virtual folder has only linked children."""
    if name not in virtual:
        return [resolve(project_dir, links, name)]
    return [target for link, target in sorted(links.items())
            if os.path.dirname(link) == name]


def resolve(project_dir, links, rel):
    """Map a project-relative path to the filesystem, following linked folders."""
    rel = rel.strip("/")
    for name in sorted(links, key=len, reverse=True):
        if rel == name or rel.startswith(name + "/"):
            return os.path.normpath(os.path.join(links[name], rel[len(name):].lstrip("/")))
    return os.path.normpath(os.path.join(project_dir, rel))


def find_configuration(cproject, config_name):
    for cfg in cproject.iter("configuration"):
        if cfg.get("name") == config_name and cfg.get("artifactName"):
            return cfg
    sys.exit(f"configuration '{config_name}' not found")


def option_values(cfg, super_class_suffix):
    for opt in cfg.iter("option"):
        if (opt.get("superClass") or "").endswith(super_class_suffix):
            return [v.get("value") for v in opt.iter("listOptionValue")]
    return []


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("project_dir")
    ap.add_argument("--config", default="Debug")
    ap.add_argument("--out", default=None, help="build directory (default: <project>/build-cli)")
    args = ap.parse_args()

    project_dir = os.path.abspath(args.project_dir)
    project_name = ET.parse(os.path.join(project_dir, ".project")).getroot().findtext("name")
    links, virtual = parse_linked_resources(project_dir)
    cproject = ET.parse(os.path.join(project_dir, ".cproject")).getroot()
    cfg = find_configuration(cproject, args.config)
    # CubeIDE runs the build inside <project>/<config>, so "../x" means <project>/x.
    build_cwd = os.path.join(project_dir, args.config)

    includes = []
    for value in option_values(cfg, "c.compiler.option.includepaths"):
        value = value.strip('"')
        m = re.match(r"\$\{workspace_loc:/\$\{ProjName\}/(.*)\}", value)
        if m:
            includes.append(resolve(project_dir, links, m.group(1)))
        else:
            includes.append(os.path.normpath(os.path.join(build_cwd, value)))
    defines = option_values(cfg, "c.compiler.option.definedsymbols")

    sources = []
    for entry in cfg.iter("entry"):
        if entry.get("kind") != "sourcePath":
            continue
        name = entry.get("name")
        excluded = [resolve(project_dir, links, os.path.join(name, e))
                    for e in (entry.get("excluding") or "").split("|") if e]
        for root in source_roots(project_dir, links, virtual, name):
            if not os.path.isdir(root):
                sys.exit(f"source folder '{name}' points to missing '{root}'")
            for dirpath, _, files in os.walk(root):
                if any(dirpath == e or dirpath.startswith(e + os.sep) for e in excluded):
                    continue
                sources += [os.path.join(dirpath, f) for f in sorted(files)
                            if f.endswith((".c", ".s", ".S"))]
    if not sources:
        sys.exit("no sources found")

    out = os.path.abspath(args.out or os.path.join(project_dir, "build-cli"))
    os.makedirs(out, exist_ok=True)
    common_c = MCU_FLAGS + C_FLAGS + [f"-D{d}" for d in defines] + [f"-I{i}" for i in includes]
    objects, failed = [], 0
    for src in sources:
        obj = os.path.join(out, os.path.relpath(src, os.path.dirname(project_dir))
                           .replace(os.sep, "_").replace("..", "up") + ".o")
        if src.endswith(".c"):
            cmd = ["arm-none-eabi-gcc", *common_c, "-c", src, "-o", obj]
        else:
            cmd = ["arm-none-eabi-gcc", *MCU_FLAGS, "-g3", "-x", "assembler-with-cpp",
                   "--specs=nano.specs", "-c", src, "-o", obj]
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.stdout or result.stderr:
            sys.stdout.write(result.stdout + result.stderr)
        if result.returncode != 0:
            failed += 1
        objects.append(obj)
    if failed:
        sys.exit(f"{project_name}: {failed} file(s) failed to compile")

    ld_scripts = [f for f in os.listdir(project_dir) if f.endswith("_FLASH.ld")]
    if len(ld_scripts) != 1:
        sys.exit(f"expected one *_FLASH.ld linker script, found {ld_scripts}")
    elf = os.path.join(out, project_name + ".elf")
    link = ["arm-none-eabi-gcc", "-o", elf, *objects, *MCU_FLAGS,
            "-T", os.path.join(project_dir, ld_scripts[0]),
            f"-Wl,-Map={os.path.join(out, project_name + '.map')}", *LD_FLAGS]
    result = subprocess.run(link, capture_output=True, text=True)
    sys.stdout.write(result.stdout + result.stderr)
    if result.returncode != 0:
        sys.exit(f"{project_name}: link failed")

    size = subprocess.run(["arm-none-eabi-size", elf], capture_output=True, text=True).stdout
    print(f"{project_name} ({args.config}): {len(sources)} sources OK")
    print(size.strip())


if __name__ == "__main__":
    main()
