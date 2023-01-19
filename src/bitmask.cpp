#include "bitmask.h"

bool BitMask::operator[](int index) const
{
	if(index < 0 || index > 63)
	{
		throw std::out_of_range("index must be in range [0,63]");
	}
	return (data & ((uint64_t)1 << index)) != 0;
}

void BitMask::Set(int index, bool value)
{
	if(value)
	{
		data |= ((uint64_t)1 << index);
	}
	else
	{
		data &= ~((uint64_t)1 << index);
	}
}

BitMask::operator uint64_t() const
{
	return data;
}
