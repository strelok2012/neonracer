//
//  DisplayLayer.cpp
//  PaperGame
//
//  Created by SBKarr on 12/17/12.
//
//

#include "DisplayLayer.h"

#define DISPLAY_SHIELD_WIDTH 300
#define DISPLAY_DURABILITY_WIDTH 300
#define DISPLAY_VELOCITY_WIDTH 300

using namespace cocos2d;

CCClippingNode *makeClippingNode(const char *spriteName, const char *stencilName) {
    CCClippingNode *clipNode;
    CCSprite *sprite, *stencil;
    
    sprite = CCSprite::create(spriteName);
    sprite->setAnchorPoint(ccp(0, 0));
    sprite->setPosition(ccp(0, 0));
    
    stencil = CCSprite::create(stencilName);
    stencil->setScaleX(sprite->getContentSize().width);
    stencil->setScaleY(sprite->getContentSize().height);
    stencil->setAnchorPoint(ccp(0, 0));
    stencil->setPosition(ccp(0, 0));
    
    clipNode = CCClippingNode::create();
    clipNode->addChild(sprite);
    clipNode->setContentSize(sprite->getContentSize());
    clipNode->setStencil(stencil);
    
    return clipNode;
}

bool DisplayLayer::init() {
    if (!CCLayer::init()) {
        return false;
    }
    
    selected = 0;
    delegate = NULL;
    
    const CCSize & size = CCDirector::sharedDirector()->getWinSize();
    
    setContentSize(size);
    /*
    accelerationX = CCSprite::create("images/line.png");
    accelerationX->setScaleX(24);
    accelerationX->setPosition(ccp(size.width/2, accelerationX->boundingBox().size.height/2));
    addChild(accelerationX, 0, TagAccelerationX);
    
    accelerationY = CCSprite::create("images/line.png");
    accelerationY->setRotation(90);
    accelerationY->setScaleX(24);
    accelerationY->setPosition(ccp(accelerationY->boundingBox().size.width/2, size.height/2));
    addChild(accelerationY, 0, TagAccelerationY);
    */
    velocity = CCLabelBMFont::create("000", "fonts/NeuropolNumbers.fnt");
    velocity->setPosition(ccp(m_obContentSize.width - 80, m_obContentSize.height - 28));
    addChild(velocity, 2, TagVelocity);
    
	setTouchMode(kCCTouchesOneByOne);
	setTouchEnabled(true);
        
    CCClippingNode *clipNode;
    
    clipNode = makeClippingNode("ui/health.png", "ui/dummy.png");
    clipNode->setAnchorPoint(ccp(0, 1));
    clipNode->setPosition(ccp(0, m_obContentSize.height));
    addChild(clipNode, 1, TagDurability);
    
    clipNode = makeClippingNode("ui/shields.png", "ui/dummy.png");
    clipNode->setAnchorPoint(ccp(0, 1));
    clipNode->setPosition(ccp(0, m_obContentSize.height - 30));
    addChild(clipNode, 1, TagShields);
    
    clipNode = makeClippingNode("ui/speed.png", "ui/dummy.png");
    clipNode->setAnchorPoint(ccp(1, 1));
    clipNode->setPosition(ccp(m_obContentSize.width, m_obContentSize.height));
    addChild(clipNode, 1, TagVelocityScale);
    
    CCLayerColor *cl = CCLayerColor::create(ccc4(0, 0, 128, 64), size.width, 40);
    cl->setPosition(ccp(0, 0));
    addChild(cl, 2, TagBottomBG);
    
    timer = CCLabelBMFont::create("00:00.000", "fonts/NeuropolNumbers.fnt");
    timer->setPosition(ccp(m_obContentSize.width - 300, 20));
    timer->setAnchorPoint(ccp(0, 0.5));
    timer->setColor(ccc3(128, 128, 255));
    addChild(timer, 3, TagTimer);
    
    score = CCLabelBMFont::create("0000", "fonts/NeuropolNumbers.fnt");
    score->setPosition(ccp(m_obContentSize.width/2, 20));
    score->setColor(ccc3(255, 255, 64));
    addChild(score, 3, TagScore);
    
    clipNode = makeClippingNode("ui/speed.png", "ui/dummy.png");
    clipNode->setAnchorPoint(ccp(0, 0));
    clipNode->setPosition(ccp(0, 0));
    addChild(clipNode, 3, TagDistance);
    
    return true;
}

void DisplayLayer::update(float fDeltha) {
}

void DisplayLayer::setAccelerationX(float value) {
    //accelerationX->setPositionX(value * m_obContentSize.width + m_obContentSize.width/2);
}

void DisplayLayer::setAccelerationY(float value) {
    //accelerationY->setPositionY(value * m_obContentSize.height + m_obContentSize.height/2);
}

void DisplayLayer::setVelocity(float value) {
    CCString *s = new CCString();
    s->initWithFormat("%d", (int)roundf(value));
    velocity->setString(s->getCString());
    delete s;
}

void DisplayLayer::setShields(float value) {
    CCClippingNode *n = dynamic_cast<CCClippingNode *>(getChildByTag(TagShields));
    if (n) n->getStencil()->setScaleX(DISPLAY_SHIELD_WIDTH * value);
}

void DisplayLayer::setDurability(float value) {
    CCClippingNode *n = dynamic_cast<CCClippingNode *>(getChildByTag(TagDurability));
    if (n) n->getStencil()->setScaleX(DISPLAY_DURABILITY_WIDTH * value);
}

void DisplayLayer::setVelocityScale(float value) {
    CCClippingNode *n = dynamic_cast<CCClippingNode *>(getChildByTag(TagVelocityScale));
    if (n) n->getStencil()->setScaleX(DISPLAY_DURABILITY_WIDTH * value);
}

void DisplayLayer::setDistance(float value) {
    CCClippingNode *n = dynamic_cast<CCClippingNode *>(getChildByTag(TagDistance));
    if (n) n->getStencil()->setScaleX(DISPLAY_DURABILITY_WIDTH * value);
}

void DisplayLayer::setTimer(float fTimer) {
    int minutes, seconds, mkseconds;
    
    seconds = floorf(fTimer);
    mkseconds = floorf((fTimer - seconds) * 1000);
    minutes = floorf(seconds/60);
    seconds -= minutes * 60;
    
    CCString *s = new CCString();
    s->initWithFormat("%02d:%02d.%03d", minutes, seconds, mkseconds);
    
    timer->setString(s->getCString());
    delete s;
}

void DisplayLayer::setScore(int newScore) {
    CCString *s = new CCString();
    s->initWithFormat("%04d", newScore);
    score->setString(s->getCString());
    delete s;
}

bool DisplayLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent) {
    if (selected != 0) {
        return false;
    }
    
    if (delegate != NULL) delegate->displayPause();
    return false;
}

void DisplayLayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent) {
}

void DisplayLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent) {
}

void DisplayLayer::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent) {
}    

