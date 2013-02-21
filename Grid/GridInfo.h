// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef GRIDINFO_H
#define GRIDINFO_H

#include <vector>
#include <sparsehash/dense_hash_set>
#include "Spatial/SpatialInfoBase.h"

namespace ssvsc
{
	class Body;
	class Grid;
	class Cell;

	class GridInfo : public SpatialInfoBase
	{
		private:
			Grid& grid;

			std::vector<Cell*> cells;
			std::vector<std::vector<Body*>*> queries; // Cell vector ptrs to query
			int startX{0}, startY{0}, endX{0}, endY{0}; // Edge cell positions
			bool invalid{false}; // IF TRUE CRASHES ON START - MUST FIX!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

			void calcEdges(); // Sets startX, startY, endX, endY
			void checkEdges(); // Checks if startXY... is different from previousStartXY... - if so, recalculates
			void calcCells(); // Clears cells/queries and gets new ones
			void clear();

		public:
			GridInfo(Grid& mGrid, Body& mBody);
			~GridInfo();

			void invalidate() override;
			void preUpdate() override;
			void postUpdate() override;
			google::dense_hash_set<Body*> getBodiesToCheck() override;
	};
}

#endif // GRIDINFO_H
