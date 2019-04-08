#include "CrashObject.h"

CrashObject* CrashObject::create(const std::string& fileName)
{
	CrashObject *object = new (std::nothrow) CrashObject();
	if (object && object->initWithFile(fileName))
	{
		object->autorelease();
		return object;
	}
	CC_SAFE_DELETE(object);
	return nullptr;
}

cocos2d::Vec2 CrashObject::getSpeed()
{
	return speed;
}
void CrashObject::setSpeed(cocos2d::Vec2 newSpeed)
{
	speed = newSpeed;
}
void CrashObject::setSpeedXY(float speedX, float speedY)
{
	setSpeed(cocos2d::Vec2(speedX, speedY));
}
void CrashObject::setSpeedX(float speedX)
{
	setSpeedXY(speedX, speed.y);
}
void CrashObject::setSpeedY(float speedY)
{
	setSpeedXY(speed.x, speedY);
}

cocos2d::Vec2 CrashObject::getAcceleration()
{
	return acceleration;
}

void CrashObject::setAcceleration(cocos2d::Vec2 newAcceleration)
{
	acceleration = newAcceleration;
}

void CrashObject::setAccelerationXY(float accelerationX, float accelerationY)
{
	setAcceleration(cocos2d::Vec2(accelerationX, accelerationY));
}

void CrashObject::setAccelerationX(float accelerationX)
{
	setAccelerationXY(accelerationX, acceleration.y);
}

void CrashObject::setAccelerationY(float accelerationY)
{
	setAccelerationXY(acceleration.x, accelerationY);
}

Size CrashObject::getSize()
{
	auto contentSize = getContentSize();
	return Size(contentSize.width * abs(getScaleX()), contentSize.height * abs(getScaleY()));
}

bool CrashObject::isTouch(CrashObject *other)
{
	cocos2d::Vec2 myPos = getPosition();
	cocos2d::Vec2 myAnchorPt = getAnchorPoint();
	cocos2d::Size mySize = getSize();

	cocos2d::Vec2 otherPos = other->getPosition();
	cocos2d::Vec2 otherAnchorPt = other->getAnchorPoint();
	cocos2d::Size otherSize = other->getSize();

	double myMinX = myPos.x - myAnchorPt.x * mySize.width;
	double myMaxX = myPos.x + (1 - myAnchorPt.x) * mySize.width;
	double myMinY = myPos.y - myAnchorPt.y * mySize.height;
	double myMaxY = myPos.y + (1 - myAnchorPt.y) * mySize.height;

	double otherMinX = otherPos.x - otherAnchorPt.x * otherSize.width;
	double otherMaxX = otherPos.x + (1 - otherAnchorPt.x) * otherSize.width;
	double otherMinY = otherPos.y - otherAnchorPt.y * otherSize.height;
	double otherMaxY = otherPos.y + (1 - otherAnchorPt.y) * otherSize.height;

	double minX = MAX(myMinX, otherMinX);
	double minY = MAX(myMinY, otherMinY);
	double maxX = MIN(myMaxX, otherMaxX);
	double maxY = MIN(myMaxY, otherMaxY);

	return minX < maxX && minY < maxY;
}

void CrashObject::update(float dt)
{
	updateSpeed(dt);
	updatePos(dt);
}

void CrashObject::updateSpeed(float dt)
{
	speed += acceleration * dt;
}

void CrashObject::updatePos(float dt)
{
	auto pos = getPosition();
	setPosition(pos.x + speed.x * dt, pos.y + speed.y * dt);
}

float CrashObject::getMinX()
{
	auto posX = getPositionX();
	auto anchorPointX = getAnchorPoint().x;
	return posX - getSize().width * anchorPointX;
}

float CrashObject::getMaxX()
{
	return getMinX() + getSize().width;
}

float CrashObject::getMinY()
{
	auto posY = getPositionY();
	auto anchorPointY = getAnchorPoint().y;
	return posY - getSize().height * anchorPointY;
}

float CrashObject::getMaxY()
{
	return getMinY() + getSize().height;
}