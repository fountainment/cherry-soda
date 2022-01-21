#ifndef _CHERRYSODA_COLLIDERS_COLLIDE_H_
#define _CHERRYSODA_COLLIDERS_COLLIDE_H_

#include <CherrySoda/Util/Calc.h>
#include <CherrySoda/Util/Math.h>
#include <CherrySoda/Util/STL.h>

namespace cherrysoda {

class Entity;

class Collide
{
public:
	static bool Check(const Entity* a, const Entity* b);
	static bool Check(Entity* a, const Entity* b, const Math::Vec2& at);

	static bool Check(const Entity* a, const STL::List<Entity*>& b);
	static bool Check(Entity* a, const STL::List<Entity*>& b, const Math::Vec2& at);

	static bool CheckPoint(const Entity* a, const Math::Vec2& point);
	static bool CheckLine(const Entity* a, const Math::Vec2& from, const Math::Vec2& to);

	static int Count(const Entity* a, const STL::List<Entity*>& b);

	static Entity* First(const Entity* a, const STL::List<Entity*>& b);

	static const STL::List<Entity*> All(const Entity* a, const STL::List<Entity*>& b);

	// Circle
	static inline bool CircleToLine(const Math::Vec2& cPosition, float cRadius, const Math::Vec2& lineFrom, const Math::Vec2& lineTo)
	{
		return Math_LengthSq(cPosition - Calc::ClosestPointOnLine(lineFrom, lineTo, cPosition)) < cRadius * cRadius;
	}

	// Rect
	static inline bool RectToPoint(float rX, float rY, float rW, float rH, const Math::Vec2& point)
	{
		return point.x >= rX && point.y >= rY && point.x < rX + rW && point.y < rY + rH;
	}
};

} // namespace cherrysoda

#endif // _CHERRYSODA_COLLIDERS_COLLIDE_H_
