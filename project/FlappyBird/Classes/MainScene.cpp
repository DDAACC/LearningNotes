#include "MainScene.h"
#include "SimpleAudioEngine.h"
#include <time.h>


Scene* MainScene::createScene()
{
    auto scene = Scene::create();
	auto layer = MainScene::create();
    scene->addChild(layer);

    return scene;
}

bool MainScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
	srand(time(0));

	initBg();
	initBird();
	initLabel();
	initEnd();
	updateStatus(WAIT);

	schedule(schedule_selector(MainScene::updateByFrame));

	auto touchlistener = EventListenerTouchOneByOne::create();
	touchlistener->onTouchBegan = CC_CALLBACK_2(MainScene::onTouchBegan, this);
	touchlistener->onTouchMoved = CC_CALLBACK_2(MainScene::onTouchMoved, this);
	touchlistener->onTouchEnded = CC_CALLBACK_2(MainScene::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithFixedPriority(touchlistener, 1.0);

	return true;
}

void MainScene::initBg()
{
	imgSky1 = ui::ImageView::create("sky.png");
	imgSky1->setScale(IMG_SCALE);
	this->addChild(imgSky1, SKY_ORDER);

	auto skySize = imgSky1->getContentSize() * IMG_SCALE;
	auto skyHeight = visibleSize.height - skySize.height / 2;
	imgSky1->setPosition(Vec2(skySize.width / 2, skyHeight));

	for (int i = 0; i < 3; ++i)
	{
		Land* newLand = Land::create();
		newLand->setScale(IMG_SCALE);
		this->addChild(newLand, LAND_ORDER);

		auto landSize = newLand->getSize();
		auto landHeight = visibleSize.height - skySize.height + landSize.height / 2;
		newLand->setPosition(Vec2(landSize.width / 2 * (2 * i + 1), landHeight));

		lands.pushBack(newLand);
	}
}

void MainScene::initBird()
{
	bird = Bird::create();
	bird->setScale(BIRD_SCALE);
	addChild(bird, BIRD_ORDER);

	bird->setPosition(Vec2(visibleSize.width / 2 - 100, visibleSize.height / 2));
}

void MainScene::initLabel()
{
	scoreLabel = Label::create();
	scoreLabel->setSystemFontSize(50);

	scoreLabel->setPosition(visibleSize.width / 2, visibleSize.height * 3 / 4);
	addChild(scoreLabel, UI_ORDER);
	updateScore(0);
}

void MainScene::initEnd()
{
	imgGameOver = ui::ImageView::create("gameover.png");
	imgGameOver->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 4 / 5));
	imgGameOver->setScale(GAME_END_SACLE);
	addChild(imgGameOver, UI_ORDER);
	auto size = imgGameOver->getSize();

	auto imgBoard = ui::ImageView::create("board.png");
	imgGameOver->addChild(imgBoard);
	auto boardSize = imgBoard->getSize();
	imgBoard->setPosition(Vec2(size.width / 2, -boardSize.height / 2 - size.height / 2));
	imgBoard->setName("imgBoard");

	buttonStart = ui::Button::create("start_btn.png", "start_btn_pressed.png");
	imgGameOver->addChild(buttonStart);
	buttonStart->addClickEventListener(CC_CALLBACK_1(MainScene::onClickStart, this));

	auto buttonSize = buttonStart->getSize();
	buttonStart->setPosition(Vec2(size.width / 2, -boardSize.height - size.height * 2));
	auto labelScore = Label::create();
	labelScore->setSystemFontSize(12);
	imgBoard->addChild(labelScore);
	labelScore->setString("0");
	labelScore->setName("labelScore");
	labelScore->setPosition(Vec2(105, 39));

	auto labelBest = Label::create();
	labelBest->setSystemFontSize(12);
	imgBoard->addChild(labelBest);
	labelBest->setString("0");
	labelBest->setName("labelBest");
	labelBest->setPosition(Vec2(105, 16));
}

void MainScene::updateStatus(int newStatus)
{
	if (status != newStatus)
	{
		status = newStatus;
		imgGameOver->setVisible(false);
		scoreLabel->setVisible(false);
		switch (status)
		{
		case WAIT:
			clear();
			bird->stopAllActions();
			bird->setRotation(0);
			bird->setAccelerationY(0);
			bird->setSpeedY(- BIRD_SHAKE_SPEED);
			bird->setPositionY(visibleSize.height / 2);
			break;
		case PLAY:
			scoreLabel->setVisible(true);
			updateScore(0);
			hasAddScore = false;
			bird->setSpeedY(0);
			bird->setAccelerationY(BIRD_FLY_ACCELERATION);
			break;
		case END:
			imgGameOver->stopAllActions();
			imgGameOver->setVisible(true);
			imgGameOver->setPosition(cocos2d::Vec2(visibleSize.width / 2, visibleSize.height * 4 / 5));
			auto actionMove = CCMoveBy::create(0.5, cocos2d::Vec2(0, -50));
			imgGameOver->runAction(actionMove);

			auto imgBoard = imgGameOver->getChildByName("imgBoard");
			if (imgBoard)
			{
				auto labelScore = static_cast<Label*>(imgBoard->getChildByName("labelScore"));
				if (labelScore)
				{
					std::string text = intToStr(score);
					labelScore->setString(text);
				}
			}
			break;
		}
	}
}

void MainScene::updateByFrame(float dt)
{
	switch (status)
	{
	case WAIT:
		bird->fly(dt);
		updateLand(dt);
		shakeBird(dt);
		break;
	case PLAY:
		bird->fly(dt);
		updateLand(dt);
		updatePipe(dt);
		flyBird(dt);
		break;
	case END:
		flyBird(dt);
		break;
	default:
		break;
	}
}

void MainScene::updateLand(float dt)
{
	for (auto land: lands)
	{
		land->update(dt);
	}
	int length = lands.size();
	for (int i = 0; i < 3; ++i)
	{
		auto land = lands.at(i);
		auto landSize = land->getSize();
		auto posX = land->getPositionX();
		if (posX < -landSize.width / 2)
		{
			int nextIndex = i == 0 ? length - 1 : i - 1;
			auto nextLand = lands.at(nextIndex);
			auto nextPosX = nextLand->getPositionX();
			land->setPositionX(nextPosX + landSize.width);
		}
	}
}

void MainScene::shakeBird(float dt)
{
	bird->update(dt);

	auto posY = bird->getPositionY();
	if (posY > visibleSize.height / 2 + 10)
	{
		bird->setSpeedY(-BIRD_SHAKE_SPEED);
	}
	else if (posY < visibleSize.height / 2 - 10)
	{
		bird->setSpeedY(BIRD_SHAKE_SPEED);
	}
}

void MainScene::flyBird(float dt)
{
	if (!bird->isStatic())
	{
		bird->update(dt);
		if (lands.at(0)->getMaxY() > bird->getMinY())
		{
			bird->setSpeedY(0);
			bird->setAccelerationY(0);
			gameEnd();
			return;
		}
	}
}

void MainScene::generatePipe()
{
	auto rand = std::rand() % 400;
	PipeGroup* newGroup = PipeGroup::create();
	newGroup->setHeight(rand);
	newGroup->setPosition(0, 0);
	addChild(newGroup, PIPE_ORDER);
	pipeGroups.pushBack(newGroup);
	if (nextPipe == nullptr)
	{
		nextPipe = newGroup;
	}
}

void MainScene::updatePipe(float dt)
{
	curInterval += dt;
	if (curInterval >= PIPE_GENERATE_INTERVAL)
	{
		generatePipe();
		curInterval -= PIPE_GENERATE_INTERVAL;
	}

	for (auto group : pipeGroups)
	{
		group->update(dt);
	}

	if (pipeGroups.size() > 0)
	{
		auto it = pipeGroups.begin();
		auto pipe = *it;
		if (!pipe->isActive())
		{
			pipe->removeAllChildrenWithCleanup(true);
			pipe = nullptr;
			pipeGroups.erase(it);
		}
	}

	if (nextPipe != nullptr)
	{
		auto downPipe = nextPipe->getDownPipe();
		auto pipeMaxX = downPipe->getMaxX();
		auto pipeMinX = downPipe->getMinX();
		auto birdMinX = bird->getMinX();
		if (!hasAddScore && pipeMinX < birdMinX)
		{
			updateScore(score + 1);
			hasAddScore = true;
		}
		else if (pipeMaxX < birdMinX)
		{
			for (auto it = pipeGroups.begin(); it < pipeGroups.end(); ++it)
			{
				auto pipe = *it;
				if (pipe->getDownPipe()->getMaxX() > birdMinX)
				{
					nextPipe = pipe;
					hasAddScore = false;
					break;
				}
			}
		}

		if (nextPipe->isTouch(bird))
		{
			gameEnd();
		}
	}
}

void MainScene::updateScore(int num)
{
	score = num;
	std::string text = intToStr(score);
	scoreLabel->setString(text);
}

void MainScene::gameEnd()
{
	updateStatus(END);
}

void MainScene::clear()
{
	nextPipe = nullptr;

	for (auto pipeGroup : pipeGroups)
	{
		pipeGroup->removeFromParentAndCleanup(true);
	}

	pipeGroups.clear();
}

void MainScene::onClickStart(Ref* pSender)
{
	updateStatus(WAIT);
}

bool MainScene::onTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	return true;
}

void MainScene::onTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{

}

void MainScene::onTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	switch (status)
	{
	case WAIT:
		updateStatus(PLAY);
		bird->stopAllActions();
		bird->setSpeedY(600);
		bird->setRotation(-25);
		break;
	case PLAY:
		bird->stopAllActions();
		bird->setSpeedY(600);
		bird->setRotation(-25);
		break;
	case END:
		break;
	default:
		break;
	}
}

void MainScene::onTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{

}

std::string MainScene::intToStr(int num)
{
	if (num <= 0)
	{
		return "0";
	}
	std::string result = "";

	do
	{
		int number = num % 10;
		switch (number)
		{
		case 0:
			result = "0" + result;
			break;
		case 1:
			result = "1" + result;
			break;
		case 2:
			result = "2" + result;
			break;
		case 3:
			result = "3" + result;
			break;
		case 4:
			result = "4" + result;
			break;
		case 5:
			result = "5" + result;
			break;
		case 6:
			result = "6" + result;
			break;
		case 7:
			result = "7" + result;
			break;
		case 8:
			result = "8" + result;
			break;
		case 9:
			result = "9" + result;
			break;
		}
		num /= 10;
	} while (num > 0);

	return result;
}

