#pragma once
class Cell
{
public:
	//Should return true, if the cell is fully alive
	virtual bool IsAlive() const = 0;
	//Should return true, if the cell is fully empty
	virtual bool IsEmpty() const = 0;
	//Should return 0 if it is about to despawn, 1 if IsAlive() and linear interpolation between 0 and 1 for anything in between
	virtual float RenderGradient() const = 0;
};