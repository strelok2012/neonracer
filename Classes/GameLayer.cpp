/* 
 * File:   GameLayer.cpp
 * Author: sbkarr
 * 
 * Created on 14 Декабрь 2012 г., 17:56
 */
#include <stdlib.h>
#include <math.h>

#include "GameLayer.h"
#include "GameMenu.h"

#define GAME_ACCEL_MOD 1
#define GAME_ACCEL_COUNT 3

using namespace cocos2d;

class MovingAverageFunction {
protected:
    int _count;
    float *_values;
public:
    MovingAverageFunction(int count) : _count(count) {
        _values = new float[_count];
        memset(_values, 0, sizeof(float) * _count);
    }
    ~MovingAverageFunction() {
        delete [] _values;
    }
    
    virtual void dropValues() {
        for (int i = 0; i < _count; i++) {
            _values[i] = 0;
        }
    }
    virtual void addValue(float value) {
        for (int i = 1; i < _count; i++) {
            _values[i-1] = _values[i];
        }
        _values[_count-1] = value;
    }
    virtual float getAverage() {
        float s = 0;
        for (int i = 0; i < _count; i++) {
            s += _values[i];
        }
        return s / _count;
    }
    virtual float increment(float value) {
        float s = 0;
        for (int i = 1; i < _count; i++) {
            s += (_values[i - 1] = _values[i]);
        }
        s += (_values[_count-1] = value);
        return s / _count;
    }
    
};

CCScene * GameLayer::scene() {
	CCScene *scene = CCScene::create();
	scene->addChild(GameLayer::create());
	return scene;
}

GameLayer::~GameLayer() {
    CCDirector::sharedDirector()->getScheduler()->unscheduleAllForTarget(this);
    CC_SAFE_DELETE(currentAcceleration);
}

bool GameLayer::init() {
	if (!CCLayer::init()) {
		return false;
	}
    
	const CCSize &size = CCDirector::sharedDirector()->getWinSize();
    setContentSize(size);
    
	currentAcceleration = NULL;
    
    accelerationX = new MovingAverageFunction(GAME_ACCEL_COUNT);
    accelerationY = new MovingAverageFunction(GAME_ACCEL_COUNT);
    
	hasCapturedTouch = false;
    
	display = DisplayLayer::create();
    display->setDelegate(this);
    addChild(display, 2);
	
	background = CCSprite::create("images/background.jpg");
	background->setPosition(ccp(size.width/2, size.height/2));
	addChild(background, 0);
    
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/figures.plist", "images/figures.png");
	
	root = GameRoot::create("images/figures.png", size);
    root->setDelegate(this);
	root->setAnchorPoint(ccp(0, 0));
	root->setPosition(ccp(0, 0));
	addChild(root, 1);
#if CC_TARGET_PLATFORM == CC_PLATFORM_LINUX
	setTouchMode(kCCTouchesOneByOne);
	setTouchEnabled(true);
#else
    setAccelerometerEnabled(true);
#endif
    
	CCDirector::sharedDirector()->getScheduler()->scheduleUpdateForTarget(this, 1, false);
	
    display->setDurability(1);
    display->setShields(1);
    
	srand(time(NULL));
    
    CCMenuItemImage *menuImage = CCMenuItemImage::create("ui/blueButton.png", "ui/grayButton.png");
    menuImage->setTarget(this, menu_selector(GameLayer::onButtonExitTap));
    CCLabelBMFont *font = CCLabelBMFont::create("Exit", "fonts/NeuropolFont.fnt");
    font->setPosition(ccp(106, 44));
    menuImage->addChild(font);
    CCMenu *menu = CCMenu::create(menuImage, NULL);
    
    popup = GamePopup::create();
    popup->setPosition(ccp(size.width/2, size.height/2));
    popup->setVisible(false);
    popup->close(0);
    popup->setButton(menu);
    addChild(popup, 10);
	
	return true;
}

void GameLayer::update(float fDelta) {
	root->update(fDelta);
	
	float velocity = root->getVelocity(), scale = root->getVelocityScale();
	
    display->setVelocity(velocity);
    display->setVelocityScale(scale);
	
	display->setDurability(root->getDurability());
	display->setShields(root->getShields());
    display->setTimer(root->getTimer());
    
    display->setDistance(root->getDistanceScale());
    display->setScore(root->getScore());
    
    int status = root->getGameStatus();
    
    if (status != 0) {
        endGameWithStatus(status == 1);
    }
}

bool GameLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent) {
	if (!hasCapturedTouch) {
		hasCapturedTouch = true;
		return true;
	}
	return false;
}

void GameLayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent) {
	const CCPoint &curr = convertToNodeSpace(pTouch->getLocation()),
				  &prev = convertToNodeSpace(pTouch->getPreviousLocation());
	root->figureMoveBy(ccpMult(ccpSub(curr, prev), 10));
}

void GameLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent) {
	hasCapturedTouch = false;
}

void GameLayer::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent) {
	hasCapturedTouch = false;
}

void GameLayer::didAccelerate(CCAcceleration* pAccelerationValue) {
    if (currentAcceleration == NULL) {
        currentAcceleration = new CCAcceleration();
        currentAcceleration->x = pAccelerationValue->x;
        currentAcceleration->y = pAccelerationValue->y;
        currentAcceleration->z = pAccelerationValue->z;
    }
    
    float angle = atan2f(pAccelerationValue->y, pAccelerationValue->z) - atan2f(currentAcceleration->y, currentAcceleration->z);
    if (angle > M_PI) { angle -= M_PI * 2; }
    if (angle < -M_PI) { angle += M_PI * 2; }
    
    float shiftX = - (currentAcceleration->x - pAccelerationValue->x);
    float shiftY = - angle / (M_PI_4);
    
    shiftX = accelerationX->increment(shiftX);
    shiftY = accelerationY->increment(shiftY);
    
    display->setAccelerationX(shiftX);
    display->setAccelerationY(shiftY);
    
	root->setAcceleration(shiftX);
    
    CCPoint p = ccp(100, shiftY * m_obContentSize.height * GAME_ACCEL_MOD + m_obContentSize.height/2);
    
    root->figureMoveTo(p);
}

void GameLayer::displayPause() {
    CCScheduler *s = CCDirector::sharedDirector()->getScheduler();
    if (!s->isTargetPaused(this) && root->getGameStatus() == 0) {
        s->pauseTarget(this);
        CCLabelBMFont *label = popup->getLabel();
        label->setColor(ccc3(255, 255, 255));
        label->setString("Paused");
        popup->setShowButton(true);
        popup->setVisible(true);
        popup->open(0.5);
    } else {
        delete currentAcceleration;
        currentAcceleration = NULL;
		root->figureDropPosition();
        popup->closeWithCallback(0.5, this, callfunc_selector(GameLayer::popupCallback));
    }
}

void GameLayer::popupCallback() {
    if (!popup->isOpened()) {
        if (root->getGameStatus() != 0) {
            CCUserDefault *ud = CCUserDefault::sharedUserDefault();
            
            if (root->getGameStatus() == 1) {
                ud->setFloatForKey(UD_KEY_CURRENT_MAX_SPEED, root->getGameMaxSpeed());
                ud->setFloatForKey(UD_KEY_CURRENT_AVG_SPEED, root->getGameAvgSpeed());
                ud->setFloatForKey(UD_KEY_CURRENT_SCORE, root->getScore());
                ud->setFloatForKey(UD_KEY_CURRENT_TIME, root->getTimer());
                
                ud->setBoolForKey(UD_KEY_ACHIVED_SUCCESS, true);
                
                int flags = root->getFlags();
                
                if (flags & GameRoot::FlagFullHP)
                    ud->setBoolForKey(UD_KEY_ACHIVED_FULL_HP, true);
                
                if (flags & GameRoot::FlagNoBreaks)
                    ud->setBoolForKey(UD_KEY_ACHIVED_NO_BREAKS, true);
                
                if (flags & GameRoot::FlagNoCollision)
                    ud->setBoolForKey(UD_KEY_ACHIVED_NO_COLLISION, true);
                
            } else {
                
            }
            
            
            CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1, GameMenu::scene()));
        } else {
            CCDirector::sharedDirector()->getScheduler()->resumeTarget(this);
        }
        popup->setVisible(false);
    }
}

void GameLayer::endGameWithStatus(bool win) {
    CCDirector::sharedDirector()->getScheduler()->unscheduleAllForTarget(this);
    
    CCLabelBMFont *label = popup->getLabel();
    
    if (win) {
        label->setColor(ccc3(128, 128, 255));
        label->setString("Complete!");
    } else {
        label->setColor(ccc3(255, 128, 128));
        label->setString("Fail!");
    }
    popup->setShowButton(false);
    popup->setVisible(true);
    popup->open(0.5);
}

void GameLayer::onButtonExitTap(CCObject *obj) {
    CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1, GameMenu::scene()));
}

void GameLayer::onDespawnLabel(CCNode *obj) {
    removeChild(obj);
}

void GameLayer::onCollision(const CCPoint &point, enum FigureType figure, int value, int fid) {
    CCLOG("collision: %f %f  %d %d", point.x, point.y, figure, fid);
    if (figure == FigureScore) {
        char buf[5] = {0};
        sprintf(buf, "%+d", value);
        CCLabelBMFont *label = CCLabelBMFont::create(buf, "fonts/NeuropolNumbers.fnt");
        label->setPosition(point);
        CCAction *a = CCSequence::createWithTwoActions(CCMoveBy::create(1, ccp(-100, 100)), CCCallFuncN::create(this, callfuncN_selector(GameLayer::onDespawnLabel)));
        switch (fid) {
            case 1: label->setColor(ccc3(255, 128, 128));
                break;
            case 2: label->setColor(ccc3(255, 146, 128));
                break;
            case 3: label->setColor(ccc3(255, 196, 128));
                break;
            case 4: label->setColor(ccc3(128, 255, 128));
                break;
            case 5: label->setColor(ccc3(255, 255, 255));
                break;
                
            default:
                break;
        }
        int alpha = 128;
        alpha += value * 4;
        if (alpha > 255) alpha = 255;
        label->setOpacity(alpha);
        label->runAction(a);
        label->setScale(0.5);
        addChild(label, 1);
    }
    
    CCParticleSystemQuad *m_emitter;
    
    m_emitter = CCParticleSystemQuad::createWithTotalParticles(100);
    //m_emitter->initWithTotalParticles(100);

    m_emitter->setTexture( CCTextureCache::sharedTextureCache()->addImage("images/particleStars.png") );
    
    m_emitter->setDuration(0.5f);
    m_emitter->setGravity(ccp(-250,0));
    
    // angle
    m_emitter->setAngle(0);
    m_emitter->setAngleVar(360);
    
    // speed of particles
    m_emitter->setSpeed(60);
    m_emitter->setSpeedVar(20);
    
    // radial
    m_emitter->setRadialAccel(-120);
    m_emitter->setRadialAccelVar(0);
    
    // tagential
    m_emitter->setTangentialAccel(30);
    m_emitter->setTangentialAccelVar(0);
    
    // emitter position
    m_emitter->setPosition( ccp(160,240) );
    m_emitter->setPosVar(ccp(40,20));
    
    // life of particles
    m_emitter->setLife(1);
    m_emitter->setLifeVar(0.25f);
    
    // spin of particles
    m_emitter->setStartSpin(0);
    m_emitter->setStartSizeVar(0);
    m_emitter->setEndSpin(0);
    m_emitter->setEndSpinVar(0);
    
    // color of particles
    ccColor4F startColor;
    switch (figure) {
        case FigureAccelerator: startColor = ccc4f(0.988f, 0.698f, 0.0f, 1.0f);
            break;
        case FigureDeccelerator: startColor = ccc4f(0.850f, 0.0f, 0.0f, 1.0f);
            break;
        case FigureRepair: startColor = ccc4f(0.568f, 0.858f, 0.0f, 1.0f);
            break;
        case FigureScore:
            switch (fid) {
                case 1: startColor = ccc4f(0.929f, 0.392f, 0.517f, 1.0f);
                    break;
                case 2: startColor = ccc4f(0.913f, 0.549f, 0.352f, 1.0f);
                    break;
                case 3: startColor = ccc4f(0.937f, 0.760f, 0.400f, 1.0f);
                    break;
                case 4: startColor = ccc4f(0.607f, 0.905f, 0.458f, 1.0f);
                    break;
                case 5: startColor = ccc4f(0.894f, 0.894, 0.964f, 1.0f);
                    break;
                default: startColor = ccc4f(0.686f, 0.686f, 0.800f, 1.0f);
                    break;
            }
            break;
        case FigureShield: startColor = ccc4f(0.431f, 0.0f, 0.862f, 1.0f);
            break;
        default: startColor = ccc4f(0.0f, 0.692f, 0.988f, 1.0f);
            break;
    }
    m_emitter->setStartColor(startColor);
    
    ccColor4F startColorVar = {0.0f, 0.0f, 0.0f, 1.0f};
    m_emitter->setStartColorVar(startColorVar);
    
    ccColor4F endColor = {0.0f, 0.0f, 0.0f, 1.0f};
    m_emitter->setEndColor(endColor);
    
    ccColor4F endColorVar = {0.0f, 0.0f, 0.0f, 0.0f};
    m_emitter->setEndColorVar(endColorVar);
    
    // size, in pixels
    m_emitter->setStartSize(40.0f);
    m_emitter->setStartSizeVar(10.0f);
    m_emitter->setEndSize(0);
    
    // emits per second
    m_emitter->setEmissionRate(200);
    
    // additive
    m_emitter->setBlendAdditive(true);
    
    m_emitter->setPosition(point.x-60,point.y);
    this->addChild(m_emitter, 0);
    CCFiniteTimeAction *move =  CCMoveTo::create(0.5f,ccp(-120,point.y));
    CCDelayTime *delayAction = CCDelayTime::create(0.1f);
    m_emitter->runAction(CCSequence::createWithTwoActions(delayAction, move));
}


