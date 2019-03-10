#ifndef  __LAND_H__
#define  __LAND_H__
#include "CrashObject.h"
#define LAND_SPEED -250
class Land :public CrashObject
{
public:
	static Land* create();
	Land()
	{
		setSpeedX(LAND_SPEED);
	}
private:
};
#endif // ! __LAND_H__
