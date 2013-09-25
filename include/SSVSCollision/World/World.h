// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVSC_WORLD
#define SSVSC_WORLD

#include <vector>
#include "SSVSCollision/Global/Typedefs.h"
#include "SSVSCollision/Query/Query.h"

namespace ssvsc
{
	class Base;
	class Body;
	class Sensor;
	struct ResolverBase;
	struct SpatialBase;

	class World
	{
		friend class Base;
		friend class Body;
		friend class Sensor;

		private:
			ssvu::MemoryManager<Base> bases;

			Uptr<ResolverBase> resolver;
			Uptr<SpatialBase> spatial;

			std::vector<Body*> bodies;
			std::vector<Sensor*> sensors;

			inline void del(Base& mBase) { bases.del(mBase); }

		public:
			World(ResolverBase& mResolver, SpatialBase& mSpatial) : resolver(&mResolver), spatial(&mSpatial) { }

			inline Body& create(const Vec2i& mPosition, const Vec2i& mSize, bool mIsStatic)
			{
				auto& result(bases.create<Body>(*this, mIsStatic, mPosition, mSize));
				bodies.push_back(&result); return result;
			}
			inline Sensor& createSensor(const Vec2i& mPosition, const Vec2i& mSize)
			{
				auto& result(bases.create<Sensor>(*this, mPosition, mSize));
				sensors.push_back(&result); return result;
			}

			void update(float mFT);
			inline void clear() { bases.clear(); bodies.clear(); sensors.clear(); }

			inline decltype(bases)::Container& getBases() noexcept	{ return bases.getItems(); }
			inline ResolverBase& getResolver() noexcept				{ return *resolver; }
			inline SpatialBase& getSpatial() noexcept				{ return *spatial; }
			inline std::vector<Body*>& getBodies() noexcept			{ return bodies; }
			inline std::vector<Sensor*>& getSensors() noexcept		{ return sensors; }

			template<typename T> inline T& getResolver()	{ return static_cast<T&>(getResolver()); }
			template<typename T> inline T& getSpatial()		{ return static_cast<T&>(getSpatial()); }

			template<typename TSpatial, QueryType TType, QueryMode TMode = QueryMode::All, typename... TArgs>
			inline Query<TSpatial, typename QueryTypeDispatcher<TSpatial, TType>::Type, typename QueryModeDispatcher<TSpatial, TMode>::Type> getQuery(TArgs&&... mArgs)
			{
				return {getSpatial<TSpatial>(), std::forward<TArgs>(mArgs)...};
			}
	};

	// TODO: templatize?
}

#endif
