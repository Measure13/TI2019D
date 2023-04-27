#pragma once

#define SET(bits, offset)   (bits = (bits | (1UL << offset)))
#define CLR(bits, offset)   (bits = (bits & (~(1UL << offset))))

#define ZERO(bits)          (bits = 0UL)
