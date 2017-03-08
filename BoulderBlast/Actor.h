#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <string>
using namespace std;

#include "GameWorld.h"

class StudentWorld;


class Actor : public GraphObject
{
public:
    Actor(int imageID, int startX, int startY, StudentWorld* world, Direction dir);
    virtual void doSomething() = 0;
    virtual void ifAttacked() = 0;
    virtual ~Actor(){}
    bool checkAlive();
    void makeDead();
    void Reveal();
    bool checkVisible();
    void ifPushed(Direction dir);
    int getImageID();
    StudentWorld* getWorld();
private:
    bool m_Alive;
    int m_reveal;
    StudentWorld* m_world;
    int ImageID;
    bool m_isVisible;
};

class Wall : public Actor
{
public:
    Wall(int x, int y, StudentWorld* world);
    virtual void doSomething();
    virtual void ifAttacked();
};

class Player : public Actor
{
public:
    Player(int x, int y, StudentWorld* world);
    virtual void doSomething();
    virtual void ifAttacked();
    void incHitpoints();
    void incRounds();
    int gethitpoints();
    int getrounds();
private:
    int m_hitpoints;
    int m_rounds;
};

class Boulder : public Actor
{
public:
    Boulder(int x, int y, StudentWorld* world);
    virtual void doSomething();
    virtual void ifAttacked();
private:
    int m_hitpoints;
    
};

class Bullet : public Actor
{
public:
    Bullet(int x, int y, StudentWorld* world, Direction dir);
    virtual void doSomething();
    virtual void ifAttacked();
private:
    int count;
};

class Hole : public Actor
{
public:
    Hole(int x, int y, StudentWorld* world);
    virtual void doSomething();
    virtual void ifAttacked();
};

class Jewel : public Actor
{
public:
    Jewel(int x, int y, StudentWorld* world);
    virtual void doSomething();
    virtual void ifAttacked();
};

class Exit : public Actor
{
public:
    Exit(int x, int y, StudentWorld* world);
    virtual void doSomething();
    virtual void ifAttacked();

};

class Goodie: public Actor
{
public:
    Goodie(int x, int y, StudentWorld* world, int imageID);
    virtual ~Goodie(){}
    virtual void doSomething() = 0;
    void ifAttacked(){}
};

class ExtraLife : public Goodie
{
public:
    ExtraLife(int x, int y, StudentWorld* world);
    virtual void doSomething();
};

class Health : public Goodie
{
public:
    Health(int x, int y, StudentWorld* world);
    virtual void doSomething();
};

class Ammo : public Goodie
{
public:
    Ammo(int x, int y, StudentWorld* world);
    virtual void doSomething();
};

class Robot : public Actor
{
public:
    Robot(int startX, int startY, StudentWorld* world, int imageID, Direction dir);
    virtual ~Robot(){}
    virtual void doSomething() = 0;
    virtual void ifAttacked() = 0;
    void klepto(int &ID, int &distanceBeforeTurning);
    bool determineFire(Direction dir);
    void determineMove(Direction dir);
};

class Snarlbot : public Robot
{
public:
    Snarlbot(int x, int y, StudentWorld* world, Direction dir);
    virtual void doSomething();
    virtual void ifAttacked();
private:
    int m_hitpoints;
    int m_count;
};

class Kleptobot : public Robot
{
public:
    Kleptobot(int x, int y, StudentWorld* world, int imageID);
    virtual void doSomething();
    virtual void ifAttacked();
private:
    int m_count;
    int m_hitpoints;
    int distanceBeforeTurning;
    int ID;
};

class AngryKleptobot : public Kleptobot
{
public:
    AngryKleptobot(int x, int y, StudentWorld* world, int imageID);
    virtual void doSomething();
    virtual void ifAttacked();
private:
    int m_count;
    int m_hitpoints;
    int distanceBeforeTurning;
    int ID;
};

class KleptobotFactory : public Actor
{
public:
    KleptobotFactory(int x, int y, StudentWorld* world, string s);
    virtual void doSomething();
    virtual void ifAttacked();
private:
    string type;
};

#endif // ACTOR_H_
