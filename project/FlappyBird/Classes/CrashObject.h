#ifndef __CRASH_OBJECT_H__
#define  __CRASH_OBJECT_H__

#include "cocos2d.h"
USING_NS_CC;

class CrashObject :public Sprite
{
public:
	static CrashObject* create(const std::string& filename);
	CrashObject() :speed(Vec2(0, 0)), acceleration(Vec2(0, 0))
	{}
	Vec2 getSpeed();
	void setSpeed(Vec2 newSpeed);
	void setSpeedXY(float speedX, float speedY);
	void setSpeedX(float speedX);
	void setSpeedY(float speedY);

	Vec2 getAcceleration();
	void setAcceleration(Vec2 newAcceleration);
	void setAccelerationXY(float accelerationX, float accelerationY);
	void setAccelerationX(float accelerationX);
	void setAccelerationY(float accelerationY);

	Size getSize();

	bool isTouch(CrashObject *other);
	void update(float dt);

	void updateSpeed(float dt);
	void updatePos(float dt);

	float getMinX();
	float getMaxX();
	float getMinY();
	float getMaxY();
protected:
	Vec2 speed;
	Vec2 acceleration;
};

#endif