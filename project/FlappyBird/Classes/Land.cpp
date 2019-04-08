#include "Land.h"

Land* Land::create()
{
	Land *object = new (std::nothrow) Land();
	if (object && object->initWithFile("land.png"))
	{
		object->autorelease();
		return object;
	}
	CC_SAFE_DELETE(object);
	return nullptr;
}