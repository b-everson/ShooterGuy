#include <vector>
#pragma once

namespace Advanced2D 
{

#define OBJECT_GUY 100
#define OBJECT_BULLET 200
#define OBJECT_MONSTER 300
#define OBJECT_SOLID_BLOCK 400
#define SPRITE_TYPES_RANGE 99


	class LevelManager
	{
		
	
	private:
		
		bool guyLoaded;
		std::vector<std::vector<int>*>* tiles;
		int currentX;
		int currentY;
		int xIncrement;
		int yDecrement;
		int maxBaseX;
		void addColumn(std::vector<int>* nextVector);
		std::vector<int>* createColumn(std::string nextLine, unsigned int size = VALUES_LENGTH);
		void loadMapFile();
		void loadMapFile(std::string file);
		static const std::string DEFAULT_FILE_NAME;// = "map.txt";
		std::string fileName;
		bool instantiateColumnEntities(std::vector<int>* nextVector);
		bool instantiateEntity(int value);
		void removeColumn(int index);
		void init();

	public:
		void scrollX(int x);
		int getMaxBaseX() { return maxBaseX; }
		LevelManager();
		LevelManager(std::string file);
		~LevelManager();
		std::vector<int>* getNextTiles();
		static const int VALUES_LENGTH = 19;
		static const int DEFAULT_BLOCK_HEIGHT = 32;
		static const int DEFAULT_BLOCK_WIDTH = 32;
		static const int DEFAULT_SCREEN_HEIGHT = 600;
	};//class
};//namespace