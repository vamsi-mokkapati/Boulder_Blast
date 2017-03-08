#include "Actor.h"
#include "StudentWorld.h"


#include "actor.h"
#include "StudentWorld.h"
#include <cstdlib>
#include <iostream>
using namespace std;

Actor::Actor(int imageID, int startX, int startY, StudentWorld* world, Direction dir)
:GraphObject(imageID, startX, startY, dir)
{
    m_world = world;
    setVisible(true);
    m_Alive = true;
    m_reveal = 0;
    ImageID = imageID;
    m_isVisible = false;
}

int Actor::getImageID()
{
    return ImageID;
}

StudentWorld* Actor::getWorld()
{
    return m_world;
}

bool Actor::checkAlive()
{
    return m_Alive;
}

void Actor::makeDead()
{
    m_Alive = false;
}

bool Actor::checkVisible()
{
    return m_isVisible;
}

void Actor::Reveal()
{
    //reveal exit
    
    m_reveal++;
    setVisible(true);
    m_isVisible = true;
    //only play sound first time
    if(m_reveal == 1)
        getWorld()->playSound(SOUND_REVEAL_EXIT);
}

void Actor::ifPushed(Direction dir)
{
    //if its pushed, and push is possible then move
    
    if(getWorld()->pushIsPossible(getX(), getY()))
    {
        switch(dir)
        {
            case(left):
            {
                moveTo(getX() - 1, getY());
                break;
            }
                
            case(right):
            {
                moveTo(getX() + 1, getY());
                break;
            }
                
            case(up):
            {
                moveTo(getX(), getY() + 1);
                break;
            }
                
            case(down):
            {
                moveTo(getX(), getY() - 1);
                break;
            }
                
            case(none):
                break;
                
        }
    }
    
}




Wall::Wall(int startX,int startY, StudentWorld* world)
:Actor(IID_WALL, startX, startY, world, none)
{
}

void Wall::ifAttacked()
{}

void Wall::doSomething()
{}

Player::Player(int startX,int startY, StudentWorld* world)
:Actor(IID_PLAYER, startX, startY, world, right), m_hitpoints(20), m_rounds(20)
{
}


void Player::doSomething()
{
    //get key and act appropriately based on key
    
    int ch;
    if(getWorld()->isGameOver())
        return;
    if(getWorld()->getKey(ch))
    {
        switch(ch)
        {
            case KEY_PRESS_ESCAPE:
                getWorld()->decLives();
                makeDead();
                if(getWorld()->isGameOver())
                    return;
                break;
                
            case KEY_PRESS_SPACE:
                if(m_rounds > 0)
                {
                    getWorld()->newBullet(getX(), getY(), getDirection());
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    m_rounds--;
                }
                break;
                
            case KEY_PRESS_LEFT:
                if(getX() > 0)
                {
                    setDirection(left);
                    if(!getWorld()->checkForActorPlayer(getX()-1,getY(),left))
                           moveTo(getX()-1,getY());
                }
                break;
                
            case KEY_PRESS_RIGHT:
                if(getX() < VIEW_WIDTH-1)
                {
                    setDirection(right);
                    if(!getWorld()->checkForActorPlayer(getX()+1,getY(),right))
                        moveTo(getX()+1,getY());
                }
                break;
                
            case KEY_PRESS_UP:
                if(getY() < VIEW_HEIGHT-1)
                {
                    setDirection(up);
                    if(!getWorld()->checkForActorPlayer(getX(),getY()+1,up))
                        moveTo(getX(),getY()+1);
                }
                break;
                
            case KEY_PRESS_DOWN:
                if(getY() > 0)
                {
                    setDirection(down);
                    if(!getWorld()->checkForActorPlayer(getX(),getY()-1,down))
                        moveTo(getX(),getY()-1);
                }
                break;
        }
    }
}

void Player::ifAttacked()
{
    //subtract helath and if its 0, kill player so it can restart level or if no lives are left, game over
    m_hitpoints -= 2;
    if(m_hitpoints>0)
        getWorld()->playSound(SOUND_PLAYER_IMPACT);
    else
    {
        getWorld()->playSound(SOUND_PLAYER_DIE);
        getWorld()->decLives();
        makeDead();
    }
    
}

int Player::gethitpoints()
{
    return m_hitpoints;
}
int Player::getrounds()
{
    return m_rounds;
}

void Player::incHitpoints()
{
    m_hitpoints = 20;
}
void Player::incRounds()
{
    m_rounds += 20;
}



Boulder::Boulder(int startX,int startY, StudentWorld* world)
:Actor(IID_BOULDER, startX, startY, world, none), m_hitpoints(10)
{
}

void Boulder::doSomething()
{}

void Boulder::ifAttacked()
{
    m_hitpoints -= 2;
    if(m_hitpoints <= 0)
        makeDead();
}




Bullet::Bullet(int startX,int startY, StudentWorld* world, Direction dir)
:Actor(IID_BULLET, startX, startY, world, dir)
{
    count = 0;
}

void Bullet::doSomething()
{
    
    //return if its dead
    if(!checkAlive())
        return;
    
    //if theres an actor on the square its dead
    if(getWorld()->checkForActorBullet(getX(),getY()))
    {
        makeDead();
        return;
    }
    
    //dont do anything the first tick
    if(count == 0)
    {
        count++;
        return;
    }
    
    //move one square in direction
    Direction dir = getDirection();
    switch(dir)
    {
        case(left):
        {
            moveTo(getX() - 1, getY());
            break;
        }
            
        case(right):
        {
            moveTo(getX() + 1, getY());
            break;
        }
            
        case(up):
        {
            moveTo(getX(), getY() + 1);
            break;
        }
            
        case(down):
        {
            moveTo(getX(), getY() - 1);
            break;
        }
            
        case(none):
            break;
            
    }
    
    //if theres an actor on the square its dead
    if(getWorld()->checkForActorBullet(getX(),getY()))
    {
        makeDead();
        return;
    }
    
    
    
}


void Bullet::ifAttacked()
{}



Hole::Hole(int startX,int startY, StudentWorld* world)
:Actor(IID_HOLE, startX, startY, world, none)
{
}

void Hole::doSomething()
{
    //if its dead, return
    if(!checkAlive())
        return;
    
    //if theres a boulder on the square, kill hole and boulder
    if(getWorld()->checkForBoulder(getX(),getY()))
    {
        makeDead();
        getWorld()->makeBoulderDead(getX(),getY());
    }
       
    
}

void Hole::ifAttacked()
{}



Jewel::Jewel(int startX,int startY, StudentWorld* world)
:Actor(IID_JEWEL, startX, startY, world, none)
{
}


void Jewel::doSomething()
{
    //if its dead, return
    if(!checkAlive())
        return;
    
    //if there is a player, kill itself, reward player
    if(getWorld()->checkForPlayer(getX(),getY()))
    {
        getWorld()->increaseScore(50);
        makeDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
    }
    
    
}

void Jewel::ifAttacked()
{}


   
   
Exit::Exit(int startX,int startY, StudentWorld* world)
:Actor(IID_EXIT, startX, startY, world, none)
{
    //make sure exit is not visible initially
    setVisible(false);
}

   
void Exit::doSomething()
{
    //if its visible and player is on it, finish level
    if((checkVisible() == true) && (getWorld()->checkForPlayer(getX(),getY()) == true))
    {
        getWorld()->playSound(SOUND_FINISHED_LEVEL);
        getWorld()->finishedLevel();
    }
}
   
void Exit::ifAttacked()
{}



Goodie::Goodie(int startX, int startY, StudentWorld* world, int imageID)
:Actor(imageID, startX, startY, world, none)
{
    
}



ExtraLife::ExtraLife(int startX,int startY, StudentWorld* world)
:Goodie(startX, startY, world, IID_EXTRA_LIFE)
{
}


void ExtraLife::doSomething()
{
    //if its dead, return
    if(!checkAlive())
        return;
    
    //if theres a player on the same square, reward player appropriately
    if(getWorld()->checkForPlayer(getX(),getY()))
    {
        getWorld()->increaseScore(1000);
        makeDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->incLives();
    }
    
    
}



Health::Health(int startX,int startY, StudentWorld* world)
:Goodie(startX, startY, world, IID_RESTORE_HEALTH)
{
    
}

void Health::doSomething()
{
    //if its dead, return
    if(!checkAlive())
        return;
    
    //if theres a player on the same square, reward player appropriately
    if(getWorld()->checkForPlayer(getX(),getY()))
    {
        getWorld()->increaseScore(500);
        makeDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseHitpoints();
    }
    
    
}




Ammo::Ammo(int startX,int startY, StudentWorld* world)
:Goodie(startX, startY, world, IID_AMMO)
{
    
}

void Ammo::doSomething()
{
    //if its dead, return
    if(!checkAlive())
        return;
    
    //if theres a player on the same square, reward player appropriately
    if(getWorld()->checkForPlayer(getX(),getY()))
    {
        getWorld()->increaseScore(100);
        makeDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseRounds();
    }
    
}




Robot::Robot(int startX, int startY, StudentWorld* world, int imageID, Direction dir)
:Actor(imageID, startX, startY, world, dir)
{
    
}

bool Robot::determineFire(Direction dir)
{
    switch(dir)
    {
        case(left):
        {
            //if theres a player and no actor in between, shoot bullet
            for(int i = 1;!getWorld()->checkforActorSnarlBotBullet(getX()-i,getY());i++)
            {
                if(getWorld()->checkForPlayer(getX()-i,getY()))
                {
                    getWorld()->newBullet(getX(),getY(), left);
                    getWorld()->playSound(SOUND_ENEMY_FIRE);
                    return true;
                }
            }
            
            break;
        }
            
        case(right):
        {
            //if theres a player and no actor in between, shoot bullet
            for(int i = 1;!getWorld()->checkforActorSnarlBotBullet(getX()+i,getY());i++)
            {
                if(getWorld()->checkForPlayer(getX()+i,getY()))
                {
                    getWorld()->newBullet(getX(),getY(), right);
                    getWorld()->playSound(SOUND_ENEMY_FIRE);
                    return true;
                }
            }
            
            break;
        }
            
        case(up):
        {
            //if theres a player and no actor in between, shoot bullet
            for(int i = 1;!getWorld()->checkforActorSnarlBotBullet(getX(),getY() + i) ;i++)
            {
                if(getWorld()->checkForPlayer(getX(),getY() + i))
                {
                    getWorld()->newBullet(getX(),getY(), up);
                    getWorld()->playSound(SOUND_ENEMY_FIRE);
                    return true;
                }
            }
            
            break;
            
        }
            
        case(down):
        {
            //if theres a player and no actor in between, shoot bullet
            for(int i = 1;!getWorld()->checkforActorSnarlBotBullet(getX(),getY() - i) ;i++)
            {
                if(getWorld()->checkForPlayer(getX(),getY() - i))
                {
                    getWorld()->newBullet(getX(),getY(), down);
                    getWorld()->playSound(SOUND_ENEMY_FIRE);
                    return true;
                }
            }
            break;
        }
            
        case(none):
            break;
            
    }
    return false;
}


void Robot::klepto(int &ID, int &distanceBeforeTurning)
{
    //if theres a goodie, and hasnt picked up one, 1/10 chance it picks it up
    if(getWorld()->checkForGoodie(getX(),getY()) && ID != -1)
    {
        if(rand() % 10 + 1 == 1)
        {
            ID  = getWorld()->getActorID(getX(),getY());
            getWorld()->playSound(SOUND_ROBOT_MUNCH);
            return;
        }
    }
    
    //if it hasnt moved the distance yet
    if(distanceBeforeTurning != 0)
    {
        Direction dir = getDirection();
        switch(dir)
        {
            case(left):
            {
                //if it can still move, go one square
                if(!getWorld()->checkForActorKleptobot(getX() - 1,getY()))
                {
                    moveTo(getX() - 1, getY());
                    distanceBeforeTurning--;
                    return;
                }
                
                break;
            }
                
            case(right):
            {
                //if it can still move, go one square
                if(!getWorld()->checkForActorKleptobot(getX() + 1,getY()))
                {
                    moveTo(getX() + 1, getY());
                    distanceBeforeTurning--;
                    return;
                }
                
                break;
            }
                
            case(up):
            {
                //if it can still move, go one square
                if(!getWorld()->checkForActorKleptobot(getX(),getY() + 1))
                {
                    moveTo(getX(), getY() + 1);
                    distanceBeforeTurning--;
                    return;
                }
                
                break;
            }
                
            case(down):
            {
                //if it can still move, go one square
                if(!getWorld()->checkForActorKleptobot(getX(),getY() - 1))
                {
                    moveTo(getX(), getY() - 1);
                    distanceBeforeTurning--;
                    return;
                }
                
                break;
            }
                
            case(none):
                break;
                
        }
        
    }
    
    //make new random distance before turning
    distanceBeforeTurning = rand() % 6 + 1;
    
    //select new random direction
    int dir = rand() % 4 + 1;
    Direction d = none;
    if(dir == 1)
        d = up;
    if(dir == 2)
        d = down;
    if(dir == 3)
        d = left;
    if(dir == 4)
        d = right;
    
    
    //check all directions, starting with inital direction. if bot can move, then move and return
    if(d == up)
    {
        if(!getWorld()->checkForActorKleptobot(getX(), getY() + 1))
        {
            setDirection(up);
            moveTo(getX(),getY() + 1);
            return;
        }
        
        if(!getWorld()->checkForActorKleptobot(getX(), getY() - 1))
        {
            setDirection(down);
            moveTo(getX(),getY() - 1);
            return;
        }
        
        if(!getWorld()->checkForActorKleptobot(getX() - 1, getY()))
        {
            setDirection(left);
            moveTo(getX() - 1,getY());
            return;
        }
        
        if(!getWorld()->checkForActorKleptobot(getX() + 1, getY()))
        {
            setDirection(right);
            moveTo(getX() + 1,getY());
            return;
        }
    }
    
    if(d == down)
    {
        if(!getWorld()->checkForActorKleptobot(getX(), getY() - 1))
        {
            setDirection(down);
            moveTo(getX(),getY() - 1);
            return;
        }
        
        if(!getWorld()->checkForActorKleptobot(getX(), getY() + 1))
        {
            setDirection(up);
            moveTo(getX(),getY() + 1);
            return;
        }
        
        if(!getWorld()->checkForActorKleptobot(getX() - 1, getY()))
        {
            setDirection(left);
            moveTo(getX() - 1,getY());
            return;
        }
        
        if(!getWorld()->checkForActorKleptobot(getX() + 1, getY()))
        {
            setDirection(right);
            moveTo(getX() + 1,getY());
            return;
        }
    }
    
    if(d == left)
    {
        if(!getWorld()->checkForActorKleptobot(getX() - 1, getY()))
        {
            setDirection(left);
            moveTo(getX() - 1,getY());
            return;
        }
        if(!getWorld()->checkForActorKleptobot(getX(), getY() + 1))
        {
            setDirection(up);
            moveTo(getX(),getY() + 1);
            return;
        }
        
        if(!getWorld()->checkForActorKleptobot(getX(), getY() - 1))
        {
            setDirection(down);
            moveTo(getX(),getY() - 1);
            return;
        }
        if(!getWorld()->checkForActorKleptobot(getX() + 1, getY()))
        {
            setDirection(right);
            moveTo(getX() + 1,getY());
            return;
        }
    }
    
    if(d == right)
    {
        if(!getWorld()->checkForActorKleptobot(getX() + 1, getY()))
        {
            setDirection(right);
            moveTo(getX() + 1,getY());
            return;
        }
        if(!getWorld()->checkForActorKleptobot(getX(), getY() + 1))
        {
            setDirection(up);
            moveTo(getX(),getY() + 1);
            return;
        }
        
        if(!getWorld()->checkForActorKleptobot(getX(), getY() - 1))
        {
            setDirection(down);
            moveTo(getX(),getY() - 1);
            return;
        }
        
        if(!getWorld()->checkForActorKleptobot(getX() - 1, getY()))
        {
            setDirection(left);
            moveTo(getX() - 1,getY());
            return;
        }
    }
    
    //if it cant move, set direction to the random one it had initally
    setDirection(d);
}

void Robot::determineMove(Direction dir)
{
    switch(dir)
    {
        case(left):
        {
            //if it didnt shoot, move in the direction its facing if it can
            if(!getWorld()->checkforActorSnarlBot(getX()-1,getY()))
                moveTo(getX() - 1, getY());
            //if not, switch directions
            else
                setDirection(right);
            break;
        }
            
        case(right):
        {
            //if it didnt shoot, move in the direction its facing if it can
            if(!getWorld()->checkforActorSnarlBot(getX()+1,getY()))
                moveTo(getX() + 1, getY());
            //if not, switch directions
            else
                setDirection(left);
            break;
        }
            
        case(up):
        {
            //if it didnt shoot, move in the direction its facing if it can
            if(!getWorld()->checkforActorSnarlBot(getX(),getY() + 1))
                moveTo(getX(), getY() + 1);
            //if not, switch directions
            else
                setDirection(down);
            break;
            
        }
            
        case(down):
        {
            //if it didnt shoot, move in the direction its facing if it can
            if(!getWorld()->checkforActorSnarlBot(getX(),getY() - 1))
                moveTo(getX(), getY() - 1);
            //if not, switch directions
            else
                setDirection(up);
            break;
        }
            
        case(none):
            break;
            
    }

}


Snarlbot::Snarlbot(int startX, int startY, StudentWorld* world, Direction dir)
:Robot(startX, startY, world, IID_SNARLBOT, dir), m_hitpoints(10), m_count(0)
{
    
}

void Snarlbot::doSomething()
{
    //if its dead, return dont do anything
    if(!checkAlive())
        return;
    
    //make sure it doesnt do something every tick
    
    m_count++;
    
    int ticks = (28 - getWorld() -> getLevel())/ 4; // levelNumber is the current
                                        // level number (0, 1, 2, etc.)
    if (ticks < 3)
        ticks = 3; // no SnarlBot moves more frequently than this
    
    if(m_count % ticks != 0)
        return;
    
    Direction dir = getDirection();
    
    if(determineFire(dir))
        return;
    
    //for each direction
    
    determineMove(dir);

    
}

void Snarlbot::ifAttacked()
{
    //if its attacked, subtract from health, if its dead, reward player, and make dead
    m_hitpoints -= 2;
    if(m_hitpoints == 0)
    {
        getWorld()->playSound(SOUND_ROBOT_DIE);
        getWorld()->increaseScore(100);
        makeDead();
    }
    else
        getWorld()->playSound(SOUND_ROBOT_IMPACT);

}


Kleptobot::Kleptobot(int startX, int startY, StudentWorld* world, int imageID)
:Robot(startX, startY, world, imageID, right), m_hitpoints(5), m_count(0)
{
    //initalize distancebeforeturning and ID, which is type of goodie it drops
    distanceBeforeTurning = rand() % 6 + 1;
    ID = -1;
}


void Kleptobot::doSomething()
{
    //if its not alive return
    if(!checkAlive())
        return;
    
    //make sure it doesnt do something every tick
    m_count++;
    
    int ticks = (28 - getWorld() -> getLevel())/ 4; // levelNumber is the current
    // level number (0, 1, 2, etc.)
    if (ticks < 3)
        ticks = 3; // no SnarlBot moves more frequently than this
    
    if(m_count % ticks != 0)
        return;
    
    
    klepto(ID, distanceBeforeTurning);
    
}

//damage function
void Kleptobot::ifAttacked()
{
    //if its attacked, subtract from health, if its dead, reward player, and make dead
    m_hitpoints -= 2;
    if(m_hitpoints <= 0)
    {
        getWorld()->newGoodie(getX(), getY(), ID);
        getWorld()->playSound(SOUND_ROBOT_DIE);
        getWorld()->increaseScore(10);
        makeDead();
    }
    else
        getWorld()->playSound(SOUND_ROBOT_IMPACT);
    
}



AngryKleptobot::AngryKleptobot(int startX, int startY, StudentWorld* world, int imageID)
:Kleptobot(startX, startY,world,imageID), m_hitpoints(8)
{

}


void AngryKleptobot::doSomething()
{
    if(!checkAlive())
        return;
    
    
    //make sure it doesnt do something every tick
    m_count++;
    
    int ticks = (28 - getWorld() -> getLevel())/ 4; // levelNumber is the current
    // level number (0, 1, 2, etc.)
    if (ticks < 3)
        ticks = 3; // no SnarlBot moves more frequently than this
    
    if(m_count % ticks != 0)
        return;
    
    
    Direction dir = getDirection();
    
    if(determineFire(dir))
        return;
    
    
    klepto(ID, distanceBeforeTurning);
    
}

//damage function
void AngryKleptobot::ifAttacked()
{
    //if its attacked, subtract from health, if its dead, reward player, and make dead
    m_hitpoints -= 2;
    if(m_hitpoints <= 0)
    {
        getWorld()->newGoodie(getX(), getY(), ID);
        getWorld()->playSound(SOUND_ROBOT_DIE);
        getWorld()->increaseScore(20);
        makeDead();
    }
    else
        getWorld()->playSound(SOUND_ROBOT_IMPACT);
    
}




KleptobotFactory::KleptobotFactory(int startX, int startY, StudentWorld* world, string s)
:Actor(IID_ROBOT_FACTORY, startX, startY, world, none)
{
    type = s;
}

void KleptobotFactory::doSomething()
{
    //initalize count, which counts number of bots within 3X3
    int count = 0;
    
    
    //make boundaries for which factory checks for bots, accounting for boundaries of game
    int x1 = getX() - 3;
    if(x1 < 0)
        while(x1 < 0)
            x1++;
    
    int x2 = getX() + 3;
    if(x2 > 14)
        while(x2 > 14)
            x2--;
    
    int y1 = getY() - 3;
    if(y1 < 0)
        while(y1 < 0)
            y1++;
    
    int y2 = getY() + 3;
    if(y2 > 14)
        while(y2 > 14)
            y2--;
    
    int x = x1;
    int y = y1;
    
    
    //count number of klepto and angryklepto bots
    while(x <= x2)
    {
        y = y1;
        while(y <= y2)
        {
            if(getWorld()->checkForBot(x, y))
                count++;
            y++;
        }
        x++;
    }
    
    //if count is less than 3 and there isnt a bot on the factory, make a new bot based on the type of factory it is
    if(count < 3 && !getWorld()->checkForBot(getX(),getY()))
    {
        //1/50 chance
        if(rand() % 50 + 1 == 1)
        {
            if(type == "angry")
                getWorld()->newAngryKlepto(getX(), getY());
            if(type == "reg")
                getWorld()->newKlepto(getX(), getY());
            getWorld()->playSound(SOUND_ROBOT_BORN);
        }
    }
}

void KleptobotFactory::ifAttacked()
{}









