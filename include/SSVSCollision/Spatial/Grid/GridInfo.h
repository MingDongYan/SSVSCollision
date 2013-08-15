// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVSC_SPATIAL_GRIDINFO
#define SSVSC_SPATIAL_GRIDINFO

#include <vector>
#include "SSVSCollision/Spatial/SpatialInfoBase.h"
#include "SSVSCollision/Spatial/Grid/Cell.h"

namespace ssvsc
{
	class Base;
	class Body;
	class Grid;

	class GridInfo : public SpatialInfoBase
	{
		private:
			Grid& grid;
			int startX{0}, startY{0}, endX{0}, endY{0}; // Edge cell positions
			int oldStartX{-1}, oldStartY{-1}, oldEndX{-1}, oldEndY{-1};
			bool invalid{true};

			void calcEdges();
			void calcCells();
			inline void clear()
			{
				for(const auto& c : cells) c->del(&base);
				cells.clear();
			}

		public:
			std::vector<Cell*> cells;
			GridInfo(Grid& mGrid, Base& mBase);

			inline void invalidate() override	{ invalid = true; }
			inline void preUpdate() override	{ if(invalid) calcEdges(); }
			inline void postUpdate() override	{ }
			inline void destroy() override		{ clear(); SpatialInfoBase::destroy(); }
			void handleCollisions(float mFrameTime) override;
	};
}

#endif
