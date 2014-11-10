#include "..\..\Engine\Advanced2D.h"
#include "LevelManager.h"
#include "JumpingSprite.h"
#include "ShootyGuy.h"
#include "Monster.h"
#include <sstream>

using namespace Advanced2D;

#define SCREENH 600
#define SCREENW 800

//TODO: Framerate killed by loading bullets... Fix it


////macro to read the keyboard asynchronously                                     
#define KEY_DOWN(vk) ((GetAsyncKeyState(vk) & 0x8000)?1:0)
void applyGravity(Sprite *sprite);
void jump(Sprite *sprite);
Sprite *loadBullet(void);
Sprite *closestEnemy();
void deactivateBullets();

bool standing = true;


enum AutoPilotMode { None, Shooting, Jumping };
void shoot(AutoPilotMode mode);

void monsterUpdate();
void bullSnort();
bool bullSnorting = false;
bool spriteOnScreen(Sprite *sprite);
void drawMonsters();
void autoMove();
void updateConsole();
Vector3 getFirstCollisionPosition(Sprite* sprite1, Sprite* sprite2);

enum Difficulty { AutoPilot, Easy };
enum CollisionSide { Left, Top, Right, Bottom, ZeroVelocity };

Difficulty difficulty = AutoPilot;
AutoPilotMode autoPilotMode = None;
CollisionSide getCollisionSide(Sprite* sprite1, Sprite* sprite2);

const float TERMINAL_VELOCITY = 8;
const float GRAVITY_RATE = 1;
void faceRight(bool right);
bool facingRight = true;


int shootTimerStart = 0;
int shootTimerInterval = 500;

int bullSnortTimerStart = 0;
int bullSnortTimerInterval = 5000;

const int BULLET_COUNT = 20;
const int MONSTER_COUNT = 30;
const int MONSTER_MOVE_SPEED = 1;
const int GRASS_BLOCK_COUNT = 7;

int lastFiredBullet = 19;
Sprite *bullets[BULLET_COUNT];
Sprite *monsters[MONSTER_COUNT];
Texture *grassTexture;
Sprite *grassBlocks[GRASS_BLOCK_COUNT];
Sprite *standingSurface = NULL;
Console* console;
std::ostringstream ostr;
bool checkOnSurface();
Timer backToBackCollisionTimer;

Sprite *bull;
Sprite *shootyGuy;
float moveSpeed = 5.0;
const float BULLET_SPEED_MODIFIER = 3.0;
float highPoint = 0;
Timer landCollisionTimer;
LevelManager* levelManager;

/*
    preload function is used to set up gameplay screen parameters
	Title, width, height, colordepth
*/
bool game_preload(){
	g_engine->setAppTitle("MotherFuckin Shooty Guy");
	g_engine->setScreenHeight(SCREENH);
	g_engine->setScreenWidth(SCREENW);
	g_engine->setColorDepth(32);
	g_engine->setMaximizeProcessor(false);
	return true;
}

/* game init function used to initialize parameters for individual pieces

*/
bool game_init(HWND){

	/*grassTexture = new Texture();
	if (!grassTexture->Load("DirtMGrass32.png", D3DCOLOR_XRGB(255, 174, 201)))
	{
		return 0;
	}

	for (int j = 0; j < GRASS_BLOCK_COUNT; j++)
	{
		for (int k = 0; k < GRASS_BLOCK_COUNT; k++){
		Sprite *grassBlock = new Sprite();
		grassBlock->setImage(grassTexture);
		grassBlock->setWidth(32);
		grassBlock->setHeight(32);
		grassBlock->setColumns(1);
		grassBlock->setTotalFrames(1);
		grassBlock->setCurrentFrame(0);
		grassBlock->setObjectType(OBJECT_SOLID_BLOCK);
		grassBlocks[j] = grassBlock;
		grassBlock->setAlive(true);
		grassBlock->setVisible(true);
		
		float xPos = j * 1.8 * 64 + k * 5;
		//float yPos = SCREENH -  rand() % (SCREENH / 4);
		float yPos = SCREENH - 96 * k + j;

		/*if (j % 2 == 0)
		{
			xPos = j * 1.8 * 64;
			yPos = SCREENH - 96 * k + j;

			xPos = 520;
			yPos = 415;
		}
		else
		{
			xPos = 330;
			yPos = 320;
		}* /

		grassBlock->setPosition(xPos, yPos);
		

		g_engine->addEntity(grassBlock);
	
		}
	} 
	*/
	shootyGuy = new Sprite();
	if (! shootyGuy->loadImage("Shooter guy.png", D3DCOLOR_XRGB(255,174,201)))
		return 0;
	shootyGuy->setColumns(2);
	shootyGuy->setTotalFrames(2);
	shootyGuy->setHeight(32);
	shootyGuy->setWidth(32);
	shootyGuy->setCurrentFrame(0);
	shootyGuy->setPosition(g_engine->getScreenWidth()/2, g_engine->getScreenHeight() - shootyGuy->getHeight());
	shootyGuy->setMoveTimer(15);
	shootyGuy->setObjectType(OBJECT_GUY);
	g_engine->addEntity(shootyGuy, DRAW_PRIORITY_FRONT);
	/*
	for (int i = 0; i < BULLET_COUNT; i++)
	{
		bullets[i] = new Sprite();
		if( !bullets[i]->loadImage("bullet.png", D3DCOLOR_XRGB(255, 174, 201)))
			return 0;
		bullets[i]->setMoveTimer(15);
		bullets[i]->setVisible(false);
	}

	for (int i = 0; i < MONSTER_COUNT; i++)
	{
		monsters[i] = new Sprite();
		if(!monsters[i]->loadImage("monster.png", D3DCOLOR_XRGB(255, 174, 201)))
			return 0;
		monsters[i]->setWidth(32);
		monsters[i]->setHeight(32);
		monsters[i]->setColumns(4);
		monsters[i]->setTotalFrames(8);
		monsters[i]->setCurrentFrame(0);
		monsters[i]->setMoveTimer(15);
		monsters[i]->setFrameTimer(0);
		monsters[i]->setObjectType(OBJECT_MONSTER);
		monsters[i]->setCollisionMethod(COLLISION_DIST);
		if( i > 1 ){
		    monsters[i]->setVisible(false);
			monsters[i]->setAlive(false);
		}else
		{
		    g_engine->addEntity(monsters[i], DRAW_PRIORITY_MIDDLE);
		}
	}


	monsters[0]->setCurrentFrame(4);
	monsters[0]->setPosition(0, g_engine->getScreenHeight() - monsters[0]->getHeight());

	monsters[1]->setPosition(g_engine->getScreenWidth() - monsters[1]->getWidth(), g_engine->getScreenHeight() - monsters[1]->getHeight());

	
	bull = new Sprite();
	if(!bull->loadImage("bull.png", D3DCOLOR_XRGB(255, 174, 201)))
		return 0;
	bull->setWidth(64);
	bull->setHeight(64);
	bull->setColumns(5);
	bull->setTotalFrames(11);
	bull->setCurrentFrame(0);
	bull->setMoveTimer(15);
	bull->setFrameTimer(150);
	g_engine->addEntity(bull);
	bull->setPosition(g_engine->getScreenWidth() * 2 / 3, g_engine->getScreenHeight() - bull->getHeight());
	
	highPoint = g_engine->getScreenHeight() - shootyGuy->getHeight() - 40;
	*/

    levelManager = new LevelManager();
	//create the console
	console = new Console();
	if(!console->init())
	{
		g_engine->message("Error initializing console");
		return false;
	}

	return 1;
}


void game_update(){

	if(KEY_DOWN(VK_ESCAPE)) g_engine->Close();
    
	int xVel = 0;
    float yVel = shootyGuy->getVelocity().getY();

	if(KEY_DOWN(VK_LEFT))
	{
		xVel = -moveSpeed;
		faceRight(false);
	}
	else if (KEY_DOWN(VK_RIGHT))
	{
		xVel = moveSpeed;
		faceRight(true);
	}

	if(KEY_DOWN(VK_SHIFT)) xVel *= 2;

	shootyGuy->setVelocity(xVel, yVel);
	
	if(KEY_DOWN(VK_SPACE))
	{
		jump(shootyGuy);
		standing = false;
	}

	if(KEY_DOWN(VK_B)) autoMove();

	if(KEY_DOWN(VK_CONTROL))
	{
		shoot(None);
	}

	if(shootyGuy->getY() + shootyGuy->getHeight() > g_engine->getScreenHeight()){
		shootyGuy->setVelocity(0, 0);
		shootyGuy->setY(g_engine->getScreenHeight() - shootyGuy->getHeight());
	}


	shootyGuy->move();

/*	if(shootyGuy->getY() < highPoint )
	{
		highPoint = shootyGuy->getY();
		std::stringstream s;
		s << shootyGuy->getY();
		
		g_engine->message(s.str());
	}*/

	if(timeGetTime() > (DWORD)(bullSnortTimerStart + bullSnortTimerInterval)){
		bullSnort();
	}


	updateConsole();

}

bool checkOnSurface()
{
	if (standingSurface == NULL)
	{
		return false;
	}

	bool onBounds = shootyGuy->getX() < standingSurface->getX() + standingSurface->getWidth() 
		&& shootyGuy->getX() + shootyGuy->getWidth() > standingSurface->getX();
	

	if (!onBounds)
	{
		//g_engine->message("Off surface");
		standingSurface = NULL;
	}

	return onBounds;
}

void game_end()
{
	delete shootyGuy;
	for (int i = 0; i < BULLET_COUNT; i++)
	{
		delete bullets[i];
	}
	for (int i = 0; i < MONSTER_COUNT; i++)
	{
		delete monsters[i];
	}
	delete bull;

}

void updateConsole()
{
	int lineY = 0;

	double x = shootyGuy->getX();
	double y = shootyGuy->getY();

	ostr.str("");
	ostr << "Guy's X Coordinate: " << x;

	console->print(ostr.str(),lineY++);

	ostr.str("");
	ostr << "Guy's Y Coordinate: " << y;

	console->print(ostr.str(),lineY++);
	lineY++;

	if ( standingSurface != NULL)
	{
		x = standingSurface->getX();
		y = standingSurface->getY();

		ostr.str("");
	    ostr << "Block's X Coordinate: " << x;

	    console->print(ostr.str(),lineY++);

	    ostr.str("");
	    ostr << "Block's Y Coordinate: " << y;

	    console->print(ostr.str(),lineY++);
	    lineY++;

		ostr.str("");
	    ostr << g_engine->getFrameRate_core() << " fps";
	    console->print(ostr.str(), lineY++);
	}
	else
	{
	    ostr.str("");
	    ostr << g_engine->getFrameRate_core() << " fps";
	    console->print(ostr.str(), lineY++);

		ostr.str("");
		console->print(ostr.str(), lineY++);
		lineY++;

		ostr.str("");
		console->print(ostr.str(), lineY);
	}

}

void game_render2d(){
	g_engine->ClearScene(D3DCOLOR_XRGB(0,0,0));
	if (console->isShowing())
	{
		console->draw();
	}
}

void game_render3d(){

}

void applyGravity(Sprite *sprite){
	if(sprite->getVelocity().getY() <= TERMINAL_VELOCITY && !checkOnSurface()){
		sprite->setVelocity(sprite->getVelocity().getX(), sprite->getVelocity().getY() + GRAVITY_RATE);
	}
}

void jump(Sprite *sprite){

	if(g_engine->getScreenHeight() - sprite->getY() - sprite->getHeight() < .5 || 
		sprite->getY() + sprite->getHeight() > g_engine->getScreenHeight() && sprite->getY() + sprite->getHeight() - g_engine->getScreenHeight() < .5 ||
		standingSurface != NULL && (
		standingSurface->getY() - sprite->getY() - sprite->getHeight() < .5 || 
		sprite->getY() + sprite->getHeight() > standingSurface->getY() && sprite->getY() + sprite->getHeight() - standingSurface->getY() < .5 )
		){

		sprite->setVelocity(sprite->getVelocity().getX(), -TERMINAL_VELOCITY * 2);
		standingSurface = NULL;
    }
}

bool spriteOnScreen(Sprite *sprite)
{
	return (sprite->getY() > 0 && sprite->getY() < g_engine->getScreenHeight() &&
		    sprite->getX() + sprite->getWidth() > 0 && sprite->getX() < g_engine->getScreenWidth());
}

void shoot(AutoPilotMode mode)
{
	if(timeGetTime() > (DWORD)( shootTimerStart + shootTimerInterval))
	{
		shootTimerStart = timeGetTime();		

	    //select bullet that will be fired next
	    Sprite *bullet = loadBullet();
	    bullet->setVisible(true);
		bullet->setObjectType(OBJECT_BULLET);
		g_engine->addEntity(bullet, DRAW_PRIORITY_FRONT);
	
	    if(mode == None)
	    {

	        float yVel = 0.0;
	        if(KEY_DOWN(VK_UP))
	        {
		        yVel = -moveSpeed * BULLET_SPEED_MODIFIER;
	        }
	        else if ( KEY_DOWN(VK_DOWN))
	        {
		        yVel = moveSpeed * BULLET_SPEED_MODIFIER;
	        }

	
	        //fire bullet -> set position to in front of shooty guy, set velocity to 2x walking speed
	        if( facingRight )
	        {
				bullet->setVelocity( moveSpeed * BULLET_SPEED_MODIFIER , yVel);
	            bullet->setPosition( shootyGuy->getX() + shootyGuy->getWidth() / 2 - bullet->getVelocity().getX(), shootyGuy->getY() + shootyGuy->getHeight() / 2 - bullet->getHeight() / 2);
	            bullet->setRotation( 0 );
	        } else 
	        {
				bullet->setVelocity( -moveSpeed * BULLET_SPEED_MODIFIER , yVel);
	            bullet->setPosition( shootyGuy->getX() + shootyGuy->getWidth() / 2 - bullet->getWidth() - bullet->getVelocity().getX(), shootyGuy->getY() + shootyGuy->getHeight() / 2 - bullet->getHeight() / 2);
	            bullet->setRotation( 3.14 );
	        }
	        if ( yVel > 0 && shootyGuy->getVelocity().getX() == 0.0)
	        {
		        bullet->setPosition(shootyGuy->getX() + bullet->getWidth() / 2, shootyGuy->getY() + shootyGuy->getHeight() - yVel);
		        bullet->setVelocity( 0.0, yVel);
		        bullet->setRotation( 1.57 );
	        }else if (yVel < 0 && shootyGuy->getVelocity().getX() == 0.0)
	        {
		        bullet->setPosition(shootyGuy->getX() + bullet->getWidth() / 2, shootyGuy->getY() - bullet->getHeight() - yVel);
		        bullet->setVelocity(0.0, yVel);
		        bullet->setRotation( -1.57 );
	        }
	    }

	    else if (mode == Shooting)
	    {
		    Sprite *monster = closestEnemy();

		    float xDistance = shootyGuy->xMidPointDiff(monster);
		    float yDistance = shootyGuy->yMidPointDiff(monster);

		    bool aimRight = monster->getMidPoint().getX() >= shootyGuy->getMidPoint().getX();

		    if(xDistance < yDistance)
		    {
			    //monster above | below 
			    bool aimUp = monster->getMidPoint().getY() <= shootyGuy->getMidPoint().getY();
			    float yVel = moveSpeed * BULLET_SPEED_MODIFIER;

			    float xVel = xDistance == 0 ? 0 : moveSpeed * BULLET_SPEED_MODIFIER * yDistance / xDistance;

			    if(aimUp)
			    {
				    bullet->setPosition(shootyGuy->getX() + bullet->getWidth() / 2, shootyGuy->getY() + shootyGuy->getHeight());
		            bullet->setVelocity( xVel, -yVel);
		            bullet->setRotation( -1.57 );
			    }
			    else 
			    {   
				    bullet->setPosition(shootyGuy->getX() + bullet->getWidth() / 2, shootyGuy->getY() - bullet->getHeight());
		            bullet->setVelocity(xVel, yVel);
		            bullet->setRotation( 1.57 );
			    }
		    }
		    else
		    {
			    float yVel = yDistance == 0 ? 0 : moveSpeed * BULLET_SPEED_MODIFIER * xDistance / yDistance;

		        //if enemy is to right set rotation to face right
			    if(aimRight)
			    {
		            bullet->setPosition( shootyGuy->getX() + shootyGuy->getWidth(), shootyGuy->getY() + shootyGuy->getHeight() / 2 - bullet->getHeight() / 2);
	                bullet->setVelocity( moveSpeed * BULLET_SPEED_MODIFIER , yVel);
	                bullet->setRotation( 0 );
			    }
			    else
			    {
				    bullet->setPosition( shootyGuy->getX() - bullet->getWidth(), shootyGuy->getY() + shootyGuy->getHeight() / 2 - bullet->getHeight() / 2);
	                bullet->setVelocity( -moveSpeed * BULLET_SPEED_MODIFIER , yVel);
	                bullet->setRotation( 3.14 );
			    }
		    }
	    }
	}
}

//use lastFiredBullet to determine next bullet
Sprite *loadBullet()
{
	int nextFiredBullet = lastFiredBullet + 1;

	if(lastFiredBullet == BULLET_COUNT - 1)
	{
		nextFiredBullet = 0;
	}

	lastFiredBullet = nextFiredBullet;

	return bullets[nextFiredBullet];
}

void deactivateBullets()
{
	for (int i = 0; i < BULLET_COUNT; i++)
	{
		if (bullets[i]->getVisible() && !spriteOnScreen(bullets[i]))
		{
			bullets[i]->setVisible(false);
		}
	}
}

void drawBullets()
{
	for (int i = 0; i < BULLET_COUNT; i++)
	{
		if (bullets[i]->getVisible())
		{
			bullets[i]->draw();
		}
	}
}





void monsterUpdate()
{

	for (int i = 0; i < MONSTER_COUNT; i++) 
	{
		if(monsters[i]->getVisible())
		{
			if(!monsters[i]->getAlive() && monsters[i]->getVisible())
			{
			    monsters[i]->setFrameTimer(150);
				//monsters[i]->animate();
			    
				if(monsters[i]->getCurrentFrame() == 0 || monsters[i]->getCurrentFrame() == 3)
			    {
				    monsters[i]->setVelocity(0,0);
			    }
			}
			else
			{
				monsters[i]->setFrameTimer(0);
				//is set velocity based on location of shootyGuy
				monsters[i]->move();

				if(fabs((float)(shootyGuy->getPosition().getX() - monsters[i]->getPosition().getX())) > 7.0f){

				    float xVel = shootyGuy->getPosition().getX() > monsters[i]->getPosition().getX() ? MONSTER_MOVE_SPEED : -MONSTER_MOVE_SPEED;

				    monsters[i]->setVelocity( xVel, 0.0f);
				}
			}
		}
	}
}



/* bullSnort()
   if current frame is not 0 or bull not snorting
     animate
   else
     reset timer

*/
void bullSnort()
{/*
	if(bull->getCurrentFrame() != 0 || !bullSnorting){
		bullSnorting = true;
		bull->setFrameTimer(150);
	}else{
	    bullSnortTimerStart = timeGetTime();
		bullSnorting = false;
		bull->setFrameTimer(0);
	}*/
}

Sprite *closestEnemy()
{
	Sprite *closest = NULL;
	int shortestDistance = g_engine->getScreenWidth() * 2; //can include monsters off screen, but not way far off of screen
		
	for(int i = 0; i < MONSTER_COUNT; i++)
	{	
		if(monsters[i]->getVisible() && monsters[i]->getAlive())
		{
			//closest to shooter guy, determined by midpoint of each    
			float shooterMidX = shootyGuy->getPosition().getX() + shootyGuy->getWidth() / 2;

			float monsterMidX = monsters[i]->getPosition().getX() + monsters[i]->getWidth() / 2;

			int distance = fabs(shooterMidX - monsterMidX);

			if (distance < shortestDistance)
			{
				shortestDistance = distance;
				closest = monsters[i];
			}
		}
	}
	return closest;
} //Sprite *closestEnemy()

void autoMove()
{
    Sprite * monster = closestEnemy();
	if(monster != NULL){
	    faceRight(monster->getPosition().getX() >= shootyGuy->getPosition().getX());
	    shoot(Shooting);
	}
}

void faceRight(bool right)
{
    facingRight = right;
	if(right)
	{
		shootyGuy->setCurrentFrame(0);
	}else
	{
	    shootyGuy->setCurrentFrame(1);
	}
}

void game_entityUpdate(Advanced2D::Entity* entity)
{
	int type = entity->getObjectType();
	Sprite* sprite;
	switch (type)
	{
	case OBJECT_MONSTER:
		sprite = (Sprite*)entity;
		if (sprite->getCurrentFrame() == 6 || sprite->getCurrentFrame() == 3)
		{
			sprite->setAlive(false);
			//entity->setAlive(false);
		}else
		{

		    if(fabs((float)(shootyGuy->getPosition().getX() - sprite->getPosition().getX())) > 7.0f){

				bool shootyGuyRight = shootyGuy->getPosition().getX() > sprite->getPosition().getX() ? true : false;
                float xVel = 0;

				if (shootyGuyRight)  
				{
                     xVel = MONSTER_MOVE_SPEED;         
					 int frame = sprite->getCurrentFrame();
					 if(frame == 4 || frame == 0)
					 {
					     sprite->setCurrentFrame(4);
					 }
				}
				else
				{
					xVel = -MONSTER_MOVE_SPEED;
					int frame = sprite->getCurrentFrame();
					if(frame == 4 || frame == 0)
					 {
					     sprite->setCurrentFrame(0);
					 }
				}
				    sprite->setVelocity( xVel, 0.0f);
		    }
		}
		break;
	case OBJECT_BULLET:
		sprite = (Sprite*)entity;
		//if not visible
        if(!spriteOnScreen(sprite))
		{
		    entity->setVisible(false);
		} 
		if (!entity->getVisible())
		{
			entity->setLifetime(1);
		}
		break;
	}
}

void game_entityRender(Advanced2D::Entity* entity)
{
	int type = entity->getObjectType();
	if(type == OBJECT_GUY){
	    if(  g_engine->getScreenHeight() - shootyGuy->getY() > shootyGuy->getHeight() && !checkOnSurface()){
	        applyGravity(shootyGuy);
	    }
	}
}


void game_entityCollision(Advanced2D::Entity* entity1, Advanced2D::Entity* entity2)
{
	int type1 = entity1->getObjectType();
	int type2 = entity2->getObjectType();
	Sprite* sprite1;
	Sprite* sprite2;

	if (type1 == OBJECT_GUY && type2 == OBJECT_SOLID_BLOCK)
	{
		//g_engine->message("Guy->block collision");
	}
	
    if(type1 == OBJECT_SOLID_BLOCK && (type2 == OBJECT_GUY))// || type1 == OBJECT_MONSTER))
	{
		Sprite* guySprite;
		Sprite* blockSprite;
		guySprite = (Sprite*)entity2;
		blockSprite = (Sprite*)entity1;
		
		CollisionSide side = getCollisionSide(guySprite, blockSprite);

		switch (side)
		{
		case Top:
			guySprite->setY(blockSprite->getY() - guySprite->getHeight());
			guySprite->setVelocity(0, 0);
			standingSurface = blockSprite;
			standing = true;
			break;
		case Bottom:
			/*if (backToBackCollisionTimer.stopwatch(1))
			{
				if (guySprite->getMidPoint().getX() > blockSprite->getMidPoint().getX())
				{
					guySprite->setPosition(blockSprite->getX() + blockSprite->getWidth(), guySprite->getY());
				}
				else 
				{
					guySprite->setPosition(blockSprite->getX() - guySprite->getWidth(), guySprite->getY());
				}
			}
			else
			{*/
			    guySprite->setY(blockSprite->getY() + blockSprite->getHeight());
				guySprite->setVelocity(0, 0);
			//}
			break;		
		case Left:
			guySprite->setX(blockSprite->getX() - guySprite->getWidth());
			guySprite->setVelocity(0, guySprite->getVelocity().getY());
			break;
		case Right:
			guySprite->setX(blockSprite->getX() + blockSprite->getWidth());
			guySprite->setVelocity(0, guySprite->getVelocity().getY());
			break;
		case ZeroVelocity:
			if (guySprite->getMidPoint().getX() > blockSprite->getMidPoint().getX())
			{
				guySprite->setPosition(blockSprite->getX() + blockSprite->getWidth(), guySprite->getY());
			}
			else 
			{
				guySprite->setPosition(blockSprite->getX() - guySprite->getWidth(), guySprite->getY());
			}
			//g_engine->message("Fuck, zero velocity");
			break;
		}
	} 

	/*if(type2 == OBJECT_SOLID_BLOCK && (type1 == OBJECT_GUY))// || type1 == OBJECT_MONSTER))
	{
		Sprite* guySprite;
		Sprite* blockSprite;
		guySprite = (Sprite*)entity1;
		blockSprite = (Sprite*)entity2;
		
		CollisionSide side = getCollisionSide(guySprite, blockSprite);

		switch (side)
		{
		case Top:
			guySprite->setY(blockSprite->getY() - guySprite->getHeight());
			guySprite->setVelocity(0, 0);
			standingSurface = blockSprite;
			standing = true;
			break;
		case Bottom:
			/*if (backToBackCollisionTimer.stopwatch(1))
			{
				if (guySprite->getMidPoint().getX() > blockSprite->getMidPoint().getX())
				{
					guySprite->setPosition(blockSprite->getX() + blockSprite->getWidth(), guySprite->getY());
				}
				else 
				{
					guySprite->setPosition(blockSprite->getX() - guySprite->getWidth(), guySprite->getY());
				}
			}
			else
			{* /
			    guySprite->setY(blockSprite->getY() + blockSprite->getHeight());
				guySprite->setVelocity(0, 0);
			//}
			break;		
		case Left:
			guySprite->setX(blockSprite->getX() - guySprite->getWidth());
			guySprite->setVelocity(0, guySprite->getVelocity().getY());
			break;
		case Right:
			guySprite->setX(blockSprite->getX() + blockSprite->getWidth());
			guySprite->setVelocity(0, guySprite->getVelocity().getY());
			break;
		case ZeroVelocity:
			if (guySprite->getMidPoint().getX() > blockSprite->getMidPoint().getX())
			{
				guySprite->setPosition(blockSprite->getX() + blockSprite->getWidth(), guySprite->getY());
			}
			else 
			{
				guySprite->setPosition(blockSprite->getX() - guySprite->getWidth(), guySprite->getY());
			}
			//g_engine->message("Fuck, zero velocity");
			break;
		}
	} */

	if (type1 == OBJECT_MONSTER && type2 == OBJECT_MONSTER)
	{
		sprite1 = (Sprite*) entity1;
		sprite2 = (Sprite*) entity2;
	
		if (sprite2->getX() > sprite1->getX())
		{
			sprite2->setPosition(sprite1->getX() + sprite1->getWidth(), sprite2->getY());
		}
		else
		{
			sprite1->setPosition(sprite2->getX() + sprite2->getWidth(), sprite1->getY());
		}
	}
	
	if(type2 == OBJECT_BULLET && type1 == OBJECT_MONSTER)
	{
		sprite2 = (Sprite*) entity1;
		sprite1 = (Sprite*) entity2;

		sprite1->setAlive(false);
		sprite2->setFrameTimer(150);
	}
}


Vector3 getFirstCollisionPosition(Sprite* sprite1, Sprite* sprite2)
{
	static int loopCounter = 0;
	Sprite* copied1  = sprite1;
	Sprite* copied2 = sprite2;

	Vector3 vel1 = copied1->getVelocity();
	Vector3 vel2 = copied2->getVelocity();

	Vector3 previousPosition1 = Vector3(copied1->getPosition());
	Vector3 previousPosition2 = Vector3(copied2->getPosition());

	//TODO: fix infinite loop when velocity set to zero
	while (g_engine->collisionBR(copied1, copied2))
	{
		copied1->setPosition(copied1->getPosition().getX() - vel1.getX(), 
		copied1->getPosition().getY() - vel1.getY());

		copied2->setPosition(copied2->getPosition().getX() - vel2.getX(), 
		copied2->getPosition().getY() - vel2.getY());
		loopCounter++;
		//DEBUG
		if (loopCounter > 30)
		{
		//	g_engine->message("Infinite loop");
			break;
		}
	}
	loopCounter = 0;
	copied1->setPosition(copied1->getPosition().getX() + vel1.getX(), 
		copied1->getPosition().getY() + vel1.getY());

	Vector3 returnPosition = Vector3(copied1->getPosition());

	return returnPosition;
}

CollisionSide getCollisionSide(Sprite* sprite1, Sprite* sprite2)
{   

	Vector3 firstCollisionPosition = getFirstCollisionPosition(sprite1, sprite2);	
	
	float tolerance = .05;

	Vector3 vel1 = sprite1->getVelocity();
	Vector3 vel2 = sprite2->getVelocity();

	Vector3 subtractedVelocity = Vector3(vel1.getX() - vel2.getX(), vel1.getY() - vel2.getY(), vel1.getZ() - vel2.getZ());

	bool verticalMovement = fabs(subtractedVelocity.getX()) < fabs(subtractedVelocity.getY());

	float depthX;
	float depthY;

	if (subtractedVelocity.getX() > 0 + tolerance)
	{
		if (subtractedVelocity.getY() > 0 + tolerance)
		{
			//right, downward angle, intercepted with either top or left side
			depthX = firstCollisionPosition.getX() + sprite1->getWidth() - sprite2->getX();
			depthY = firstCollisionPosition.getY() + sprite1->getHeight() - sprite2->getY();

			//if depth is greater than 1 unit, then collision was on Y axis
			float xRatio = depthX / subtractedVelocity.getX() + .001;  //possible divide by zero
			float yRatio = depthY / subtractedVelocity.getY() + .001;

			return xRatio > yRatio ? Top : Left;
		}
		else if (subtractedVelocity.getY() < 0 - tolerance)
		{
			//right, upward angle, intrecpeted with either left or top side
			depthX = firstCollisionPosition.getX() + sprite1->getWidth() - sprite2->getX();
			depthY = sprite2->getY() + sprite2->getHeight() - firstCollisionPosition.getY();

			//if depth is greater than 1 unit, then collision was on Y axis
			float xRatio = depthX / subtractedVelocity.getX() + .001;  //possible divide by zero
			float yRatio = depthY / subtractedVelocity.getY() * -1 + .001;

			return xRatio > yRatio ? Bottom : Left;

		}
		else if (subtractedVelocity.getY() > 0 - tolerance && subtractedVelocity.getY() < 0 + tolerance)
		{
			//y angle effectively zero, strikes left side
			return Left;
		}
	}

	else if (subtractedVelocity.getX() < 0 - tolerance)
	{
		if (subtractedVelocity.getY() > 0 + tolerance)
		{
			//left, downward angle, intercepted with either top or left side
			depthX = sprite2->getX() + sprite2->getWidth() - firstCollisionPosition.getX();
			depthY = firstCollisionPosition.getY() + sprite1->getHeight() - sprite2->getY();

			//if depth is greater than 1 unit, then collision was on Y axis
			float xRatio = depthX / subtractedVelocity.getX() * -1 + .001;  //possible divide by zero
			float yRatio = depthY / subtractedVelocity.getY() + .001;

			return xRatio > yRatio ? Top : Right;
		}
		else if (subtractedVelocity.getY() < 0 - tolerance)
		{
			//right, upward angle, intrecpeted with either left or top side
			depthX = sprite2->getX() + sprite2->getWidth() - firstCollisionPosition.getX();
			depthY = sprite2->getY() + sprite2->getHeight() - firstCollisionPosition.getY();

			//if depth is greater than 1 unit, then collision was on Y axis
			float xRatio = depthX / subtractedVelocity.getX() * -1 + .001;  //possible divide by zero
			float yRatio = depthY / subtractedVelocity.getY() * -1 + .001;

			return xRatio > yRatio ? Bottom : Right;

		}
		else if (subtractedVelocity.getY() > 0 - tolerance && subtractedVelocity.getY() < 0 + tolerance)
		{
			//y angle effectively zero, strikes left side
			return Right;
		}
	}

	else if (subtractedVelocity.getX() > 0 - tolerance && subtractedVelocity.getX() < 0 + tolerance)
	{
		if (subtractedVelocity.getY() < 0 - tolerance)
		{
			return Bottom;
		}
		else if (subtractedVelocity.getY() > 0 + tolerance)
		{
			return Top;
		}
		else if (subtractedVelocity.getY() > 0 - tolerance &&  subtractedVelocity.getY() < 0 + tolerance)
		{
			return ZeroVelocity;
		}
	}
	
	return ZeroVelocity;

}


void game_keyPress(int key){}
void game_keyRelease(int key){ if (key == DIK_GRAVE) { console->setShowing(!console->isShowing()); }}
void game_mouseButton(int button){}
void game_mouseMotion(int x, int y){}
void game_mouseMove(int x, int y){}
void game_mouseWheel(int wheel){}



