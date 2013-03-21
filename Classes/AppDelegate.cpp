#include "AppDelegate.h"

#include "cocos2d.h"
#include "CCEGLView.h"
#include "GameLayer.h"
#include "GameMenu.h"

USING_NS_CC;

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
//    SimpleAudioEngine::end();
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    CCDirector *pDirector = CCDirector::sharedDirector();
    CCEGLView *view = CCEGLView::sharedOpenGLView();
    pDirector->setOpenGLView(view);
    view->setDesignResolutionSize(1024, 600, kResolutionShowAll);
	
    // turn on display FPS
    pDirector->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0 / 60);
	
	pDirector->runWithScene(GameMenu::scene());
//#endif

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    CCDirector::sharedDirector()->stopAnimation();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    CCDirector::sharedDirector()->startAnimation();
}
