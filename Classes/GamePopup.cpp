//
//  GamePopup.cpp
//  PaperGame
//
//  Created by SBKarr on 1/14/13.
//
//

#include "GamePopup.h"

USING_NS_CC;

bool GamePopup::init() {
    CCSprite *s;
    
    const CCSize &size = CCSize(850, 405);
    setContentSize(size);
    setAnchorPoint(ccp(0.5, 0.5));
    
    s = CCSprite::create("ui/tableSide.png");
    s->setAnchorPoint(ccp(1, 0));
    s->setPosition(ccp(0, 0));
    s->setScaleX(-1);
    addChild(s, 1, TagLeft);
    
    s = CCSprite::create("ui/tableSide.png");
    s->setAnchorPoint(ccp(1, 0));
    s->setPosition(ccp(size.width, 0));
    addChild(s, 1, TagRight);
    
    s = CCSprite::create("ui/tableBody.png");
    s->setColor(ccc3(255, 255, 255));
    s->setScaleX(size.width - 50);
    s->setScaleY(size.height - 36);
    s->setPosition(ccp(size.width/2, size.height/2));
    addChild(s, 0, TagBody);
    
    label = CCLabelBMFont::create("Test Label", "fonts/NeuropolFont.fnt");
    label->setPosition(ccp(size.width/2, size.height/2));
    addChild(label, 2, TagLabel);
    
    popupOpened = true;
    hasActiveAnimation = false;
    showButton = true;
    button = NULL;
    
    return true;
};

void GamePopup::onActionFinished() {
    hasActiveAnimation = false;
}

void GamePopup::open(float duration) {
    openWithCallback(duration, NULL, NULL);
}

void GamePopup::openWithCallback(float duration, CCObject *obj, SEL_CallFunc selector) {
    openWithAction(duration, (obj != NULL && selector != NULL) ? CCCallFunc::create(obj, selector) : NULL);
}

void GamePopup::openWithAction(float duration, CCFiniteTimeAction *action) {
    if (hasActiveAnimation) return;
    
    CCNode *n;
    CCFiniteTimeAction *a;
    
    n = getChildByTag(TagLeft);
    n->stopAllActions();
    n->setPosition(ccp(getContentSize().width/2 - 25, 0));
    a = CCMoveTo::create(duration, ccp(0, 0));
    n->runAction(a);
    
    n = getChildByTag(TagRight);
    n->stopAllActions();
    n->setPosition(ccp(getContentSize().width/2 + 25, 0));
    a = CCMoveTo::create(duration, ccp(getContentSize().width, 0));
    n->runAction(a);
    
    n = getChildByTag(TagBody);
    n->stopAllActions();
    n->setScaleX(1);
    a = CCScaleTo::create(duration, getContentSize().width - 50, getContentSize().height - 36);
    if (action != NULL) {
        a = CCSequence::createWithTwoActions(a, action);
    }
    n->runAction(a);
    
    a = CCSequence::createWithTwoActions(CCDelayTime::create(duration + 0.1),CCCallFunc::create(this, callfunc_selector(GamePopup::onActionFinished)));
    this->runAction(a);
    hasActiveAnimation = true;
    popupOpened = true;
    
    label->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(duration/2), CCFadeIn::create(duration/2)));
    if (button && showButton) {
        button->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(duration/2), CCFadeIn::create(duration/2)));
    }
}

void GamePopup::close(float duration) {
    closeWithCallback(duration, NULL, NULL);
}

void GamePopup::closeWithCallback(float duration, CCObject *obj, SEL_CallFunc selector) {
    closeWithAction(duration, (obj != NULL && selector != NULL) ? CCCallFunc::create(obj, selector) : NULL);
}

void GamePopup::closeWithAction(float duration, CCFiniteTimeAction *action) {
    if (hasActiveAnimation) return;
    
    CCNode *n;
    CCFiniteTimeAction *a;
    
    n = getChildByTag(TagLeft);
    n->stopAllActions();
    n->setPosition(ccp(0, 0));
    a = CCMoveTo::create(duration, ccp(getContentSize().width/2 - 25, 0));
    n->runAction(a);
    
    n = getChildByTag(TagRight);
    n->stopAllActions();
    n->setPosition(ccp(getContentSize().width, 0));
    a = CCMoveTo::create(duration, ccp(getContentSize().width/2 + 25, 0));
    n->runAction(a);
    
    n = getChildByTag(TagBody);
    n->stopAllActions();
    n->setScaleX(getContentSize().width - 50);
    a = CCScaleTo::create(duration, 1, getContentSize().height - 36);
    if (action != NULL) {
        a = CCSequence::createWithTwoActions(a, action);
    }
    n->runAction(a);
    
    a = CCSequence::createWithTwoActions(CCDelayTime::create(duration + 0.1),CCCallFunc::create(this, callfunc_selector(GamePopup::onActionFinished)));
    this->runAction(a);
    hasActiveAnimation = true;
    popupOpened = false;
    
    label->runAction(CCFadeOut::create(duration/2));
    if (button) {
        button->runAction(CCFadeOut::create(duration/2));
    }
}

void GamePopup::switchPopup(float duration) {
    switchPopupWithCallback(duration, NULL, NULL);
}

void GamePopup::switchPopupWithCallback(float duration, CCObject *obj, SEL_CallFunc selector) {
    switchPopupWithAction(duration, (obj != NULL && selector != NULL) ? CCCallFunc::create(obj, selector) : NULL);
}

void GamePopup::switchPopupWithAction(float duration, CCFiniteTimeAction *a) {
    if (isOpened()) {
        closeWithAction(duration, a);
    } else {
        openWithAction(duration, a);
    }
}

void GamePopup::setString(const char *text) {
    label->setString(text);
}

const char* GamePopup::getString(void) {
    return label->getString();
}

void GamePopup::setColor(const ccColor3B& color) {
    label->setColor(color);
}
ccColor3B GamePopup::getColor(void) {
    return label->getColor();
}
GLubyte GamePopup::getOpacity(void) {
    return label->getOpacity();
}
void GamePopup::setOpacity(GLubyte opacity) {
    label->setOpacity(opacity);
}
void GamePopup::setOpacityModifyRGB(bool bValue) {
    label->setOpacityModifyRGB(bValue);
}
bool GamePopup::isOpacityModifyRGB(void) {
    return label->isOpacityModifyRGB();
}
void GamePopup::setButton(CCNode *theButton) {
    if (button) {
        removeChild(button);
    }
    button = theButton;
    button->setPosition(ccp(m_obContentSize.width/2, m_obContentSize.height/2 - 100));
    addChild(button);
    
    CCRGBAProtocol *p = dynamic_cast<CCRGBAProtocol *>(button);
    if (p) p->setOpacity(0);
}
void GamePopup::setShowButton(bool show) {
    showButton = show;
    button->setVisible(show);
}
