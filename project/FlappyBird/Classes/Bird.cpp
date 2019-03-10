#include "Bird.h"

Bird* Bird::create()
{
	Bird *object = new (std::nothrow) Bird();
	if (object && object->initWithFile("bird1.png"))
	{
		object->autorelease();
		return object;
	}
	CC_SAFE_DELETE(object);
	return nullptr;
}

Bird::Bird()
{
	index = 0;
	interval = 0;
}
void Bird::fly(float dt)
{
	auto rotate = getRotation();
	if (rotate < 90)
	{
		interval += dt;
		if (interval >= flyInterval)
		{
			interval -= flyInterval;
			++index;
			index %= 3;
			auto szPath = imgs[index];
			setTexture(szPath);
		}
	}
}

void Bird::update(float dt)
{
	auto oldSpeed = speed;
	updateSpeed(dt);
	updatePos(dt);
	float critical = -300;
	if (oldSpeed.y > critical && speed.y <= critical && acceleration.y < 0)
	{
		stopAllActions();
		auto actionRotate = CCRotateTo::create(0.3, 90);
		runAction(actionRotate);
	}
}

bool Bird::isStatic()
{	
	return speed.y == 0 && acceleration.y == 0;
}
