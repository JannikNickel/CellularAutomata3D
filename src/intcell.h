#pragma once
#include "cell.h"

class IntCell : public Cell
{
public:
	static int statesMinusOne;

	IntCell();
	IntCell(int value);

	bool IsAlive() const override;
	bool IsEmpty() const override;
	float RenderGradient() const override;

	operator int() const;

private:
	int value = 0;
};