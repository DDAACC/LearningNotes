#ifndef __MAINSCENE_H__
#define __MAINSCENE_H__

#include "cocos2d.h"
#include "Land.h"
#include "Bird.h"
#include "PipeGroup.h"
#include "ui/UIImageView.h"
#include "ui/UIButton.h"
#include <vector>
USING_NS_CC;
enum STATUS
{
	WAIT = 1,
	PLAY = 2,
	END = 3,
};

#define LAND_ORDER 10
#define PIPE_ORDER 3
#define SKY_ORDER 1
#define BIRD_ORDER 5
#define UI_ORDER 15

#define IMG_SCALE 2.7
#define BIRD_SCALE 3.5
#define GAME_END_SACLE 3.0
#define BIRD_SHAKE_SPEED 60
#define PIPE_GENERATE_INTERVAL 2
#define BIRD_FLY_ACCELERATION -1400
class MainScene : public Layer
{
public:
    static Scene* createScene();
	CREATE_FUNC(MainScene);
	virtual bool init();
	void initBg();
	void initBird();
	void initLabel();
	void initEnd();

	void updateStatus(int status);
	void updateScore(int num);

	void updateByFrame(float dt);
	void updateLand(float dt);
	void shakeBird(float dt);
	void flyBird(float dt);
	void updatePipe(float dt);
	void generatePipe();

	void gameEnd();
	void clear();


	std::string intToStr(int num);

	void onClickStart(Ref* pSender);
	bool onTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	void onTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	void onTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	void onTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
private:
	ui::ImageView* imgSky1; // 天空1
	Vector<Land*> lands; // 地面
	Bird* bird; // 鸟
	ui::ImageView* imgGameOver;
	ui::Button* buttonStart;
	Vector<PipeGroup*> pipeGroups; // 水管
	Label* scoreLabel;
	PipeGroup* nextPipe = nullptr;

	CCSize visibleSize;
	int status;
	float curInterval = 0;
	bool hasAddScore = false;
	int score;
};

#endif // __MAINSCENE_H__
