#include "LevelManager.h"
#include <iostream>
#include <fstream>
#include <string>
#include "..\..\Engine\Advanced2D.h"

namespace Advanced2D{

	const std::string LevelManager::DEFAULT_FILE_NAME = "map.txt";

	LevelManager::LevelManager(std::string file)
	{
		fileName = file;
        init();
	}

	LevelManager::LevelManager()
	{
		fileName = DEFAULT_FILE_NAME;
		init();
	}

	void LevelManager::init()
	{
		guyLoaded = false;
		tiles = new std::vector<std::vector<int>*>();
		
		yDecrement = DEFAULT_BLOCK_HEIGHT;
		xIncrement = DEFAULT_BLOCK_WIDTH;

		currentX = 0;
		currentY = DEFAULT_SCREEN_HEIGHT;
		this->maxBaseX = 0;

		this->loadMapFile();

		std::vector<std::vector<int>*>::iterator createIter = this->tiles->begin();
		while (createIter != tiles->end())
		{
			this->instantiateColumnEntities(*createIter);
			createIter++;
		}
	}

	void LevelManager::loadMapFile()
	{
		loadMapFile(fileName);
	}

	void LevelManager::loadMapFile(std::string file)
	{
		//try to open file
		std::ifstream myFile(file);
		std::string nextLine;

		if (myFile.is_open())
		{
			while (getline(myFile, nextLine))
			{
				tiles->push_back(createColumn(nextLine));
			}
		}
		else
		{
			g_engine->message("Error opening map file");
		}
		
	}

	std::vector<int>* LevelManager::createColumn(std::string nextLine, unsigned int size)
	{
		std::vector<int>* nextColumn = new std::vector<int>();

		//csv line, last numeric value doesn't have comma after
		while (nextColumn->size() < size) 
		{
			int commaPosition = nextLine.find(',');
			std::string stringValue;

			if (commaPosition != std::string::npos)
			{
				stringValue = nextLine.substr(0, commaPosition);
			    nextLine = nextLine.substr(commaPosition + 1, nextLine.length() - 1 - commaPosition);
			}
			else
			{
				stringValue = nextLine;
			}
					
			int nextValue = std::stoi(stringValue);
			
			nextColumn->push_back(nextValue);
		}

		this->maxBaseX += xIncrement;
		return nextColumn;
	}

	void LevelManager::removeColumn(int index)
	{
		tiles->erase(tiles->begin() + index);
	}

	/* instantiateColumnEntities(std::vector<int>* nextVector
	   for each value in vector
	       get entity type to be created by given value
		   if entity is player
		     set player loaded to true
		   else if entity is monster
		     add monster to middle of list
		   else entity is solid block
		     add entity to end of list
		   add entity to entity manager (location matters, those at beggining rendered first)
		   
	*/
	bool LevelManager::instantiateColumnEntities(std::vector<int>* nextVector)
	{
		bool loaded = true;
		std::vector<int>::iterator values;
		values = nextVector->begin();

		while (values != nextVector->end())
		{
			currentY -= yDecrement;
			int nextTile = (*values);
			instantiateEntity(nextTile);
			values++;
		}

		currentX += xIncrement;
		currentY = DEFAULT_SCREEN_HEIGHT;
		return loaded;
	}

    bool LevelManager::instantiateEntity(int value)
	{
		bool loaded = false;
		Sprite* sprite = NULL;
		
		if (value < OBJECT_GUY)
		{
			loaded = true;
		}
		else if (value >= OBJECT_GUY && value < OBJECT_BULLET)
		{
			if (!guyLoaded)
			{
				//load guy entity
				sprite = new Sprite();
				if (sprite->loadImage("Shooter guy.png", D3DCOLOR_XRGB(255,174,201)))
				{
					sprite->setColumns(2);
				    sprite->setTotalFrames(2);
				    sprite->setHeight(32);
				    sprite->setWidth(32);
				    sprite->setCurrentFrame(0);
				    sprite->setMoveTimer(15);
				    sprite->setObjectType(OBJECT_GUY);
				    g_engine->addEntity(sprite, DRAW_PRIORITY_FRONT);
				}

			    guyLoaded = true;
			}
			else
			{
				loaded = false;
			}
		}
		else if (value >= OBJECT_MONSTER && value < OBJECT_SOLID_BLOCK)
		{
			sprite = new Sprite();
			if (sprite->loadImage("monster.png", D3DCOLOR_XRGB(255, 174, 201)))
			{
				sprite->setWidth(32);
				sprite->setHeight(32);
				sprite->setColumns(4);
				sprite->setTotalFrames(8);
				sprite->setCurrentFrame(0);
				sprite->setMoveTimer(15);
				sprite->setFrameTimer(0);
				sprite->setObjectType(OBJECT_MONSTER);
				sprite->setCollisionMethod(COLLISION_DIST);
				g_engine->addEntity(sprite, DRAW_PRIORITY_FRONT);
			}
		}
		else if (value >= OBJECT_SOLID_BLOCK && value <= OBJECT_SOLID_BLOCK + SPRITE_TYPES_RANGE)
		{
			sprite = new Sprite();
			if (sprite->loadImage("DirtMGrass32.png", D3DCOLOR_XRGB(255, 174, 201)))
			{
				sprite->setWidth(32);
		        sprite->setHeight(32);
		        sprite->setColumns(1);
		        sprite->setTotalFrames(1);
		        sprite->setCurrentFrame(0);
		        sprite->setObjectType(OBJECT_SOLID_BLOCK);
				sprite->setAlive(true);
				sprite->setVisible(true);
				g_engine->addEntity(sprite, DRAW_PRIORITY_BACK);
			}
		}
		else
		{
			g_engine->message("Error loading map, invalid data");
		}

		if (sprite != NULL)
		{
			//set position of sprite based on current x and y block values
			int spriteX = this->currentX;
			int spriteY = this->currentY - yDecrement + sprite->getHeight();

			sprite->setPosition(spriteX, spriteY);
			loaded = true;
		}

		return loaded;
	}

	void LevelManager::scrollX(int x)
	{
		this->currentX -= x;
		this->maxBaseX -= x;
	}
}