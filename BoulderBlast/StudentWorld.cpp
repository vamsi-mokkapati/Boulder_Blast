#include "StudentWorld.h"
#include <string>
#include "Level.h"
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>  // defines the manipulator setw
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(std::string assetDir)
: GameWorld(assetDir)
{
    m_bonus = 1000;
    m_player = nullptr;
    isFinished = false;
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    //get current level and use stringstreams to make the name
    int level = getLevel();
    ostringstream oss;
    oss.fill('0');
    oss << "level" << setw(2) << level << ".dat";

    //load level of name from above
    string	curLevel	=  oss.str();
    Level	lev(assetDirectory());
    Level::LoadResult result	=	lev.loadLevel(curLevel);
    //return correct result from level.h
    if (result	==	Level:: load_fail_bad_format)
        return GWSTATUS_LEVEL_ERROR;
    if(result	==	Level::load_fail_file_not_found || getLevel() == 99)
        return GWSTATUS_PLAYER_WON;
    
    //add new actors based on level.h
    for(int y = 0; y < VIEW_HEIGHT; y++)
    {
        for(int x = 0; x < VIEW_WIDTH; x++)
        {
            if(lev.getContentsOf(x,y) == Level::player)
            {
                m_player = new Player(x,y,this);
            }
            
            if(lev.getContentsOf(x,y) == Level::wall)
            {
                Wall* w = new Wall(x,y,this);
                m_actor.push_back(w);
            }
            
            if(lev.getContentsOf(x,y) == Level::boulder)
            {
                Boulder* b = new Boulder(x,y,this);
                m_actor.push_back(b);
            }
            
            if(lev.getContentsOf(x,y) == Level::hole)
            {
                Hole* h = new Hole(x,y,this);
                m_actor.push_back(h);
            }
            
            if(lev.getContentsOf(x,y) == Level::jewel)
            {
                Jewel* j = new Jewel(x,y,this);
                m_actor.push_back(j);
            }
            
            if(lev.getContentsOf(x,y) == Level::exit)
            {
                Exit* e = new Exit(x,y,this);
                m_actor.push_back(e);
            }
            
            if(lev.getContentsOf(x,y) == Level::extra_life)
            {
                ExtraLife* e = new ExtraLife(x,y,this);
                m_actor.push_back(e);
            }
            
            if(lev.getContentsOf(x,y) == Level::restore_health)
            {
                Health* h = new Health(x,y,this);
                m_actor.push_back(h);
            }
            
            if(lev.getContentsOf(x,y) == Level::ammo)
            {
                Ammo* a = new Ammo(x,y,this);
                m_actor.push_back(a);
            }
            
            if(lev.getContentsOf(x,y) == Level::horiz_snarlbot)
            {
                Snarlbot* s = new Snarlbot(x,y,this, Actor::right);
                m_actor.push_back(s);
            }
            
            if(lev.getContentsOf(x,y) == Level::vert_snarlbot)
            {
                Snarlbot* s = new Snarlbot(x,y,this, Actor::down);
                m_actor.push_back(s);
            }
            
            if(lev.getContentsOf(x,y) == Level::angry_kleptobot_factory)
            {
                KleptobotFactory* k = new KleptobotFactory(x,y,this,"angry");
                m_actor.push_back(k);
            }
            
            if(lev.getContentsOf(x,y) == Level::kleptobot_factory)
            {
                KleptobotFactory* k = new KleptobotFactory(x,y,this,"reg");
                m_actor.push_back(k);
            }
        }
        
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    //display text and send player's hitpoints and rounds as parameters
    setDisplayText(m_player->gethitpoints(),m_player->getrounds());
    
    //ask player to do something
    m_player->doSomething();
    
    //ask all actors to do something,
    for(int i = 0; i<m_actor.size(); i++)
    {
        m_actor[i]->doSomething();
 
        //make sure actor isn't dead
        if(!m_player->checkAlive())
        {
            m_bonus = 1000;
            return GWSTATUS_PLAYER_DIED;
        }
        
        //if finished level, act appropriately
        if(isFinished)
        {
            increaseScore(2000);
            increaseScore(m_bonus);
            isFinished = false;
            m_bonus = 1000;
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    
    //erase dead actors
    for(int i = 0; i < m_actor.size(); i++)
    {
        if(!m_actor[i]->checkAlive())
        {
            delete m_actor[i];
            m_actor.erase(m_actor.begin() + i);
            i--;
        }
    }

    m_bonus--; //subtract one bonus every tick
    
    
    //go through and see how many jewels are left
    int jewels = 0;
    for(int i = 0; i < m_actor.size(); i++)
    {
        if(m_actor[i]->getImageID() == IID_JEWEL)
            jewels++;
    }
    //if no more jewels, reveal exit
    if(jewels == 0)
    {
        for(int i = 0; i < m_actor.size(); i++)
        {
            if(m_actor[i]->getImageID() == IID_EXIT)
                m_actor[i]->Reveal();
        }
    }
    
    //check if level is finished, act appropriately
    if(isFinished)
    {
        increaseScore(2000);
        increaseScore(m_bonus);
        isFinished = false;
        m_bonus = 1000;
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    // return the proper result
    if (!m_player->checkAlive())
        return GWSTATUS_PLAYER_DIED;
    
    return GWSTATUS_CONTINUE_GAME;
    
}

void StudentWorld::cleanUp()
{
    //delete pointers to all actors, and delete actors
    for(int i = 0; i < m_actor.size(); i++)
    {
            delete m_actor[i];
            m_actor.erase(m_actor.begin() + i);
            i--;
    }
    //delete player
    delete m_player;
}


void StudentWorld::setDisplayText(int hitpoints, int rounds)
{
    //get appropriate information
    int score = getScore();
    int level = getLevel();
    unsigned int bonus = m_bonus;
    if(m_bonus <= 0)
        bonus = 0;
    int livesLeft = getLives();

    //print info in correct format
    ostringstream oss;
    oss.fill('0');
    oss << "Score: " << setw(7) << score << "  Level: " << setw(2) << level;
    oss.fill(' ');
    oss << "  Lives: " << setw(2) << livesLeft << "  Health: " << setw(3) << (static_cast<double>(hitpoints)/20)*100 << "%";
    oss << "  Ammo: " <<  setw(3) << rounds << "  Bonus: " << setw(4) << bonus << endl;

    string s = oss.str();
    
    setGameStatText(s); // calls our provided GameWorld::setGameStatText
}


bool StudentWorld::checkForActorPlayer(int x, int y, Actor::Direction dir)
{
    //check for actors for the player
    double x1, y1;
    for (int i = 0; i < m_actor.size(); i++)
    {
        //if its a boulder, it must be pushed by player
        if(m_actor[i]->getImageID()==IID_BOULDER)
        {
            
            x1 = m_actor[i]->getX();
            y1 = m_actor[i]->getY();
            if(x1 == x && y1 == y)
            {
                m_player->setDirection(dir);
                m_actor[i]->ifPushed(dir);
            }
            
        }
        
        //if its these things, the player is not stopped by them
        if(m_actor[i]->getImageID() == IID_JEWEL || m_actor[i]->getImageID() == IID_AMMO || m_actor[i]->getImageID() == IID_EXIT
           || m_actor[i]->getImageID() == IID_RESTORE_HEALTH || m_actor[i]->getImageID() == IID_EXTRA_LIFE)
            continue;
        
        //for other actors, check if they are on the same square as the player
        x1 = m_actor[i]->getX();
        y1 = m_actor[i]->getY();
        if(x1 == x && y1 == y)
            return true;
    }
    
    return false;

}

bool StudentWorld::checkForActorBullet(int x, int y)
{
    //check for actors for bullet
    
    //if its a player, its attacked
    double x1, y1;
    
    x1 = m_player->getX();
    y1 = m_player->getY();
    if(x1 == x && y1 == y)
    {
        m_player->ifAttacked();
        return true;
    }
    

    for (int i = 0; i < m_actor.size(); i++)
    {
        //if its these bullet can pass through
        if(m_actor[i]->getImageID()==IID_BULLET || m_actor[i]->getImageID()==IID_EXIT || m_actor[i]->getImageID()==IID_HOLE
           || m_actor[i]->getImageID()==IID_EXTRA_LIFE || m_actor[i]->getImageID()==IID_JEWEL || m_actor[i]->getImageID()==IID_AMMO
           || m_actor[i]->getImageID()==IID_RESTORE_HEALTH)
            continue;
        
        //if its these bullet attacks them
        if(m_actor[i]->getImageID()==IID_BOULDER || m_actor[i]->getImageID()==IID_KLEPTOBOT || m_actor[i]->getImageID()==IID_SNARLBOT
           || m_actor[i]->getImageID()==IID_ANGRY_KLEPTOBOT)
        {
            x1 = m_actor[i]->getX();
            y1 = m_actor[i]->getY();
            if(x1 == x && y1 == y)
            {
                m_actor[i]->ifAttacked();
                return true;
            }
        }
        
        //if its these, bullet does nothing
        if(m_actor[i]->getImageID()==IID_WALL || m_actor[i]->getImageID()==IID_ROBOT_FACTORY)
        {
            x1 = m_actor[i]->getX();
            y1 = m_actor[i]->getY();
            if(x1 == x && y1 == y)
            {
                for (int i = 0; i < m_actor.size(); i++)
                {
                    //check if theres a bot on the same square
                    if(m_actor[i]->getImageID()==IID_KLEPTOBOT || m_actor[i]->getImageID()==IID_ANGRY_KLEPTOBOT)
                    {
                        double x2 = m_actor[i]->getX();
                        double y2 = m_actor[i]->getY();
                        if(x1 == x2 && y1 == y2)
                        {
                            m_actor[i]->ifAttacked();
                            return true;
                        }
                    }
                }
                return true;
            }

        }
        
    }
    
    return false;
    
}


bool StudentWorld::checkforActorSnarlBot(int x, int y)
{
    //check actors for snarlbot
    double x1, y1;
    
    for (int i = 0; i < m_actor.size(); i++)
    {
        //if its these, snarlbot can go
        if(m_actor[i]->getImageID() == IID_BULLET || m_actor[i]->getImageID() == IID_AMMO || m_actor[i]->getImageID() == IID_JEWEL
           || m_actor[i]->getImageID() == IID_EXIT || m_actor[i]->getImageID() == IID_RESTORE_HEALTH
           || m_actor[i]->getImageID() == IID_EXTRA_LIFE)
            continue;
        
        //any others, check if its same square
        x1 = m_actor[i]->getX();
        y1 = m_actor[i]->getY();
        if(x1 == x && y1 == y)
            return true;
    }

    return false;
}

bool StudentWorld::checkforActorSnarlBotBullet(int x, int y)
{
    //check if an actor is between snarlbot and player when its trying to shoot
    
    double x1, y1;
    
    for (int i = 0; i < m_actor.size(); i++)
    {
        //if its these, bullet can still be shot
        if(m_actor[i]->getImageID() == IID_BULLET || m_actor[i]->getImageID() == IID_AMMO || m_actor[i]->getImageID() == IID_JEWEL
           || m_actor[i]->getImageID() == IID_EXIT || m_actor[i]->getImageID() == IID_RESTORE_HEALTH
           || m_actor[i]->getImageID() == IID_EXTRA_LIFE || m_actor[i]->getImageID() == IID_HOLE)
            continue;
        
        //if its these, can't shoot
        x1 = m_actor[i]->getX();
        y1 = m_actor[i]->getY();
        if(x1 == x && y1 == y)
            return true;
    }
    
    return false;
}

bool StudentWorld::checkForActorKleptobot(int x, int y)
{
    //check for actor for kleptobot
    
    double x1, y1;
    
    //if its player, cant move
    x1 = m_player->getX();
    y1 = m_player->getY();
    if(x1 == x && y1 == y)
        return true;
    
    for (int i = 0; i < m_actor.size(); i++)
    {
        //if its these, cant move
        if(m_actor[i]->getImageID()==IID_BOULDER || m_actor[i]->getImageID()== IID_WALL || m_actor[i]->getImageID()==IID_HOLE
           || m_actor[i]->getImageID()==IID_SNARLBOT || m_actor[i]->getImageID()==IID_KLEPTOBOT || m_actor[i]->getImageID()==IID_ANGRY_KLEPTOBOT
           || m_actor[i]->getImageID()==IID_ROBOT_FACTORY)
        {
            
            x1 = m_actor[i]->getX();
            y1 = m_actor[i]->getY();
            if(x1 == x && y1 == y)
                return true;
        }
        
    }
    return false;
    
}



void StudentWorld::newBullet(int x, int y, Actor::Direction dir)
{
    //make new bullet one square in actor's direction
    switch(dir)
    {
        case(Actor::left):
        {
            Bullet* b = new Bullet(x - 1,y,this,dir);
            m_actor.push_back(b);
            break;
        }
            
        case(Actor::right):
        {
            Bullet* b = new Bullet(x + 1,y,this,dir);
            m_actor.push_back(b);
            break;
        }
            
        case(Actor::up):
        {
            Bullet* b = new Bullet(x,y + 1,this,dir);
            m_actor.push_back(b);
            break;
        }
            
        case(Actor::down):
        {
            Bullet* b = new Bullet(x,y - 1,this,dir);
            m_actor.push_back(b);
            break;
        }
        
        case(Actor::none):
            break;
            
    }
    
}

void StudentWorld::newGoodie(int x, int y, int id)
{
    //make a new goodie(dropped by klepto) based on the ID passed through
    switch(id)
    {
        case(IID_RESTORE_HEALTH):
        {
            Health *h = new Health(x,y,this);
            m_actor.push_back(h);

            break;
        }
        
        case(IID_EXTRA_LIFE):
        {
            ExtraLife *e = new ExtraLife(x,y,this);
            m_actor.push_back(e);
            break;
        }
            
        case(IID_AMMO):
        {
            Ammo *a = new Ammo(x,y,this);
            m_actor.push_back(a);
            break;
        }
            
    }
}

void StudentWorld::newAngryKlepto(int x, int y)
{
    //make new angrykleptobot for robot factory
    AngryKleptobot *a = new AngryKleptobot(x,y,this, IID_ANGRY_KLEPTOBOT);
    m_actor.push_back(a);
}

void StudentWorld::newKlepto(int x, int y)
{
    //make new kleptobot for robot factory
    Kleptobot *k = new Kleptobot(x,y,this, IID_KLEPTOBOT);
    m_actor.push_back(k);
}


bool StudentWorld::checkForBoulder(int x, int y)
{
    //check for boulder on the same square
    double x1, y1;
    for (int i = 0; i < m_actor.size(); i++)
    {
        if(m_actor[i]->getImageID()==IID_BOULDER)
        {
            x1 = m_actor[i]->getX();
            y1 = m_actor[i]->getY();
            if(x1 == x && y1 == y)
                return true;
        }
    }
    return false;
}

bool StudentWorld::checkForPlayer(int x, int y)
{
    //check for player on the same square
    double x1, y1;
    x1 = m_player->getX();
    y1 = m_player->getY();
    if(x1 == x && y1 == y)
        return true;

    return false;
}

bool StudentWorld::checkForGoodie(int x, int y)
{
    //check for goodie to be picked up
    double x1, y1;
    
    for (int i = 0; i < m_actor.size(); i++)
    {
        if(m_actor[i]->getImageID() == IID_EXTRA_LIFE || m_actor[i]->getImageID() == IID_AMMO || m_actor[i]->getImageID() == IID_RESTORE_HEALTH)
        {
            x1 = m_actor[i]->getX();
            y1 = m_actor[i]->getY();
            if(x1 == x && y1 == y)
                return true;
        }
    }
    
    return false;
}

bool StudentWorld::checkForBot(int x, int y)
{
    //check if there is a klepto or angryklepto so factory knows how many there are
    double x1, y1;
    
    for (int i = 0; i < m_actor.size(); i++)
    {
        if(m_actor[i]->getImageID() == IID_KLEPTOBOT || m_actor[i]->getImageID() == IID_ANGRY_KLEPTOBOT)
        {
            x1 = m_actor[i]->getX();
            y1 = m_actor[i]->getY();
            if(x1 == x && y1 == y)
                return true;
        }
    }
    
    return false;
}

int StudentWorld::getActorID(int x, int y)
{
    //get ID of the goodie, and make it dead cause its picked up by klepto or angryklepto
    double x1, y1;
    
    for (int i = 0; i < m_actor.size(); i++)
    {
        if(m_actor[i]->getImageID() == IID_EXTRA_LIFE || m_actor[i]->getImageID() == IID_AMMO || m_actor[i]->getImageID() == IID_RESTORE_HEALTH)
        {
            x1 = m_actor[i]->getX();
            y1 = m_actor[i]->getY();
            if(x1 == x && y1 == y)
            {
                m_actor[i]->makeDead();
                return m_actor[i]->getImageID();
            }
            
        }
    }
    
    return -1;
}


void StudentWorld::makeBoulderDead(int x, int y)
{
    //boulder has no hitpoints so kill it
    for (int i = 0; i < m_actor.size(); i++)
    {
        if(m_actor[i]->getImageID()==IID_BOULDER)
        {
            double x1, y1;
            x1 = m_actor[i]->getX();
            y1 = m_actor[i]->getY();
            if(x1 == x && y1 == y)
                m_actor[i]->makeDead();
        }
    }

}



void StudentWorld::finishedLevel()
{
    //make level finished is true so move() function can end level
    isFinished = true;

}


void StudentWorld::increaseHitpoints()
{
    //make hitpoints 20 by calling function in actor.cpp
    m_player->incHitpoints();
}

void StudentWorld::increaseRounds()
{
    //increase rounds by 20 by calling function in actor.cpp
    m_player->incRounds();
}


bool StudentWorld::pushIsPossible(int x, int y)
{
    //check if its possible to push the boulder in each direction
    switch(m_player->getDirection())
    {
        case(Actor::left):
        {
            for (int i = 0; i < m_actor.size(); i++)
            {
                //special case for hole
                if(m_actor[i]->getImageID() != IID_HOLE && m_actor[i]->getImageID() != IID_EXIT)
                {
                    double x1, y1;
                    x1 = m_actor[i]->getX();
                    y1 = m_actor[i]->getY();
                    if(x1 == x-1 && y1 == y)
                        return false;
                }
            }

            return true;
            break;
        }
            
        case(Actor::right):
        {
            for (int i = 0; i < m_actor.size(); i++)
            {
                if(m_actor[i]->getImageID() != IID_HOLE && m_actor[i]->getImageID() != IID_EXIT)
                {
                    double x1, y1;
                    x1 = m_actor[i]->getX();
                    y1 = m_actor[i]->getY();
                    if(x1 == x+1 && y1 == y)
                        return false;
                }
            }

            return true;
            break;
        }
            
        case(Actor::up):
        {
            for (int i = 0; i < m_actor.size(); i++)
            {
                if(m_actor[i]->getImageID() != IID_HOLE && m_actor[i]->getImageID() != IID_EXIT)
                {
                    double x1, y1;
                    x1 = m_actor[i]->getX();
                    y1 = m_actor[i]->getY();
                    if(x1 == x && y1 == y+1)
                        return false;
                }
            }
            
            return true;
            break;
        }
            
        case(Actor::down):
        {
            for (int i = 0; i < m_actor.size(); i++)
            {
                if(m_actor[i]->getImageID() != IID_HOLE && m_actor[i]->getImageID() != IID_EXIT)
                {
                    double x1, y1;
                    x1 = m_actor[i]->getX();
                    y1 = m_actor[i]->getY();
                    if(x1 == x && y1 == y-1)
                        return false;
                }
            }
            
            return true;
            break;
        }
            
        case(Actor::none):
            break;
            
    }
    return false;
}




