#ifndef __GAME_SCENE__
#define __GAME_SCENE__

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "HeroBazzi.hpp"
#include "HeroCappi.hpp"
#include "HeroDao.hpp"
#include "HeroMarid.hpp"
#include "AniReader.hpp"
#include "BubbleClass.hpp"

#include "PropsSpeed.hpp"
#include "PropsRange.hpp"
#include "PropsBubbleNum.hpp"
#include "PropsGold.hpp"
#include "PropsSilver.hpp"
#include "Client.hpp"
#include "math.h"
USING_NS_CC;
using namespace CocosDenshion;
class GameScene : public cocos2d::CCLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback    	map->setScale(MAX(size.height/map->getContentSize().height,size.width/map->getContentSize().width));
    void menuButtonCallback(CCObject* pSender);
    void menuCloseCallback(CCObject* pSender);
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene);
    Hero* hero[10];
    int heronum;
    int myheroid;
    Bubble* bubble[85];
    int bhead;
    int btail;
    Props* props[85];
    int propsnum;
	CCSize size;
	virtual void ccTouchesBegan(CCSet* touches, CCEvent* pEvent);
	virtual void ccTouchesMoved(CCSet* touches, CCEvent* pEvent);
	virtual void ccTouchesEnded(CCSet* touches, CCEvent* pEvent);
	virtual void ccTouchesCancelled(CCSet* touches, CCEvent* pEvent);
    virtual void registerWithTouchDispatcher();
    void sendmessage();
    void getcommand();
    void createhero(int type,CCPoint a,float scale);
    void createprops(int type,CCPoint a,float scale);
    void createbubble(CCPoint a,float scale,int range,int heroid);
    void BombCallback(CCNode* obj,void* id);
    void BubbleBomb(int idx);
    void test();
	CCPoint PositionToTileCoord(CCPoint cocosCoord);
	CCPoint TileCoordToPosition(CCPoint tileCoord);
    CCTMXTiledMap *map;
    //static
    CCTMXLayer *mapBackgroundLayer,*mapItemLayer;
    static CCScene *gameScene;
    CCLayer *statusLayer,*chatLayer;
    int doubleTouchCount,tripleTouchCount;
    CCPoint firstTripleTouchPoint;
    char sMsg[255], rMsg[255];
    void sendMoveMsg();
};
#include "UserData.hpp"
#include "GameSceneChatLayer.hpp"
#include "GameSceneMessageLayer.hpp"
#include "GameSceneStatusLayer.hpp"
CCScene *GameScene::gameScene;
//CCTMXLayer *GameScene::mapBackgroundLayer,*GameScene::mapItemLayer;
CCScene* GameScene::scene()
{
    // 'scene' is an autorelease object
    gameScene = CCScene::create();
    
    // 'layer' is an autorelease object
    CCLayer *gameLayer = GameScene::create();
    // add layer as a child to scene
    gameScene->addChild(gameLayer,0);
    // return the scene
    return gameScene;
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    //////////////////////////////
    // 1. super init first
	bhead = 0;
	btail = 0;
	heronum = 0;
	myheroid = 1;
	propsnum = 0;
    if ( !CCLayer::init() )
    {
        return false;
    }
    memset(sMsg, 0,sizeof(sMsg));
    memset(rMsg, 0,sizeof(rMsg));
    this->setTouchEnabled(true);
    SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(true);
	SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic("audio/bg_1.ogg");
	SimpleAudioEngine::sharedEngine()->playBackgroundMusic("audio/bg_1.ogg",true);
	SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(1.0f);
	SimpleAudioEngine::sharedEngine()->preloadEffect("audio/ef_0.ogg");
	SimpleAudioEngine::sharedEngine()->preloadEffect("audio/ef_2.ogg");
	SimpleAudioEngine::sharedEngine()->playEffect("audio/ef_7.ogg");
    size = CCDirector::sharedDirector()->getWinSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

    statusLayer=GameSceneStatusLayer::create();
    statusLayer->setPosition(ccp(0,size.height));
    chatLayer=GameSceneChatLayer::create();
    chatLayer->setPosition(ccp(0,-size.height));
    gameScene->addChild(chatLayer,1);
    gameScene->addChild(statusLayer,1);
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create("image/ui/CloseNormal.png","image/ui/CloseSelected.png",this,menu_selector(GameScene::menuCloseCallback));
    
	pCloseItem->setPosition(ccp(size.width - pCloseItem->getContentSize().width/2 ,
                                pCloseItem->getContentSize().height/2));


	CCMenuItemImage *pButtonItem = CCMenuItemImage::create("image/ui/item_BB.png","image/ui/item_BB.png",this,menu_selector(GameScene::menuButtonCallback));
	pButtonItem->setScale(2.0);
	pButtonItem->setPosition(ccp(size.width/10*9 - pButtonItem->getContentSize().width/2 ,
	                                size.height/2+ pButtonItem->getContentSize().height/2));
	// create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, pButtonItem,NULL);
    pMenu->setPosition(CCPointZero);
    gameScene->addChild(pMenu, 20);
    map=CCTMXTiledMap::create("map/map_fact.tmx");
    mapBackgroundLayer=map->layerNamed("background");
    mapItemLayer=map->layerNamed("item");
    if(map->getContentSize().height<size.height or map->getContentSize().width<size.width){
    	mapBackgroundLayer->setScale(MAX(size.height/map->getContentSize().height,size.width/map->getContentSize().width));
    	mapItemLayer->setScale(MAX(size.height/map->getContentSize().height,size.width/map->getContentSize().width));
    }
    this->addChild(mapItemLayer,3);
    this->addChild(mapBackgroundLayer,0);
    AniReader::mapw = mapBackgroundLayer->getMapTileSize().width*mapBackgroundLayer->getScale();
    AniReader::maph = mapBackgroundLayer->getMapTileSize().height*mapBackgroundLayer->getScale();
    //Bubble* bubble;
    //bubble = new Bubble(TileCoordToPosition(PositionToTileCoord(ccp(size.width/2,size.height/2))),mapBackgroundLayer->getScale(),1,1);
    /*Props* pp = new PropsSpeed();
    pp->create(TileCoordToPosition(PositionToTileCoord(ccp(size.width/2,size.height/2))),mapBackgroundLayer->getScale());
    this->addChild(pp->sprite,3);*/
    createprops(1,TileCoordToPosition(PositionToTileCoord(ccp(size.width/2,size.height/2))),mapBackgroundLayer->getScale());
    createhero(1,TileCoordToPosition(PositionToTileCoord(ccp(size.width/2,size.height/2))),0.8*mapBackgroundLayer->getScale());
    createhero(2,TileCoordToPosition(PositionToTileCoord(ccp(size.width/2,size.height/2))),0.8*mapBackgroundLayer->getScale());
    //hero = new HeroBazzi();
    //hero->createhero(TileCoordToPosition(PositionToTileCoord(ccp(size.width/2,size.height/2))),0.8*mapBackgroundLayer->getScale());
    //this->addChild(hero[myheroid]->sprite,11);
    //this->addChild(bubble->layer,10);
    //this->addChild(bubble->ups[1],1);
    //bubble->bomb(3,3,3,3);
    doubleTouchCount=0;tripleTouchCount=0;
     return true;
}

void GameScene::createhero(int type,CCPoint a,float s)
{
	if (type == 1)
	{
		hero[heronum] = new HeroBazzi();
	}
	else if (type == 2)
	{
		hero[heronum] = new HeroCappi();
	}
	else if (type == 3)
	{
		hero[heronum] = new HeroDao();
	}
	else if (type == 4)
	{
		hero[heronum] = new HeroMarid();
	}
	hero[heronum]->createhero(a,s);
	this->addChild(hero[heronum]->sprite,11);
	heronum++;
}

void GameScene::createbubble(CCPoint a,float s,int r,int heroid)
{
	if (bubble[btail] != NULL)
	{
		delete bubble[btail];
	}
	bubble[btail] = new Bubble(a,s,btail,hero[heroid]->bubble_range,heroid);
	CCFiniteTimeAction *delay;
	delay = CCDelayTime::create(3);
	CCCallFuncND * bomb = CCCallFuncND::create(this,callfuncND_selector(GameScene::BombCallback),(void*)&bubble[btail]->idx);
	CCAction *action;
	action = CCSequence::create(delay,bomb,NULL);
	action->setTag(0);
	bubble[btail]->sprite->runAction(action);
	this->addChild(bubble[btail]->layer,10);
	btail++;
	if (btail > 80)
	{
		btail -= 80;
	}
}

void GameScene::createprops(int type,CCPoint a,float scale)
{
	  if (type == 1)
	  {
		  props[propsnum] = new PropsSpeed();
	  }
	  else if (type == 2)
	  {
		  props[propsnum] = new PropsRange();
	  }
	  else if (type == 3)
	  {
		  props[propsnum] = new PropsBubbleNum();
	  }
	  else if (type == 4)
	  {
		  props[propsnum] = new PropsGold();
	  }
	  else if (type == 5)
	  {
		  props[propsnum] = new PropsSilver();
	  }
	  props[propsnum]->create(a,scale);
	  this->addChild(props[propsnum]->sprite,9);
	  propsnum++;
}
void GameScene::menuCloseCallback(CCObject* pSender)
{
	this->setTouchEnabled(false);
	SimpleAudioEngine::sharedEngine()->playEffect("audio/ef_0.ogg");
	if(statusLayer->getPositionY()!=size.height){
		CCAction *move=CCEaseExponentialOut::create(CCMoveTo::create(0.5,ccp(0,size.height)));
		statusLayer->runAction(move);
	}
	if(chatLayer->getPositionY()!=-size.height){
		CCAction *move=CCEaseExponentialOut::create(CCMoveTo::create(0.5,ccp(0,-size.height)));
		chatLayer->runAction(move);
	}
	CCLayer* messageLayer=GameSceneMessageLayer::create();
	gameScene->addChild(messageLayer,2);

}

void GameScene::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent){
}
void GameScene::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent){
}
void GameScene::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent){
	if(pTouches->count()==2 and tripleTouchCount==0){
		CCTouch* touch=dynamic_cast<CCTouch*>(*pTouches->begin());
		float x=ccpAdd(this->getPosition(),touch->getDelta()).x;
		float y=ccpAdd(this->getPosition(),touch->getDelta()).y;
		if(x>0)x=0;if(y>0)y=0;
		if(x<size.width-mapBackgroundLayer->boundingBox().size.width)x=size.width-mapBackgroundLayer->boundingBox().size.width;
		if(y<size.height-mapBackgroundLayer->boundingBox().size.height)y=size.height-mapBackgroundLayer->boundingBox().size.height;
		this->setPosition(ccp(x,y));
		doubleTouchCount=2;
	}
	if(pTouches->count()==3 and tripleTouchCount==0){
		firstTripleTouchPoint=dynamic_cast<CCTouch*>(*pTouches->begin())->getLocationInView();
		tripleTouchCount=3;
	}
	if(pTouches->count()==1 and doubleTouchCount==0 and tripleTouchCount==0)
	{
		CCTouch* touch=dynamic_cast<CCTouch*>(pTouches->anyObject());
		CCPoint aim=PositionToTileCoord(convertTouchToNodeSpace(touch));
		CCPoint origin=PositionToTileCoord(hero[myheroid]->sprite->getPosition());
		if(aim.x==origin.x and aim.y==origin.y)return;
		if(abs(aim.x-origin.x)<abs(aim.y-origin.y)){
			sprintf(sMsg, "%f %f", origin.x, origin.y+(aim.y-origin.y)/abs(aim.y-origin.y));
			//hero[myheroid]->moveto(TileCoordToPosition(ccp(origin.x,origin.y+(aim.y-origin.y)/abs(aim.y-origin.y))));
			scheduleOnce(schedule_selector(GameScene::sendMoveMsg), 0.02f);
		}
		else{
			sprintf(sMsg,"%f %f",origin.x+(aim.x-origin.x)/abs(aim.x-origin.x), origin.y );
			//hero[myheroid]->moveto(TileCoordToPosition(ccp(origin.x+(aim.x-origin.x)/abs(aim.x-origin.x),origin.y)));
			scheduleOnce(schedule_selector(GameScene::sendMoveMsg),0.02f);
		}
	}
}
void GameScene::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent){
	if(tripleTouchCount==3){
		CCTouch* touch=dynamic_cast<CCTouch*>(*pTouches->begin());
		if(touch->getLocationInView().y-firstTripleTouchPoint.y>100){
			if(chatLayer->getPositionY()!=-size.height){
				chatLayer->stopAllActions();
				CCAction *move=CCEaseExponentialOut::create(CCMoveTo::create(0.5,ccp(0,-size.height)));
				chatLayer->runAction(move);
			}else{
				statusLayer->stopAllActions();
				CCAction *move=CCEaseExponentialOut::create(CCMoveTo::create(0.5,ccp(0,0)));
				statusLayer->runAction(move);
			}
		}
		if(touch->getLocationInView().y-firstTripleTouchPoint.y<-100){
			if(statusLayer->getPositionY()!=size.height){
				statusLayer->stopAllActions();
				CCAction *move=CCEaseExponentialOut::create(CCMoveTo::create(0.5,ccp(0,size.height)));
				statusLayer->runAction(move);
			}else{
				chatLayer->stopAllActions();
				CCAction *move=CCEaseExponentialOut::create(CCMoveTo::create(0.5,ccp(0,0)));
				chatLayer->runAction(move);
			}
		}
	}
	if(doubleTouchCount>0)doubleTouchCount--;
	if(tripleTouchCount>0)tripleTouchCount--;
	if(pTouches->count()==1 and doubleTouchCount==0 and tripleTouchCount==0)
	{
		if(statusLayer->getPositionY()!=size.height){
			statusLayer->stopAllActions();
			CCAction *move=CCEaseExponentialOut::create(CCMoveTo::create(0.5,ccp(0,size.height)));
			statusLayer->runAction(move);
		}
		if(chatLayer->getPositionY()!=-size.height){
			chatLayer->stopAllActions();
			CCAction *move=CCEaseExponentialOut::create(CCMoveTo::create(0.5,ccp(0,-size.height)));
			chatLayer->runAction(move);
		}
		//hero[myheroid]->clearMove();
		//hero[myheroid]->stand();
		//hero->encase();
	}

}
void GameScene::registerWithTouchDispatcher()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addStandardDelegate(this, 0);
    CCLayer::registerWithTouchDispatcher();
}
CCPoint GameScene::PositionToTileCoord(CCPoint cocosCoord)
{
	int x=cocosCoord.x/mapBackgroundLayer->getScale()/mapBackgroundLayer->getMapTileSize().width;
	int y=((mapBackgroundLayer->getLayerSize().height-cocosCoord.y)/mapBackgroundLayer->getScale()) / mapBackgroundLayer->getMapTileSize().height;
	return ccp(x,y);
}
CCPoint GameScene::TileCoordToPosition(CCPoint tileCoord){
	float x=mapBackgroundLayer->getMapTileSize().width*(0.5+tileCoord.x)*mapBackgroundLayer->getScale();
	float y=(mapBackgroundLayer->getLayerSize().height-mapBackgroundLayer->getMapTileSize().height*(0.5+tileCoord.y-0.9))*mapBackgroundLayer->getScale();
	return ccp(x,y);
}
void GameScene::menuButtonCallback(CCObject* pSender)
{
	if (hero[myheroid]->bubble_num > 0)
	{
		hero[myheroid]->bubble_num--;
		SimpleAudioEngine::sharedEngine()->playEffect("audio/ef_0.ogg");
		createbubble(TileCoordToPosition(PositionToTileCoord(hero[myheroid]->sprite->getPosition())),mapBackgroundLayer->getScale(),hero[myheroid]->bubble_range,myheroid);
	}
}

void  GameScene::BubbleBomb(int idx)
{
	if (bubble[idx] != NULL && bubble[idx]->isdelay)
	{
		int r = bubble[idx]->range;
		hero[bubble[idx]->heroid]->bubble_num++;
		CCAction *action = NULL;
		action = bubble[idx]->sprite->getActionByTag(0);
		if (action != NULL)
		{
			bubble[idx]->sprite->stopAction(action);
		}
		bubble[idx]->bomb(r,r,r,r);
		int x,y;
		x = PositionToTileCoord(bubble[idx]->sprite->getPosition()).x;
		y = PositionToTileCoord(bubble[idx]->sprite->getPosition()).y;
		for (int i = 0; i < heronum; i++)
		{
			int x1 = PositionToTileCoord(hero[i]->sprite->getPosition()).x;
			int y1 = PositionToTileCoord(hero[i]->sprite->getPosition()).y;
			for (int j = 0; j <= r; j++)
			{
				if (x1 == (x+j) && y == y1)
				{
					hero[i]->encase();
				}
				else if (x1 == (x-j) && y == y1)
				{
					hero[i]->encase();
				}
				else if (x1 == x && (y+j) == y1)
				{
					hero[i]->encase();
				}
				else if (x1 == x && (y-j) == y1)
				{
					hero[i]->encase();
				}
			}
		}
		for (int i = 0; i < propsnum; i++)
		if (props[i]->isdelay)
		{
			int x1 = PositionToTileCoord(props[i]->sprite->getPosition()).x;
			int y1 = PositionToTileCoord(props[i]->sprite->getPosition()).y;
			for (int j = 0; j <= r; j++)
			{
				if (x1 == (x+j) && y == y1)
				{
					props[i]->remove();
				}
				else if (x1 == (x-j) && y == y1)
				{
					props[i]->remove();
				}
				else if (x1 == x && (y+j) == y1)
				{
					props[i]->remove();
				}
				else if (x1 == x && (y-j) == y1)
				{
					props[i]->remove();
				}
			}
		}
		for (int i = 0; i <= 80; i++)
		{
			if (i != idx && bubble[i] != NULL && bubble[i]->isdelay)
			{
				int x1 = PositionToTileCoord(bubble[i]->sprite->getPosition()).x;
				int y1 = PositionToTileCoord(bubble[i]->sprite->getPosition()).y;
				for (int j = 1; j <= r; j++)
				{
					if (x1 == (x+j) && y == y1)
					{
						BubbleBomb(i);
					}
					else if (x1 == (x-j) && y == y1)
					{
						BubbleBomb(i);
					}
					else if (x1 == x && (y+j) == y1)
					{
						BubbleBomb(i);
					}
					else if (x1 == x && (y-j) == y1)
					{
						BubbleBomb(i);
					}
				}
			}
		}

	}
}
void GameScene::BombCallback(CCNode* obj,void* id) {
	int idx = *((int*)id);
	BubbleBomb(idx);

}
void GameScene::sendMoveMsg(){
	float x, y;
	send(Client::client->sockfd,sMsg, sizeof(sMsg),0);
	recv(Client::client->sockfd,rMsg, sizeof(rMsg),0);
	sscanf(rMsg, "%f %f", &x, &y);
	hero[myheroid]->moveto(TileCoordToPosition(ccp(x, y)));
}
#endif

