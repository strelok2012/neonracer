/* 
 * File:   GameRoot.cpp
 * Author: sbkarr
 * 
 * Created on 11 Январь 2013 г., 11:29
 */

#include "GameRoot.h"

#include <math.h>

#define GAME_SPAWN_RATE 60.0f
#define GAME_SPEED_RATE 300.0f
#define GAME_SPEED_START 300.0f
#define GAME_SPEED_FACTOR 250.0f
#define GAME_SPEED_BREAKS 10.0f
#define GAME_RACE_DIRECT_RATE 1023
#define GAME_RACE_DIRECT_SIZE 200
#define GAME_RACE_DIRECT_MIN 160
#define GAME_RACE_DIRECT_SHIFT 30

#define GAME_SPAWN_PERCENT 0.1f

#define GAME_BORDER_DURABILITY 20
#define GAME_COLLISION_SPEED 40

#define GAME_PARAM_DURABILITY 100
#define GAME_PARAM_SHIELD 50
#define GAME_PARAM_SHIELD_REGEN 2

#define GAME_RANDOM_START 500
#define GAME_RANDOM_MIN 100
#define GAME_RANDOM_MAX 500
#define GAME_RANDOM_ROW 3

//#define GAME_LENGTH_MAX 10000
#define GAME_LENGTH_MAX 100000

#define GAME_ACCEL_INTERVAL 0.25

class FigureValues : public CCObject {
public:
protected:
    float durability, parameter;
    int type, id;
public:
    FigureValues(enum FigureType t, float d, float p, int i) : durability(d), parameter(p), type(t), id(i) {};
    
    static FigureValues *create(enum FigureType t, float d, float p, int i) {
        FigureValues *v = new FigureValues(t, d, p, i);
        v->autorelease();
        return v;
    }
    
    static FigureValues *create(enum FigureType t, float d, float p) {
        return FigureValues::create(t, d, p, 0);
    }
    
    
    int getType() { return type; }
    void setType(enum FigureType t) { type = t; }
    
    float getDurability() { return durability; }
    void setDurability(float d) { durability = d; }
    
    float getParameter() { return parameter; }
    void setParameter(float p) { parameter = p; }
    
    int getID() { return id; }
    void setID(int i) { id = i; }
};

GameRoot * GameRoot::create(const char *filename, const CCSize &size) {
	GameRoot *root = new GameRoot();
	if (root->init(filename, size)) {
		root->autorelease();
		return root;
	} else {
		delete root;
		root = NULL;
		return NULL;
	}
}

bool GameRoot::init(const char* filename, const CCSize &size) {
	if (!CCSpriteBatchNode::initWithFile(filename, 100)) {
		return false;
	}

	setContentSize(size);

	gameSpeedRate = GAME_SPEED_START;
	gameSpeedAcceleration = GAME_SPEED_FACTOR;
	gameSpeedMax = GAME_SPEED_START * 2;

	gamePathLength = 0;
    gameTimer = 0;
    
    gameScore = 0;
    gameScoreMod = 0;
    
    gameMaxSpeed = 0;
    
    delegate = NULL;

	figure = CCSprite::createWithSpriteFrameName("Player.png");
	figure->setPosition(ccp(100, m_obContentSize.height / 2));
	figure->setUserObject(FigureValues::create(FigurePlayer, GAME_PARAM_DURABILITY, GAME_PARAM_SHIELD));
	figure->setScale(0.6);
	addChild(figure);

	figureStartPoint = figure->getPosition();
	figureTargetPoint = figure->getPosition();

	cleanupArray = new CCArray(1);
    
    flags = FlagFullHP | FlagNoBreaks | FlagNoCollision;

	return true;
}

void GameRoot::spawnNode(float pos, float offset, enum FigureType type, float d, float p) {
	const char *name;
    char buf[20] = {0};
    int id = 0;
	switch (type) {
		case FigureBorder:
            name = "Blocks1.png";
			break;
		case FigureRepair: name = "healBlock.png";
			break;
		case FigureShield: name = "shieldBlock.png";
			break;
		case FigureAccelerator: name = "speedBlock.png";
			break;
		case FigureDeccelerator: name = "slowBlock.png";
			break;
		case FigureScore:
            id = rand() % 5 + 1;
            sprintf(buf, "Gems%d.png", id);
            name = buf;
			break;
		default: name = NULL;
			break;
	}
	if (name != NULL) {
		CCSprite *s = CCSprite::createWithSpriteFrameName(name);
		s->setPosition(ccp(m_obContentSize.width - offset, pos));
		s->setUserObject(FigureValues::create(type, d, p, id));
		s->setScale(0.6);
		addChild(s);
	}
}

void GameRoot::spawnRandomNode(float top, float bottom) {
	static int counter = 0;
	int r = rand();
	int val = r % 100;
	enum FigureType type;
	if (counter < GAME_RANDOM_ROW) {
		type = FigureScore;
		counter++;
	} else if (val < 50) {
		type = FigureScore;
	} else {
		if (val < 60) {
			type = FigureBorder;
		} else if (val < 70) {
			type = FigureAccelerator;
		} else if (val < 80) {
			type = FigureDeccelerator;
		} else if (val < 90) {
			type = FigureRepair;
		} else if (val < 100) {
			type = FigureShield;
		}
		counter = 0;
	}


	float pos = bottom + (top - bottom) * ((float) r / (float) RAND_MAX);
	spawnNode(m_obContentSize.height / 2 + pos, 0, type, GAME_BORDER_DURABILITY, GAME_BORDER_DURABILITY);
}

void GameRoot::processCollision(cocos2d::CCNode *player, cocos2d::CCNode *node) {
	FigureValues *p = dynamic_cast<FigureValues *> (player->getUserObject());
	FigureValues *v = dynamic_cast<FigureValues *> (node->getUserObject());

	int type = v->getType();
	if (type == FigureBorder) {
		float damage = v->getParameter() * gameSpeedRate / GAME_SPEED_START;
		float shields = p->getParameter();
		float durability = p->getDurability();

		if (shields > 0) {
			shields -= damage;
			damage = 0;
			if (shields < 0) {
				damage -= shields;
				shields = 0;
			}
		}
		if (damage > 0) {
            if (flags & FlagFullHP) flags ^= FlagFullHP;
			durability -= damage;
			gameSpeedRate -= GAME_COLLISION_SPEED * gameSpeedRate / GAME_SPEED_START;
		}
		if (durability < 0) {
			durability = 0;
            
            // player crashed
            gameStatus = StatusLose;
		}
        
		p->setDurability(durability);
		p->setParameter(shields);
        
        if (flags & FlagNoCollision) flags ^= FlagNoCollision;
        gameScoreMod = 0;
	} else if (type == FigureScore) {
		gameSpeedMax += 1;
        gameScoreMod += 1;
        gameScore += gameScoreMod;
	} else if (type == FigureAccelerator) {
		gameSpeedMax += 50;
		gameSpeedRate += 100;
	} else if (type == FigureDeccelerator) {
		gameSpeedMax -= 150;
		if (gameSpeedMax < GAME_SPEED_START * 2) {
			gameSpeedMax = GAME_SPEED_START * 2;
		}
		gameSpeedRate -= 300;
        
        if (flags & FlagNoBreaks) flags ^= FlagNoBreaks;
        if (flags & FlagNoCollision) flags ^= FlagNoCollision;
        gameScoreMod = 0;
	} else if (type == FigureRepair) {
		float durability = p->getDurability();
		durability += 20;
		if (durability > GAME_PARAM_DURABILITY) {
			durability = GAME_PARAM_DURABILITY;
		}
		p->setDurability(durability);
	} else if (type == FigureShield) {
		p->setParameter(GAME_PARAM_SHIELD);
	}
    if (delegate) {
        delegate->onCollision(node->getPosition(), (FigureType)type, (type == FigureScore)?gameScoreMod:0, v->getID());
    }
	cleanupArray->addObject(node);
}

void GameRoot::raceSpawnProc(float fDeltha) {
	static float spawnTimer = 0, directPos = 0, directRate = 0;
	float pathDeltha = fDeltha * gameSpeedRate;

	spawnTimer += pathDeltha;
	if (spawnTimer > GAME_SPAWN_RATE) {
		float diff = spawnTimer - GAME_SPAWN_RATE;
		
		if (fabsf(directPos) > m_obContentSize.height - GAME_RACE_DIRECT_SIZE * 2 - GAME_RACE_DIRECT_MIN) {
			if (directPos > 0 && directRate > 0) {
				directRate = 0;
			} else if (directPos < 0 && directRate < 0) {
				directRate = 0;
			}
		}
		
		int r = rand();
		directRate += r % GAME_RACE_DIRECT_RATE - (GAME_RACE_DIRECT_RATE / 2);
		directPos += directRate * fDeltha;
		
		spawnNode(m_obContentSize.height / 2 - GAME_RACE_DIRECT_SIZE + directPos + GAME_RACE_DIRECT_SHIFT, diff,
			FigureBorder, GAME_BORDER_DURABILITY, GAME_BORDER_DURABILITY);
		spawnNode(m_obContentSize.height / 2 + GAME_RACE_DIRECT_SIZE + directPos + GAME_RACE_DIRECT_SHIFT, diff,
			FigureBorder, GAME_BORDER_DURABILITY, GAME_BORDER_DURABILITY);
		
		spawnTimer -= GAME_SPAWN_RATE;
	}

	static float randomSpawnRate = GAME_RANDOM_START;
	
	if (randomSpawnRate < 0) {
		spawnRandomNode(GAME_RACE_DIRECT_SIZE + directPos - 100,
			- GAME_RACE_DIRECT_SIZE + directPos + 100);
		
		randomSpawnRate = GAME_RANDOM_MIN + rand() % (GAME_RANDOM_MAX - GAME_RANDOM_MIN);
	}
	
	randomSpawnRate -= pathDeltha;
	gamePathLength += pathDeltha;

	gameSpeedRate += fDeltha * gameSpeedAcceleration;
}

void GameRoot::collisionCheck() {
	// keep array safe
	cleanupArray->removeAllObjects();

	const CCRect &rect = figure->boundingBox();
	CCObject *obj;
	CCSprite *child;

	CCARRAY_FOREACH(getChildren(), obj) {
		child = dynamic_cast<CCSprite *> (obj);
		if (child && child != figure) {
			if (child->getPositionX() < -child->boundingBox().size.width / 2) {
				cleanupArray->addObject(child);
			} else {
				// check for collision
				if (rect.intersectsRect(child->boundingBox())) {
					processCollision(figure, child);
				}
			}
		}
	}

}

void GameRoot::collisionCleanup() {
	CCObject *obj;
	CCNode *child;

	CCARRAY_FOREACH(cleanupArray, obj) {
		child = dynamic_cast<CCNode *> (obj);
		if (child) {
			removeChild(child, true);
		}
	}
}

void GameRoot::updatePlayerPosition(float fDeltha) {
	// accelerometer amplifier for high speed driving
	float accelMod = (gameSpeedRate > GAME_SPEED_START) ? (GAME_ACCEL_INTERVAL * log2f(gameSpeedRate - GAME_SPEED_START + 1) / 20) : (0);

	// speed for player movement
	float speedRate = ccpDistance(figureTargetPoint, figureStartPoint) / (GAME_ACCEL_INTERVAL - accelMod);

	const CCPoint &pos = figure->getPosition();
	float distance = ccpDistance(pos, figureTargetPoint);

	// if target point is reachable in this step on current speed - new poistion is target point
	// in other case - next position calculating with vector's math
	CCPoint nextPos = (fDeltha * speedRate < distance) ? (ccpAdd(pos, ccpMult(ccpSub(figureTargetPoint, pos), fDeltha * speedRate / distance))) : (figureTargetPoint);

	const CCSize &s = figure->boundingBox().size;

	// do not allow player stay beside the borders
	if (nextPos.x < s.width / 2) nextPos.x = s.width / 2;
	if (nextPos.x > m_obContentSize.width - s.width / 2) nextPos.x = m_obContentSize.width - s.width / 2;

	if (nextPos.y < s.height / 2) nextPos.y = s.height / 2;
	if (nextPos.y > m_obContentSize.height - s.height / 2) nextPos.y = m_obContentSize.height - s.height / 2;

	figure->setPosition(nextPos);

	FigureValues *p = dynamic_cast<FigureValues *> (figure->getUserObject());
	float shields = p->getParameter();
	shields += GAME_PARAM_SHIELD_REGEN *fDeltha;
	if (shields > GAME_PARAM_SHIELD) {
		shields = GAME_PARAM_SHIELD;
	}
	p->setParameter(shields);
}

void GameRoot::updateWorldPosition(float fDeltha) {
	float shift = gameSpeedRate * fDeltha;
	CCObject *obj;
	CCSprite *child;

	CCARRAY_FOREACH(getChildren(), obj) {
		child = dynamic_cast<CCSprite *> (obj);
		if (child && child != figure) {
			child->setPositionX(child->getPositionX() - shift);
		}
	}
}

void GameRoot::update(float fDelta) {
    if (gameStatus != StatusGame) return;
    
	collisionCheck();
	collisionCleanup();

	updatePlayerPosition(fDelta);
	updateWorldPosition(fDelta);

	raceSpawnProc(fDelta);
    
    gameTimer += fDelta;
    
	if (gameSpeedRate < GAME_SPEED_START) gameSpeedRate = GAME_SPEED_START;
	if (gameSpeedRate > gameSpeedMax) gameSpeedRate = gameSpeedMax;
    if (gameMaxSpeed < gameSpeedRate) gameMaxSpeed = gameSpeedRate;
    
    if (gamePathLength > GAME_LENGTH_MAX) {
        gameStatus = StatusWin;
    }
}

GameRoot::~GameRoot() {
	CC_SAFE_DELETE(cleanupArray);
}

void GameRoot::figureMoveBy(const CCPoint& point) {
	figureMoveTo(ccpAdd(figure->getPosition(), point));
}

void GameRoot::figureMoveTo(const CCPoint &point) {
	figureStartPoint = figure->getPosition();
	figureTargetPoint = point;
}

void GameRoot::figureDropPosition() {
	figureStartPoint = figure->getPosition();
	figureTargetPoint = figure->getPosition();
}

void GameRoot::setAcceleration(float a) {
	//if (a < 0) a *= GAME_SPEED_BREAKS;
    if (a < -0.1) {
        a = 0;
    } else {
        a = 0.1;
    }
	gameSpeedAcceleration = a * GAME_SPEED_FACTOR;
}

float GameRoot::getVelocity() {
	return gameSpeedRate;
}

float GameRoot::getVelocityScale() {
	return (gameSpeedRate - GAME_SPEED_START) / (gameSpeedMax - GAME_SPEED_START);
}

float GameRoot::getDurability() {
	float durability = dynamic_cast<FigureValues *> (figure->getUserObject())->getDurability();
	return durability / GAME_PARAM_DURABILITY;
}

float GameRoot::getShields() {
	float shields = dynamic_cast<FigureValues *> (figure->getUserObject())->getParameter();
	return shields / GAME_PARAM_SHIELD;
}

float GameRoot::getTimer() {
    return gameTimer;
}

float GameRoot::getDistanceScale() {
    return gamePathLength / GAME_LENGTH_MAX;
}

int GameRoot::getGameStatus() {
    return (int)gameStatus;
}

int GameRoot::getScore() {
    return gameScore;
}

float GameRoot::getGameMaxSpeed() {
    return gameMaxSpeed;
}

float GameRoot::getGameAvgSpeed() {
    return gamePathLength / gameTimer;
}

int GameRoot::getFlags() {
    return flags;
}