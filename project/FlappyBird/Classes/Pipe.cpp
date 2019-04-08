#include "Pipe.h"

Pipe* Pipe::create()
{
	Pipe *object = new (std::nothrow) Pipe();
	if (object && object->initWithFile("pipe1.png"))
	{
		object->autorelease();
		return object;
	}
	CC_SAFE_DELETE(object);
	return nullptr;
}