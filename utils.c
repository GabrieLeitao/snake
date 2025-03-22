#include "utils.h"

int max(int a, int b)
{
    return a > b ? a : b;
}

int clamp(int value, int min, int max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}