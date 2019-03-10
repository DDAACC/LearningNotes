#ifndef __PIPE_GROUP_H__
#define  __PIPE_GROUP_H__

#include "cocos2d.h"
#include "Pipe.h"
USING_NS_CC;

#define GAP 200
class PipeGroup:public Node
{
public:
	static PipeGroup* create();
	PipeGroup();
	void update(float dt);
	void setHeight(int height);
	bool isActive();
	Pipe* getUpPipe();
	Pipe* getDownPipe();
	bool isTouch(CrashObject *other);
private:
	Pipe* upPipe;
	Pipe* downPipe;
	int scaleX;
	int scaleY;
	CCSize size;
};

#endif // !__PIPE_GROUP_H__
