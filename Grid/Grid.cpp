// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "Grid.h"
#include "GridInfo.h"
#include "Cell.h"

using namespace std;

namespace ssvsc
{
	Grid::Grid(int mColumns, int mRows, int mCellSize, int mOffset) : columns{mColumns}, rows{mRows}, cellSize{mCellSize}, offset{mOffset}
	{
		for(int iX{0}; iX < columns; iX++)
		{
			cells.push_back(vector<Cell*>(rows));
			for(int iY{0}; iY < rows; iY++)
			{
				int left{iX * cellSize}, right{cellSize + left}, top{iY * cellSize}, bottom{cellSize + top};
				cells[iX][iY] = new Cell{left, right, top, bottom};
			}
		}
	}
	Grid::~Grid()
	{
		memoryManager.clear();
		for(auto& vector : cells) for(auto& cell : vector) delete cell;
	}

	SpatialInfoBase& Grid::createSpatialInfo(Body& mBody) { return memoryManager.create(*this, mBody); }
	void Grid::delSpatialInfo(SpatialInfoBase& mSpatialInfo)
	{
		memoryManager.del(&(static_cast<GridInfo&>(mSpatialInfo))); 
		memoryManager.cleanUp();
	}

	Cell* Grid::getCell(int mX, int mY) { return cells[mX + offset][mY + offset]; }
	int Grid::getIndex(int mValue) const { return mValue / cellSize; }
	bool Grid::isOutside(int mStartX, int mStartY, int mEndX, int mEndY) const
	{
		return mStartX < 0 - offset || mEndX >= columns - offset || mStartY < 0 - offset || mEndY >= rows - offset;
	}
}

