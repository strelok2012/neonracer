//
//  GameMenu.cpp
//  PaperGame
//
//  Created by SBKarr on 1/11/13.
//
//

#include "GameMenu.h"
#include "GameLayer.h"

USING_NS_CC;

CCScene * GameMenu::scene() {
	CCScene *scene = CCScene::create();
	scene->addChild(GameMenu::create());
	return scene;
}

CCLabelBMFont *newLabel(const char *text, const CCPoint &p) {
    CCLabelBMFont *fnt = CCLabelBMFont::create(text, "fonts/NeuropolNumbers.fnt");
    fnt->setPosition(p);
    return fnt;
}

CCSprite *newAchivementSprite(const char *frame, const CCPoint &pos, int count, bool isNew) {
    char b[24] = {0};
    
    const char *score;
    if (count > 3) {
        score = "Gold";
    } else if (count > 1) {
        score = "Silver";
    } else {
        score = "Bronze";
    }
    
    sprintf(b, "square%s%s.png", score, frame);
    
    CCSprite *ach = CCSprite::createWithSpriteFrameName(b);
    ach->setPosition(pos);
    ach->setScale(0.75);
    if (count == 0) {
        ach->setOpacity(128);
    }
    
    memset(b, 0, sizeof(char) * 24);
    sprintf(b, "x%d", count);
    
    CCLabelBMFont *font = CCLabelBMFont::create(b, "fonts/NeuropolFont.fnt");
    font->setAnchorPoint(ccp(0, 0.5));
    font->setPosition(ccp(ach->getContentSize().width + 10, ach->getContentSize().height/2));
    ach->addChild(font);
    
    if (isNew) {
        CCSprite *n = CCSprite::create("images/new.png");
        n->setPosition(ccp(1, ach->getContentSize().height-1));
        n->setAnchorPoint(ccp(0, 1));
        ach->addChild(n);
    }
    
    return ach;
}

void GameMenu::initStatList(const CCPoint &p, int tag) {
    int i;
    CCLabelBMFont *fnt;
    
    for (i = 0; i < 4; i++) {
        const char *str;
        if (i == 3) { str = "-00:00.000"; } else { str = "00000"; }
        fnt = newLabel(str, ccpAdd(p, ccp(0, -56 * i)));
        if (i == 3) { fnt->setScale(0.3); } else { fnt->setScale(0.4); }
        addChild(fnt, 2, tag + i);
    }
}

bool GameMenu::init() {
    if (!CCLayer::init()) {
        return false;
    }
    
	const CCSize &size = CCDirector::sharedDirector()->getWinSize();
    setContentSize(size);
    
	CCSprite *background = CCSprite::create("images/background.jpg");
	background->setPosition(ccp(size.width/2, size.height/2));
	addChild(background, 0);
    
    CCSprite *table = CCSprite::create("ui/tableLabels.png");
    table->setScale(1);
    table->setPosition(ccp(size.width/2, size.height/2 + 120));
    addChild(table, 1);
    
    hasCapturedTouch = false;
    
	setTouchMode(kCCTouchesOneByOne);
	setTouchEnabled(true);
    
    /* Menu Buttons */
    
    CCLabelBMFont *font;
    
    CCMenuItemImage *btnExit = CCMenuItemImage::create("ui/grayButton.png", "ui/grayButton.png");
    btnExit->setTarget(this, menu_selector(GameMenu::onButtonExitTap));
    font = CCLabelBMFont::create("Exit", "fonts/NeuropolFont.fnt");
    font->setPosition(ccp(106, 44));
    btnExit->addChild(font);
    
    CCMenuItemImage *btnPlay = CCMenuItemImage::create("ui/blueButton.png", "ui/grayButton.png");
    btnPlay->setTarget(this, menu_selector(GameMenu::onButtonPlayTap));
    font = CCLabelBMFont::create("Play", "fonts/NeuropolFont.fnt");
    font->setPosition(ccp(106, 44));
    btnPlay->addChild(font);
    
    CCMenu *menu = CCMenu::create(btnExit, btnPlay, NULL);
    menu->alignItemsHorizontallyWithPadding(100);
    menu->setPosition(ccp(512, 60));
    addChild(menu);
    
    /* Achivement score */
    
    CCUserDefault *ud = CCUserDefault::sharedUserDefault();
    
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/achives.plist", "images/achives.png");
    
    int achSuccess, achFullHP, achNoCollision, achNoBreaks;
    achSuccess = ud->getIntegerForKey(UD_KEY_COUNT_SUCCESS, 0);
    achFullHP = ud->getIntegerForKey(UD_KEY_COUNT_FULL_HP, 0);
    achNoCollision = ud->getIntegerForKey(UD_KEY_COUNT_NO_COLLISION, 0);
    achNoBreaks = ud->getIntegerForKey(UD_KEY_COUNT_NO_BREAKS, 0);
    
    bool newSuccess, newFullHP, newNoCollision, newNoBreaks;
    newSuccess = ud->getBoolForKey(UD_KEY_ACHIVED_SUCCESS, false);
    newFullHP = ud->getBoolForKey(UD_KEY_ACHIVED_FULL_HP, false);
    newNoCollision = ud->getBoolForKey(UD_KEY_ACHIVED_NO_COLLISION, false);
    newNoBreaks = ud->getBoolForKey(UD_KEY_ACHIVED_NO_BREAKS, false);
    
    if (newSuccess) achSuccess ++;
    if (newFullHP) achFullHP ++;
    if (newNoCollision) achNoCollision ++;
    if (newNoBreaks) achNoBreaks ++;
    
    addChild(newAchivementSprite("Player", ccp(160, 180), achSuccess, newSuccess));
    addChild(newAchivementSprite("Heal", ccp(390, 180), achFullHP, newFullHP));
    addChild(newAchivementSprite("Shield", ccp(size.width - 390, 180), achNoCollision, newNoCollision));
    addChild(newAchivementSprite("Fast", ccp(size.width - 160, 180), achNoBreaks, newNoBreaks));
    
    ud->setIntegerForKey(UD_KEY_COUNT_SUCCESS, achSuccess);
    ud->setIntegerForKey(UD_KEY_COUNT_FULL_HP, achFullHP);
    ud->setIntegerForKey(UD_KEY_COUNT_NO_COLLISION, achNoCollision);
    ud->setIntegerForKey(UD_KEY_COUNT_NO_BREAKS, achNoBreaks);
    
    ud->setBoolForKey(UD_KEY_ACHIVED_SUCCESS, false);
    ud->setBoolForKey(UD_KEY_ACHIVED_FULL_HP, false);
    ud->setBoolForKey(UD_KEY_ACHIVED_NO_COLLISION, false);
    ud->setBoolForKey(UD_KEY_ACHIVED_NO_BREAKS, false);
    
    initStatList(ccp(470, 476), TagCurrentMaxSpeed);
    initStatList(ccp(615, 476), TagDiffMaxSpeed);
    initStatList(ccp(770, 476), TagBestMaxSpeed);
    
    
    float curMaxSpeed, curAvgSpeed, curScore, curTime;
    float bestMaxSpeed, bestAvgSpeed, bestScore, bestTime;
    float diffMaxSpeed, diffAvgSpeed, diffScore, diffTime;
    
    curMaxSpeed = ud->getFloatForKey(UD_KEY_CURRENT_MAX_SPEED, 0);
    curAvgSpeed = ud->getFloatForKey(UD_KEY_CURRENT_AVG_SPEED, 0);
    curScore = ud->getFloatForKey(UD_KEY_CURRENT_SCORE, 0);
    curTime = ud->getFloatForKey(UD_KEY_CURRENT_TIME, 0);
    
    bestMaxSpeed = ud->getFloatForKey(UD_KEY_BEST_MAX_SPEED, 0);
    bestAvgSpeed = ud->getFloatForKey(UD_KEY_BEST_AVG_SPEED, 0);
    bestScore = ud->getFloatForKey(UD_KEY_BEST_SCORE, 0);
    bestTime = ud->getFloatForKey(UD_KEY_BEST_TIME, 0);
    
    if (bestMaxSpeed < curMaxSpeed) { ud->setFloatForKey(UD_KEY_BEST_MAX_SPEED, curMaxSpeed); }
    if (bestAvgSpeed < curAvgSpeed) { ud->setFloatForKey(UD_KEY_BEST_AVG_SPEED, curAvgSpeed); }
    if (bestScore < curScore) { ud->setFloatForKey(UD_KEY_BEST_SCORE, curScore); }
    if ((bestTime > curTime && curTime != 0) || bestTime == 0) { ud->setFloatForKey(UD_KEY_BEST_TIME, curTime); }
    
    diffMaxSpeed = curMaxSpeed - bestMaxSpeed;
    diffAvgSpeed = curAvgSpeed - bestAvgSpeed;
    diffScore = curScore - bestScore;
    diffTime = curTime - bestTime;
    
    setLabelString(TagCurrentMaxSpeed, "%04d", (int)curMaxSpeed);
    setLabelString(TagCurrentAvgSpeed, "%04d", (int)curAvgSpeed);
    setLabelString(TagCurrentScore, "%05d", (int)curScore);
    setLabelTimer(TagCurrentTime, curTime);
    
    setLabelString(TagBestMaxSpeed, "%04d", (int)bestMaxSpeed);
    setLabelString(TagBestAvgSpeed, "%04d", (int)bestAvgSpeed);
    setLabelString(TagBestScore, "%05d", (int)bestScore);
    setLabelTimer(TagBestTime, bestTime);
    
    setLabelDiff(TagDiffMaxSpeed, diffMaxSpeed);
    setLabelDiff(TagDiffAvgSpeed, diffAvgSpeed);
    setLabelDiff(TagDiffScore, diffScore);
    setLabelDiffTimer(TagDiffTime, diffTime);
    
    ud->flush();
    
    return true;
}

void GameMenu::setLabelString(int tag, const char *format, ...) {
    CCLabelBMFont *fnt = dynamic_cast<CCLabelBMFont *>(getChildByTag(tag));
    
    if (fnt == NULL) return;

    va_list ap;
    va_start(ap, format);
    CCString *s = new CCString();
    s->initWithFormatAndValist(format, ap);
    fnt->setString(s->getCString());
    delete s;
    va_end(ap);
}

void GameMenu::setLabelTimer(int tag, float fTimer) {
    int minutes, seconds, mkseconds;
    
    seconds = floorf(fTimer);
    mkseconds = floorf((fTimer - seconds) * 1000);
    minutes = floorf(seconds/60);
    seconds -= minutes * 60;
    
    setLabelString(tag, "%02d:%02d.%03d", minutes, seconds, mkseconds);
}

void GameMenu::setLabelDiff(int tag, float fValue) {
    CCLabelBMFont *fnt = dynamic_cast<CCLabelBMFont *>(getChildByTag(tag));
    
    if (fnt == NULL) return;
    
    if (fValue == 0) {
        fnt->setColor(ccc3(255, 255, 255));
        fnt->setString("-");
        return;
    }
    
    if (fValue > 0) {
        fnt->setColor(ccc3(128, 255, 128));
    } else if (fValue < 0) {
        fnt->setColor(ccc3(255, 128, 128));
    }
    setLabelString(tag, "%+d", (int)fValue);
}

void GameMenu::setLabelDiffTimer(int tag, float fTimer) {
    CCLabelBMFont *fnt = dynamic_cast<CCLabelBMFont *>(getChildByTag(tag));
    
    if (fnt == NULL) return;
    
    if (fTimer == 0) {
        fnt->setColor(ccc3(255, 255, 255));
        fnt->setString("-");
        return;
    }
    
    int minutes, seconds, mkseconds;
    
    float t = fabsf(fTimer);
    
    seconds = floorf(t);
    mkseconds = floorf((t - seconds) * 1000);
    minutes = floorf(seconds/60);
    seconds -= minutes * 60;
    if (fTimer > 0) {
        fnt->setColor(ccc3(255, 128, 128));
        setLabelString(tag, "+%02d:%02d.%03d", minutes, seconds, mkseconds);
    } else {
        fnt->setColor(ccc3(128, 255, 128));
        setLabelString(tag, "-%02d:%02d.%03d", minutes, seconds, mkseconds);
    }
}

bool GameMenu::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent) {
	if (!hasCapturedTouch) {
		hasCapturedTouch = true;
		return true;
	}
	return false;
}

void GameMenu::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent) {
}

void GameMenu::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent) {
	hasCapturedTouch = false;
}

void GameMenu::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent) {
	hasCapturedTouch = false;
}

void GameMenu::onButtonExitTap(CCObject *obj) {
    exit(0);
}
void GameMenu::onButtonPlayTap(CCObject *obj) {
    CCDirector::sharedDirector()->replaceScene(CCTransitionFade ::create(1, GameLayer::scene()));
}