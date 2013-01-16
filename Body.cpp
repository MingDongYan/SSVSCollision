#include "Body.h"
#include "World.h"

using namespace std;
using namespace sf;

namespace ssvsc
{
	Body::Body(World* mWorld, bool mIsStatic, Vector2i mPosition, int mWidth, int mHeight) :
		world{mWorld}, isStatic{mIsStatic}, position{mPosition}, halfSize{mWidth / 2, mHeight / 2} { }

	void Body::addGroups(const vector<string>& mGroups) { for(auto group : mGroups) groups.insert(group); }
	void Body::addGroupsToCheck(const vector<string>& mGroups) { for(auto group : mGroups) groupsToCheck.insert(group); }
	void Body::addGroupsToIgnoreResolve(const vector<string>& mGroups) { for(auto group : mGroups) groupsToIgnoreResolve.insert(group); }

	bool Body::isOverlapping(Body* mBody) { return getRight() > mBody->getLeft() && getLeft() < mBody->getRight() && (getBottom() > mBody->getTop() && getTop() < mBody->getBottom()); }
	void Body::update(float mFrameTime)
	{
		if(isStatic) return;

		previousPosition = position;

		Vector2f tempVelocity{velocity.x * mFrameTime, velocity.y * mFrameTime};
		Vector2f tempPosition{position.x + tempVelocity.x, position.y + tempVelocity.y};

		position = Vector2i(tempPosition.x, tempPosition.y);

		unordered_set<Body*> checkedBodies{this};
		unordered_set<Body*> bodiesToCheck = world->getBodies(this);

		//sort bodies
		//bodiesToCheck.OrderBy(x => Velocity.X > 0 ? x.X : -x.X)

		for(Body* body : bodiesToCheck)
		{
			if(checkedBodies.find(body) != checkedBodies.end()) continue;
			checkedBodies.insert(body);

			if(!isOverlapping(body)) continue;

			cout << "collision" << endl;

			//onCollision({mFrameTime, body->getUserData(), body});
			//body->onCollision({mFrameTime, userData, this});

			for(auto g : groupsToIgnoreResolve)	if(body->getGroups().find(g) != body->getGroups().end()) continue;

			int encrX{0}, encrY{0};

			if (getBottom() < body->getBottom() && getBottom() >= body->getTop()) encrY = body->getTop() - getBottom();
			else if (getTop() > body->getTop() && getTop() <= body->getBottom()) encrY = body->getBottom() - getTop();

			if (getLeft() < body->getLeft() && getRight() >= body->getLeft()) encrX = body->getLeft() - getRight();
			else if (getRight() > body->getRight() && getLeft() <= body->getRight()) encrX = body->getRight() - getLeft();

			int overlapX = getLeft() < body->getLeft() ? getRight() - body->getLeft() : body->getRight() - getLeft();
			int overlapY = getTop() < body->getTop() ? getBottom() - body->getTop() : body->getBottom() - getTop();

			if(overlapX > overlapY) position += {0, encrY}; else position += {encrX, 0};
		}

		world->updateBody(this);
	}

	void Body::setCells(std::unordered_set<Cell*> mCells) { cells = mCells; }
	void Body::setPosition(Vector2i mPosition) { position = mPosition; }
	void Body::setVelocity(Vector2i mVelocity) { velocity = mVelocity; }
	unordered_set<Cell*> Body::getCells() { return cells; }
	unordered_set<string> Body::getGroups (){ return groups; }
	unordered_set<string> Body::getGroupsToCheck() { return groupsToCheck; }
	Vector2i Body::getPosition() { return position; }
	Vector2i Body::getVelocity() { return velocity; }
	int Body::getX() { return position.x; }
	int Body::getY() { return position.y; }
	int Body::getLeft() { return position.x - halfSize.x; }
	int Body::getRight() { return position.x + halfSize.x; }
	int Body::getTop() { return position.y - halfSize.y; }
	int Body::getBottom() { return position.y + halfSize.y; }
	int Body::getHalfWidth() { return halfSize.x; }
	int Body::getHalfHeight() { return halfSize.y; }
	int Body::getWidth() { return halfSize.x * 2; }
	int Body::getHeight() { return halfSize.y * 2; }
}

