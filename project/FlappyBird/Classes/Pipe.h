#ifndef __PIPE_H__
#define  __PIPE_H__
#include "CrashObject.h"
#define PIPE_SPEED -250
class Pipe :public CrashObject
{
public:
	static Pipe* create();
	Pipe()
	{
		setSpeedX(PIPE_SPEED);
	}
private:

};
#endif