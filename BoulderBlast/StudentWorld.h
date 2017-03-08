#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <vector>
#include <string>
using namespace std;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    ~StudentWorld();

    virtual int init();
    virtual int move();
    virtual void cleanUp();

    bool checkForActorPlayer(int x, int y, Actor::Direction dir);
    bool checkForActorBullet(int x, int y);
    bool checkForBoulder(int x, int y);
    bool checkForPlayer(int x, int y);
    bool checkforActorSnarlBot(int x, int y);
    bool checkforActorSnarlBotBullet(int x, int y);
    bool checkForActorKleptobot(int x, int y);
    bool checkForGoodie(int x, int y);
    bool checkForBot(int x, int y);
    bool pushIsPossible(int x, int y);
    void finishedLevel();
    void increaseHitpoints();
    void increaseRounds();
    void makeBoulderDead(int x, int y);
    void setDisplayText(int hitpoints, int rounds);
    void newBullet(int x, int y, Actor::Direction dir);
    void newGoodie(int x, int y, int id);
    void newAngryKlepto(int x, int y);
    void newKlepto(int x, int y);
    int getActorID(int x, int y);

private:
    vector<Actor*> m_actor;
    Player* m_player;
    int m_bonus;
    bool isFinished;
};

#endif // STUDENTWORLD_H_
