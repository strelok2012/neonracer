/* 
 * File:   GameRoot.h
 * Author: sbkarr
 *
 * Created on 11 Январь 2013 г., 11:29
 */

#ifndef GAMEROOT_H
#define	GAMEROOT_H

#include "cocos2d.h"

USING_NS_CC;

class FigureValues;

typedef enum FigureType {
    FigurePlayer,
    FigureBorder,
    FigureAccelerator,
    FigureDeccelerator,
    FigureRepair,
    FigureScore,
    FigureShield,
} FigureType;

class GameRootDelegate {
public:
    virtual void onCollision(const CCPoint &point, enum FigureType figure, int value, int fid) = 0;
};

class GameRoot : public CCSpriteBatchNode {
protected:
    float gameSpeedRate;
    float gameSpeedMax;
    float gameSpeedAcceleration;
	
	float gamePathLength, gameTimer;
    
    int gameScore, gameScoreMod;
    
    float gameMaxSpeed;
	
    CCPoint figureStartPoint;
    CCPoint figureTargetPoint;
	
	CCSprite *figure;
	
    CCArray *cleanupArray;
    
    GameRootDelegate *delegate;
	
    virtual void processCollision(CCNode *player, CCNode *node);
    virtual void spawnNode(float pos, float offset, enum FigureType type, float d, float p);
    virtual void spawnRandomNode(float top, float bottom);
    
    virtual void raceSpawnProc(float fDeltha);
    virtual void collisionCheck();
    virtual void collisionCleanup();
    virtual void updatePlayerPosition(float fDeltha);
    virtual void updateWorldPosition(float fDeltha);
	
public:
	~GameRoot();
	static GameRoot * create(const char *filename, const CCSize &size);
	bool init(const char *filename, const CCSize &size);

	virtual void update(float fDeltha);
	
	virtual void figureMoveBy(const CCPoint &point);
	virtual void figureMoveTo(const CCPoint &point);
	virtual void figureDropPosition();
	
	virtual void setAcceleration(float a);
	
	virtual float getVelocity();
	virtual float getVelocityScale();
	
	virtual float getDurability();
	virtual float getShields();
    virtual float getTimer();
    
    virtual float getDistanceScale();
    
    virtual int getScore();
    
    virtual float getGameMaxSpeed();
    virtual float getGameAvgSpeed();
    
    virtual int getFlags();
    
    virtual void setDelegate(GameRootDelegate *theDelegate) { delegate = theDelegate; }
    
public:
    enum Status {
        StatusGame = 0,
        StatusWin = 1,
        StatusLose = 2,
    } gameStatus;
    
    int getGameStatus();
    
    enum Flags {
        FlagEmpty,
        FlagNoBreaks = 1,
        FlagFullHP = 2,
        FlagNoCollision = 4,
    };
    
    int flags;
};


#endif	/* GAMEROOT_H */

