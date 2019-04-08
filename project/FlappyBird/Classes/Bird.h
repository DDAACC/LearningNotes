#ifndef __BIRD_H__
#define  __BIRD_H__
#include "CrashObject.h"

const std::vector<std::string> imgs = {
	"bird1.png",
	"bird2.png",
	"bird3.png",
};
const float flyInterval = 0.1;
class Bird : public CrashObject
{
public:
	static Bird* create();
	Bird();
	void fly(float dt);
	void update(float dt);
	bool isStatic();
private:
	int index;
	float interval;
};
#endif // !__BIRD_H__
