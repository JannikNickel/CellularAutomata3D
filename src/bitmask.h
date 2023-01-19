#pragma once
#include <cstdint>
#include <stdexcept>

class BitMask
{
public:
	bool operator[](int index) const;
	void Set(int index, bool value);

	operator uint64_t() const;

private:
	uint64_t data = 0;
};