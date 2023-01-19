#pragma once
#include <vector>
#include <array>
#include <algorithm>
#include <type_traits>
#include <tuple>

#include "config.h"
#include "cell.h"

static const int NEIGHBOURS_VN[6][3] =
{
	{ 1, 0, 0 },
	{ 0, 1, 0 },
	{ 0, 0, 1 },
	{ -1, 0, 0 },
	{ 0, -1, 0 },
	{ 0, 0, -1 }
};

static const int NEIGHBOURS_MOORE[26][3] =
{
	{ -1, -1, -1 },
	{ -1, 0, -1 },
	{ -1, 1, -1 },
	{ 0, -1, -1 },
	{ 0, 0, -1 },
	{ 0, 1, -1 },
	{ 1, -1, -1 },
	{ 1, 0, -1 },
	{ 1, 1, -1 },

	{ -1, -1, 0 },
	{ -1, 0, 0 },
	{ -1, 1, 0 },
	{ 0, -1, 0 },
	{ 0, 1, 0 },
	{ 1, -1, 0 },
	{ 1, 0, 0 },
	{ 1, 1, 0 },

	{ -1, -1, 1 },
	{ -1, 0, 1 },
	{ -1, 1, 1 },
	{ 0, -1, 1 },
	{ 0, 0, 1 },
	{ 0, 1, 1 },
	{ 1, -1, 1 },
	{ 1, 0, 1 },
	{ 1, 1, 1 }
};

template<typename T>
class Grid3d
{
	static_assert(std::is_base_of<Cell, T>::value, "T must derive from Cell");

public:
	Grid3d(int dimSize, bool wrapAround, NeighbourMode neighbourMode)
	{
		this->dimSize = dimSize;
		this->wrapAround = wrapAround;
		this->dataLen = dimSize * dimSize * dimSize;
		this->data = std::vector<T>(this->dataLen, T());
		this->stepData = std::vector<T>(this->dataLen, T());
		this->requireNeighbourUpdate = false;
		this->neighbourOffsets = neighbourMode == NeighbourMode::Moore ? NEIGHBOURS_MOORE : NEIGHBOURS_VN;
		this->neighbourOffsetsLen = neighbourMode == NeighbourMode::Moore ? 26 : 6;
		this->neighbourData = std::vector<int>(this->dataLen, 0);
		this->stepNeighbourData = std::vector<int>(this->dataLen, 0);
	}

	int SetCount() const
	{
		int c = 0;
		for(int i = 0; i < dataLen; i++)
		{
			if(data[i])
			{
				c++;
			}
		}
		return c;
	}

	int GetDimSize() const
	{
		return dimSize;
	}

	std::tuple<int, int, int> GetCellPos(int index) const
	{
		return std::tuple<int, int, int>(index % dimSize, (index / dimSize) % dimSize, index / (dimSize * dimSize));
	}

	const T& operator [](int index) const
	{
		return data[index];
	}

	const T& GetCell(int x, int y, int z) const
	{
		return data[(z * dimSize * dimSize) + (y * dimSize) + x];
	}

	void SetCell(int x, int y, int z, const T& value)
	{
		data[(z * dimSize * dimSize) + (y * dimSize) + x] = value;
		requireNeighbourUpdate = true;
	}

	void UpdateNeighbours()
	{
		requireNeighbourUpdate = false;
		for(int i = 0; i < dataLen; i++)
		{
			int x = i % dimSize;
			int y = (i / dimSize) % dimSize;
			int z = i / (dimSize * dimSize);
			neighbourData[i] = CountNeighbours(x, y, z);
		}
	}

	void ChangeNeighbours(int x, int y, int z, int delta)
	{
		for(int i = 0; i < neighbourOffsetsLen; i++)
		{
			int nx = x + neighbourOffsets[i][0];
			int ny = y + neighbourOffsets[i][1];
			int nz = z + neighbourOffsets[i][2];
			if(wrapAround)
			{
				nx = (nx + dimSize) % dimSize;
				ny = (ny + dimSize) % dimSize;
				nz = (nz + dimSize) % dimSize;
			}
			else if(nx < 0 || nx >= dimSize || ny < 0 || ny >= dimSize || nz < 0 || nz >= dimSize)
			{
				continue;
			}
			int index = (nz * dimSize * dimSize) + (ny * dimSize) + nx;
			neighbourData[index] += delta;
		}
	}

	int CountNeighbours(int x, int y, int z) const
	{
		int c = 0;
		for(int i = 0; i < neighbourOffsetsLen; i++)
		{
			int nx = x + neighbourOffsets[i][0];
			int ny = y + neighbourOffsets[i][1];
			int nz = z + neighbourOffsets[i][2];
			if(wrapAround)
			{
				nx = (nx + dimSize) % dimSize;
				ny = (ny + dimSize) % dimSize;
				nz = (nz + dimSize) % dimSize;
			}
			else if(nx < 0 || nx >= dimSize || ny < 0 || ny >= dimSize || nz < 0 || nz >= dimSize)
			{
				continue;
			}
			if(GetCell(nx, ny, nz).IsAlive())
			{
				c++;
			}
		}
		return c;
	}

	typedef T (*ModFunc)(const T& cell, int neighbours);
	void Transform(ModFunc func)
	{
		if(requireNeighbourUpdate)
		{
			UpdateNeighbours();
		}

		stepNeighbourData = neighbourData;
		for(int i = 0; i < dataLen; i++)
		{
			int x = i % dimSize;
			int y = (i / dimSize) % dimSize;
			int z = i / (dimSize * dimSize);
			const T& cell = GetCell(x, y, z);
			bool wasAlive = cell.IsAlive();
			bool wasEmpty = cell.IsEmpty();
			const T& nextCell = func(cell, stepNeighbourData[i]);
			stepData[i] = nextCell;
			if(wasAlive && !nextCell.IsAlive())
			{
				ChangeNeighbours(x, y, z, -1);
			}
			if(wasEmpty && !nextCell.IsEmpty())
			{
				ChangeNeighbours(x, y, z, 1);
			}
		}
		data = stepData;
	}

private:
	int dimSize;
	bool wrapAround;
	int dataLen;
	std::vector<T> data;
	std::vector<T> stepData;
	bool requireNeighbourUpdate;
	const int(*neighbourOffsets)[3];
	int neighbourOffsetsLen;
	std::vector<int> neighbourData;
	std::vector<int> stepNeighbourData;
};
