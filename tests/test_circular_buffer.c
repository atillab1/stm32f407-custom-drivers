/*
 * Host-side unit tests for drivers/circular_buffer.
 *
 * The circular buffer has no hardware dependency, so it is compiled and run
 * on the development machine (and in CI) with the native compiler:
 *
 *     make -C tests
 */

#include <stdio.h>

#include "circular_buffer.h"

static int failures = 0;

#define CHECK(cond)                                                        \
	do {                                                                   \
		if (!(cond)) {                                                     \
			printf("  FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);       \
			failures++;                                                    \
		}                                                                  \
	} while (0)

static Circular_Buffer_t cb;

static void test_starts_empty(void)
{
	uint8_t value;

	Circular_Buffer_Init(&cb);
	CHECK(Circular_Buffer_Is_Empty(&cb));
	CHECK(!Circular_Buffer_Is_Full(&cb));
	CHECK(Circular_Buffer_Count(&cb) == 0);
	CHECK(!Circular_Buffer_Dequeue(&cb, &value));
}

static void test_zero_is_valid_data(void)
{
	uint8_t value = 0xAA;

	Circular_Buffer_Init(&cb);
	CHECK(Circular_Buffer_Enqueue(&cb, 0));
	CHECK(!Circular_Buffer_Is_Empty(&cb));
	CHECK(Circular_Buffer_Dequeue(&cb, &value));
	CHECK(value == 0);
	CHECK(Circular_Buffer_Is_Empty(&cb));
}

static void test_capacity_and_fifo_order(void)
{
	uint8_t value;
	int stored = 0;

	Circular_Buffer_Init(&cb);
	while (Circular_Buffer_Enqueue(&cb, (uint8_t)stored))
		stored++;

	/* one slot stays free so that "full" and "empty" can be told apart */
	CHECK(stored == CIRCULAR_BUFFER_SIZE - 1);
	CHECK(Circular_Buffer_Is_Full(&cb));
	CHECK(Circular_Buffer_Count(&cb) == CIRCULAR_BUFFER_SIZE - 1);
	CHECK(!Circular_Buffer_Enqueue(&cb, 0x55));   /* full: data must not be overwritten */

	for (int i = 0; i < stored; i++) {
		CHECK(Circular_Buffer_Dequeue(&cb, &value));
		CHECK(value == (uint8_t)i);
	}
	CHECK(Circular_Buffer_Is_Empty(&cb));
}

static void test_wrap_around(void)
{
	uint8_t value, next_write = 0, next_read = 0;
	int count = 0;

	Circular_Buffer_Init(&cb);
	/* uneven put/get bursts move head and tail across the end of the array many times */
	for (int round = 0; round < 20000; round++) {
		int puts = (round * 7) % 5;
		int gets = (round * 3) % 4;

		for (int i = 0; i < puts; i++) {
			if (Circular_Buffer_Enqueue(&cb, next_write)) {
				next_write++;
				count++;
			}
		}
		for (int i = 0; i < gets; i++) {
			if (Circular_Buffer_Dequeue(&cb, &value)) {
				CHECK(value == next_read);
				next_read++;
				count--;
			}
		}
		CHECK(Circular_Buffer_Count(&cb) == count);
	}
	while (Circular_Buffer_Dequeue(&cb, &value)) {
		CHECK(value == next_read);
		next_read++;
	}
	CHECK(next_read == next_write);
	CHECK(Circular_Buffer_Is_Empty(&cb));
}

int main(void)
{
	test_starts_empty();
	test_zero_is_valid_data();
	test_capacity_and_fifo_order();
	test_wrap_around();

	if (failures) {
		printf("circular_buffer: %d check(s) failed\n", failures);
		return 1;
	}
	printf("circular_buffer: all tests passed\n");
	return 0;
}
