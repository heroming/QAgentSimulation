#include "Tool.h"

const double Tool::EPS = 1e-6;

int Tool::sgn(const double e)
{
    return (int)(e > EPS) - (e < -EPS);
}