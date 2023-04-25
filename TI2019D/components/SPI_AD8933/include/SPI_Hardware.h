#pragma once

#include <driver/gpio.h>

#define CS          GPIO_NUM_15
#define FSYNC       GPIO_NUM_16
#define SCLK        GPIO_NUM_14
#define MOSI        GPIO_NUM_13

#define BITS_MASK   ((1UL << FSYNC) | (1UL << SCLK) | (1UL << CS) | (1UL << MOSI))

#define SELECT_DEVICE()     gpio_set_level(CS, 1)
#define RELEASE_DEVICE()    gpio_set_level(CS, 0)
#define CS_1()              gpio_set_level(CS, 1)
#define CS_0()              gpio_set_level(CS, 0)
#define FSYNC_1()           gpio_set_level(FSYNC, 1)
#define FSYNC_0()           gpio_set_level(FSYNC, 0)
#define SCLK_1()            gpio_set_level(SCLK, 1)
#define SCLK_0()            gpio_set_level(SCLK, 0)
#define MOSI_1()            gpio_set_level(MOSI, 1)
#define MOSI_0()            gpio_set_level(MOSI, 0)