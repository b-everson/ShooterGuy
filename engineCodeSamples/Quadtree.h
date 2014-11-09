/*This file is code I added to the engine used in Jonathan Harbour's book
titled "Advanced 2D Game Development".

The quadtree implementation that I used as reference can be found at
http://gamedevelopment.tutsplus.com/tutorials/quick-tip-use-quadtrees-to-detect-likely-collisions-in-2d-space--gamedev-374

*/

#include "Advanced2D.h"
#pragma once

namespace Advanced2D
{
    class Quadtree
    {
    private:        
        static const int MAX_OBJECTS = 3;
        static const int MAX_LEVELS = 6;
		static const int NODE_COUNT = 4;
        int level;
        std::vector<Entity*>* objects;
        Rect* bounds;
        Quadtree* nodes[NODE_COUNT];
        void split();
        int getIndex(Entity* entity);
    public:
        Quadtree(int pLevel, Rect* pBounds);
        ~Quadtree();
        void Clear();
        void Insert(Entity* entity);
        std::vector<Entity*>* retrieve(std::vector<Entity*>* returnObjects, Entity* entity);
		bool ContainsSprite(Sprite* sprite1);
                int highestObjectCount;
    };

};
