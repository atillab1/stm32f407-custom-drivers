# 011 – LIS3DSH accelerometer over SPI

Reads the on-board LIS3DSH 3-axis accelerometer over SPI1, works out roll and pitch, and lights
one of the four LEDs when the board is tilted more than 15° in that direction. The project keeps
its own `lis3dsh` driver in `Core/Inc` and `Core/Src`.

## Wiring

Nothing to connect: the LIS3DSH is on the board (STM32F4DISCOVERY revision MB997C or later;
older boards carry a LIS302DL, which answers `WHO_AM_I` with 0x3B instead of 0x3F).

| Signal | Pin |
|---|---|
| SPI1_SCK | **PA5** |
| SPI1_MISO | **PA6** |
| SPI1_MOSI | **PA7** |
| CS (`MEMS_CS`) | **PE3**, active low |
| INT1 / INT2 | PE0 / PE1 (configured, not used yet) |

## Configuration

| Setting | Value |
|---|---|
| Peripheral | SPI1, full-duplex master, 8 bit, MSB first, 8 Mbit/s |
| Sensor | 800 Hz output data rate, X/Y/Z enabled, ±2 g, 800 Hz anti-aliasing filter |
| LEDs | PD12–PD15 outputs |
| System clock | HSI 16 MHz |

## How it works

- `LIS3DSH_Initialization()` stores the SPI handle and CS pin, checks `WHO_AM_I` (0x3F) and
  writes `CTRL_REG4` and `CTRL_REG5`.
- `LIS3DSH_Read_Register()` sends the register address with bit 7 set (read), then clocks out
  one byte. `LIS3DSH_Write_Register()` sends the address with bit 7 clear, then the data byte.
- `LIS3DSH_Read_XYZ()` reads the six `OUT_X_L` … `OUT_Z_H` registers and joins each pair into a
  signed 16-bit value.
- The main loop turns the three axes into roll and pitch with `atan2f()` once a second:

| LED | Lights when |
|---|---|
| PD12 (green) | pitch < −15° |
| PD14 (red) | pitch > +15° |
| PD13 (orange) | roll < −15° |
| PD15 (blue) | roll > +15° |
