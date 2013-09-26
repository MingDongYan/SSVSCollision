// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVSC_SPATIAL_GRIDQUERYTYPES
#define SSVSC_SPATIAL_GRIDQUERYTYPES

#include <vector>
#include <queue>
#include "SSVSCollision/Global/Typedefs.h"
#include "SSVSCollision/Body/Body.h"
#include "SSVSCollision/Utils/Utils.h"

namespace ssvsc
{
	namespace GridQueryTypes
	{
		template<typename TGrid> struct Base
		{
			TGrid& grid;
			Vec2f startPos, pos, lastPos;
			Vec2i startIndex, index;

			Base(TGrid& mGrid, const Vec2i& mPos) : grid(mGrid), startPos{mPos}, pos{mPos}, startIndex{grid.getIndex(Vec2i(mPos))}, index{startIndex} { }

			inline void reset()					{ pos = startPos; index = startIndex; }
			inline const Vec2f& getLastPos()	{ return lastPos; }
		};

		namespace Bodies
		{
			struct All
			{
				template<typename T> inline static void getBodies(std::vector<Body*>& mBodies, const T& mInternal) { mBodies = mInternal.grid.getCell(mInternal.index).getBodies(); }
			};
			struct ByGroup
			{
				template<typename T> inline static void getBodies(std::vector<Body*>& mBodies, const T& mInternal, Group mGroup)
				{
					std::vector<Body*> result;
					for(const auto& b : mInternal.grid.getCell(mInternal.index).getBodies()) if(b->hasGroup(mGroup)) result.push_back(b);
					mBodies = result;
				}
			};
		}

		template<typename TGrid> struct OrthoLeft : public Base<TGrid>
		{
			template<typename... TArgs> OrthoLeft(TArgs&&... mArgs) : Base<TGrid>(std::forward<TArgs>(mArgs)...) { }
			inline bool isValid()									{ return this->index.x >= this->grid.getIndexXMin(); }
			inline void step()										{ --this->index.x; }
			inline bool getSorting(const Body* mA, const Body* mB)	{ return mA->getPosition().x < mB->getPosition().x; }
			inline bool hits(const AABB& mShape)					{ return mShape.getLeft() <= this->pos.x && this->pos.y >= mShape.getTop() && this->pos.y <= mShape.getBottom(); }
			inline void setOut(const AABB& mShape)					{ this->lastPos = Vec2f(mShape.getRight(), this->pos.y); }
		};
		template<typename TGrid> struct OrthoRight : public Base<TGrid>
		{
			template<typename... TArgs> OrthoRight(TArgs&&... mArgs) : Base<TGrid>(std::forward<TArgs>(mArgs)...) { }
			inline bool isValid()									{ return this->index.x < this->grid.getIndexXMax(); }
			inline void step()										{ ++this->index.x; }
			inline bool getSorting(const Body* mA, const Body* mB)	{ return mA->getPosition().x > mB->getPosition().x; }
			inline bool hits(const AABB& mShape)					{ return mShape.getRight() >= this->pos.x && this->pos.y >= mShape.getTop() && this->pos.y <= mShape.getBottom(); }
			inline void setOut(const AABB& mShape)					{ this->lastPos = Vec2f(mShape.getLeft(), this->pos.y); }
		};
		template<typename TGrid> struct OrthoUp : public Base<TGrid>
		{
			template<typename... TArgs> OrthoUp(TArgs&&... mArgs) : Base<TGrid>(std::forward<TArgs>(mArgs)...) { }
			inline bool isValid()									{ return this->index.y >= this->grid.getIndexYMin(); }
			inline void step()										{ --this->index.y; }
			inline bool getSorting(const Body* mA, const Body* mB)	{ return mA->getPosition().y < mB->getPosition().y; }
			inline bool hits(const AABB& mShape)					{ return mShape.getTop() <= this->pos.y && this->pos.x >= mShape.getLeft() && this->pos.x <= mShape.getRight(); }
			inline void setOut(const AABB& mShape)					{ this->lastPos = Vec2f(this->pos.x, mShape.getBottom()); }
		};
		template<typename TGrid> struct OrthoDown : public Base<TGrid>
		{
			template<typename... TArgs> OrthoDown(TArgs&&... mArgs) : Base<TGrid>(std::forward<TArgs>(mArgs)...) { }
			inline bool isValid()									{ return this->index.y < this->grid.getIndexYMax(); }
			inline void step()										{ ++this->index.y; }
			inline bool getSorting(const Body* mA, const Body* mB)	{ return mA->getPosition().y > mB->getPosition().y; }
			inline bool hits(const AABB& mShape)					{ return mShape.getBottom() >= this->pos.y && this->pos.x >= mShape.getLeft() && this->pos.x <= mShape.getRight(); }
			inline void setOut(const AABB& mShape)					{ this->lastPos = Vec2f(this->pos.x, mShape.getTop()); }
		};

		template<typename TGrid> struct Point : public Base<TGrid>
		{
			bool finished{false};

			template<typename... TArgs> Point(TArgs&&... mArgs) : Base<TGrid>(std::forward<TArgs>(mArgs)...) { }

			inline bool isValid()										{ return !finished && this->grid.isIndexValid(this->index); }
			inline void step()											{ finished = true; }
			inline bool getSorting(const Body*, const Body*)			{ return true; }
			inline bool hits(const AABB& mShape)						{ return mShape.contains(Vec2i(this->pos)); }
			inline void setOut(const AABB&)								{ }
		};

		template<typename TGrid> struct RayCast : public Base<TGrid>
		{
			int cellSize;
			Vec2i next;
			Vec2f dir, deltaDist, increment, max;

			RayCast(TGrid& mGrid, const Vec2i& mPos, const Vec2f& mDir) : Base<TGrid>{mGrid, mPos}, cellSize{this->grid.getCellSize()}, dir{ssvs::getNormalized(mDir)},
				increment{dir * static_cast<float>(cellSize)}, max{Vec2f(this->startIndex * cellSize) - this->startPos}
			{
				next.x = dir.x < 0 ? -1 : 1;
				next.y = dir.y < 0 ? -1 : 1;
				if(dir.x >= 0) max.x += cellSize;
				if(dir.y >= 0) max.y += cellSize;

				if(dir.x != 0)
				{
					max.x /= dir.x;
					deltaDist.x = cellSize / std::abs(dir.x);
				}

				if(dir.y != 0)
				{
					max.y /= dir.y;
					deltaDist.y = cellSize / std::abs(dir.y);
				}
			}

			inline bool isValid() { return this->grid.isIndexValid(this->index); }
			inline void step()
			{
				this->lastPos = this->pos;
				this->pos += increment;

				if(max.x < max.y)	{ max.x += deltaDist.x; this->index.x += next.x; }
				else				{ max.y += deltaDist.y; this->index.y += next.y; }
			}
			inline bool getSorting(const Body* mA, const Body* mB)
			{
				const auto& aPos(mA->getPosition());
				const auto& bPos(mB->getPosition());
				return pow((aPos.x - this->startPos.x), 2) + pow((aPos.y - this->startPos.y), 2) > pow((bPos.x - this->startPos.x), 2) + pow((bPos.y - this->startPos.y), 2);
			}
			bool hits(const AABB& mShape)
			{
				Segment<float> ray{this->startPos, this->pos};
				Segment<float> test1{dir.x > 0 ? mShape.getSegmentLeft<float>() : mShape.getSegmentRight<float>()};
				Segment<float> test2{dir.y > 0 ? mShape.getSegmentTop<float>() : mShape.getSegmentBottom<float>()};

				Vec2f intersection;
				if(Utils::isSegmentInsersecting(ray, test1, intersection) || Utils::isSegmentInsersecting(ray, test2, intersection))
				{
					this->lastPos = intersection;
					return true;
				}

				return false;
			}
			inline void setOut(const AABB&) { }
		};



		template<typename TGrid> struct Distance : public Base<TGrid>
		{
			int cellSize, distance, cellRadius;
			std::queue<Vec2i> offsets;

			Distance(TGrid& mGrid, const Vec2i& mPos, int mDistance) : Base<TGrid>{mGrid, mPos}, cellSize{this->grid.getCellSize()}, distance{mDistance}, cellRadius{distance / cellSize}
			{
				for(int iRadius{0}; iRadius < cellRadius + 1; ++iRadius)
				{
					for(int iY{-iRadius}; iY <= iRadius; ++iY)
					{
						offsets.emplace(iRadius, iY);
						if(-iRadius != iRadius) offsets.emplace(-iRadius, iY);
					}
					for(int iX{-iRadius + 1}; iX <= iRadius -1; ++iX)
					{
						offsets.emplace(iX, iRadius);
						offsets.emplace(iX, -iRadius);
					}
				}
			}

			inline bool isValid() { return !offsets.empty() && this->grid.isIndexValid(this->index); }
			inline void step()
			{
				this->lastPos = this->pos;
				this->index = this->startIndex + offsets.front();
				if(!offsets.empty()) offsets.pop();
			}
			inline bool getSorting(const Body* mA, const Body* mB)
			{
				const auto& aPos(mA->getPosition());
				const auto& bPos(mB->getPosition());
				return pow((aPos.x - this->startPos.x), 2) + pow((aPos.y - this->startPos.y), 2) > pow((bPos.x - this->startPos.x), 2) + pow((bPos.y - this->startPos.y), 2);
			}
			bool hits(const AABB& mShape)
			{
				int testX{this->startPos.x < mShape.getX() ? mShape.getLeft() : mShape.getRight()};
				int testY{this->startPos.y < mShape.getY() ? mShape.getTop() : mShape.getBottom()};

				if(pow((testX - this->startPos.x), 2) + pow((testY - this->startPos.y), 2) > pow(distance, 2)) return false;

				this->lastPos = Vec2f(testX, testY);
				return true;
			}
			inline void setOut(const AABB&) { }
		};
	}
}

#endif
