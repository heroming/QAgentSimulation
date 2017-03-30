#include "Tool.h"

#include <algorithm>

const double Tool::EPS = 1e-6;

int Tool::sgn(const double e)
{
    return (int)(e > EPS) - (e < -EPS);
}

float Tool::distance(const float x, const float y, const float a, const float b)
{
    return std::sqrt((x - a) * (x - a) + (y - b) * (y - b));
}