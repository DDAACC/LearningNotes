#include "PipeGroup.h"

PipeGroup* PipeGroup::create()
{
	PipeGroup *object = new (std::nothrow) PipeGroup();
	if (object && object->init())
	{
		object->autorelease();
		return object;
	}
	CC_SAFE_DELETE(object);
	return nullptr;
}

PipeGroup::PipeGroup()
{
	scaleX = 4;
	scaleY = 3;
	upPipe = Pipe::create();
	downPipe = Pipe::create();
	upPipe->setScaleY(scaleY);
	upPipe->setScaleX(-scaleX);
	downPipe->setScaleY(scaleY);
	downPipe->setScaleX(scaleX);
	addChild(upPipe);
	addChild(downPipe);
	upPipe->setRotation(180);

	auto visibleSize = Director::getInstance()->getVisibleSize();
	size = upPipe->getContentSize();
	size.width *= scaleX;
	size.height *= scaleY;
	upPipe->setPosition(size.width / 2 + visibleSize.width, visibleSize.height - size.height / 2);
	downPipe->setPosition(size.width / 2 + visibleSize.width, size.height / 2);
}

void PipeGroup::update(float dt)
{
	upPipe->update(dt);
	downPipe->update(dt);
}

bool PipeGroup::isActive()
{
	auto pos = upPipe->getPosition();
	return pos.x >= -size.width;
}

Pipe* PipeGroup::getUpPipe()
{
	return upPipe;
}

Pipe* PipeGroup::getDownPipe()
{
	return downPipe;
}

void PipeGroup::setHeight(int height)
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto posY = size.height / 2 - height;
	downPipe->setPositionY(posY);

	posY += + size.height + GAP;
	upPipe->setPositionY(posY);
}

bool PipeGroup::isTouch(CrashObject *other)
{
	return upPipe->isTouch(other) || downPipe->isTouch(other);
}