/*This file is code I added to the engine used in Jonathan Harbour's book
titled "Advanced 2D Game Development".

The quadtree implementation that I used as reference can be found at
http://gamedevelopment.tutsplus.com/tutorials/quick-tip-use-quadtrees-to-detect-likely-collisions-in-2d-space--gamedev-374

*/

#include "Advanced2D.h"

namespace Advanced2D
{  
    Quadtree::Quadtree(int pLevel, Rect* pBounds)  
    {
		this->highestObjectCount = 0;
        level = pLevel;
        objects = new std::vector<Entity*>();
        bounds = pBounds;
		nodes[0] = NULL;
		nodes[1] = NULL;
		nodes[2] = NULL;
		nodes[3] = NULL;
    }

    
    
    Quadtree::~Quadtree()
    {
        if(nodes[0] != NULL)
        {  
            for(int i = 0; i < NODE_COUNT; i++)
			{
				delete nodes[i];
			}
        }
        
        delete objects;
        delete bounds;  
    }

    

    void Quadtree::split()
    {
        int subWidth = (int)(bounds->getWidth() / 2);
        int subHeight = (int)(bounds->getHeight() / 2);
        int x = (int)bounds->left;
        int y = (int)bounds->top;     

        nodes[0] = new Quadtree(level + 1, new Rect(x + subWidth, y, x + subWidth + subWidth, y + subHeight));
        nodes[1] = new Quadtree(level + 1, new Rect(x, y, x + subWidth, y + subHeight));
        nodes[2] = new Quadtree(level + 1, new Rect(x, y + subHeight, x + subWidth, y + subHeight + subHeight));
        nodes[3] = new Quadtree(level + 1, new Rect(x + subWidth, y + subHeight, x + subWidth + subWidth, y + subHeight + subHeight));
    }

    

    void Quadtree::Clear()
    { 
        objects->clear();
        
        for(int i = 0; i < NODE_COUNT; i++)
        {    
            if(nodes[i] != NULL)
            {    
				delete nodes[i];
                nodes[i] = NULL;
            }
        }

		this->highestObjectCount = 0;
    }

    

    int Quadtree::getIndex(Entity* entity)
    {
        int index = -1;
        double verticalMidpoint = bounds->left + bounds->getWidth() / 2;
        double horizontalMidpoint = bounds->top + bounds->getHeight() / 2;
        Sprite* sprite = (Sprite*)entity;
        

        //object can completely fit within top quadrants 
        boolean topQuadrant = sprite->getY() + sprite->getHeight() < horizontalMidpoint;
        
        boolean bottomQuadrant = sprite->getY() > horizontalMidpoint;        

        //object can completely fit within left quadrants
        if(sprite->getX() + sprite->getWidth() < verticalMidpoint)
        {
            if (topQuadrant)
            {
                index = 1;
            }
            else if (bottomQuadrant)
            {
                index = 2;
            }
        }
        else if (sprite->getX() > verticalMidpoint)
        {
            if (topQuadrant)
            {
                index = 0;
            }
            else if (bottomQuadrant)
            {
                index = 3;
            }
        }        
        return index;
    }

    

    void Quadtree::Insert(Entity* entity)    
    {
        if(nodes[0] != NULL)
        {
            int index = getIndex(entity);
            if (index != -1)
            {
                nodes[index]->Insert(entity);
				if(nodes[index]->highestObjectCount > this->highestObjectCount)
					{
						this->highestObjectCount = nodes[index]->highestObjectCount;
					}
				return;
            }
        }

        objects->push_back(entity);


        if(objects->size() > MAX_OBJECTS && level < MAX_LEVELS)
        {
            if(nodes[0] == NULL)
            {
                split();
            }

            int i = 0;
            while (i < objects->size())
            {
                Entity* entity = objects->at(i);
				this->highestObjectCount = this->objects->size();
                int index = getIndex(entity);
                if (index != -1)
                {
                    nodes[index]->Insert(entity);
					objects->erase(objects->begin() + i);
					
					if(nodes[index]->highestObjectCount > this->highestObjectCount)
					{
						this->highestObjectCount = nodes[index]->highestObjectCount;
					}
                }
				else
				{
					i++;
				}
            }
        }         
    }//insert(Entity* entity)

    std::vector<Entity*>* Quadtree::retrieve(std::vector<Entity*>* returnObjects, Entity* entity)
    {
        int index = getIndex(entity);
		//entity is found in leaf node
        if (index != -1 && nodes[0] != NULL)
        {
            nodes[index]->retrieve(returnObjects, entity);
        }

		if(index == -1 && nodes[0] != NULL)
		{
			//check any node where the sprite would be within borders
			for (int i = 0; i < NODE_COUNT; i++)
			{
				Quadtree* thisNode = nodes[i];
				if (thisNode->ContainsSprite((Sprite*)entity))
				{
					nodes[i]->retrieve(returnObjects, entity);
				}

			}
		}

		//all objects of parent nodes added
        for(int i = 0; i < objects->size(); i ++)
        {   
			if(entity != objects->at(i))
			{
                returnObjects->push_back(objects->at(i));
			}
        }

		
        return returnObjects;
    }

	bool Quadtree::ContainsSprite(Sprite *sprite1) 
		{
			bool ret = false;

			Rect *ra = new Rect(
				sprite1->getX(),
				sprite1->getY(),
				sprite1->getX() + sprite1->getWidth() * sprite1->getScale(), 
				sprite1->getY() + sprite1->getHeight() * sprite1->getScale() );		

			Rect *rb = bounds;

			//are any of sprite b's corners intersecting sprite a?
			if (ra->isInside( rb->getLeft(), rb->getTop() ) ||
				ra->isInside( rb->getRight(), rb->getTop() ) ||
				ra->isInside( rb->getLeft(), rb->getBottom() ) ||
				ra->isInside( rb->getRight(), rb->getBottom() )) 
            {
			    ret = true;
			}

			else if(rb->isInside(ra->getLeft(), ra->getTop())||
				rb->isInside(ra->getRight(), ra->getTop()) ||
				rb->isInside( ra->getLeft(), ra->getBottom() ) ||
				rb->isInside( ra->getRight(), ra->getBottom() )) 
			{
				ret = true;
			}


            delete ra;
			return ret;
		}
}
