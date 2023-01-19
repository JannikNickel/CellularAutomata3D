#include "intcell.h"
#include <algorithm>

int IntCell::statesMinusOne = 0;

IntCell::IntCell() : value(0)
{

}

IntCell::IntCell(int value) : value(value)
{

}

bool IntCell::IsAlive() const
{
    return value == statesMinusOne;
}

bool IntCell::IsEmpty() const
{
    return value == 0;
}

float IntCell::RenderGradient() const
{
    if(statesMinusOne == 1)
    {
        return 1.0f;
    }
    return (value - 1.0f) / (statesMinusOne - 1.0f);
}

IntCell::operator int() const
{
    return value;
}
