This file is a code snippet of a function I added to the engine used in Jonathan Harbour's book
titled "Advanced 2D Game Development".

The quadtree implementation that I used as reference can be found at
http://gamedevelopment.tutsplus.com/tutorials/quick-tip-use-quadtrees-to-detect-likely-collisions-in-2d-space--gamedev-374


	//This method uses the quadtree methods to check for possible collisions
	void Engine::TestForCollisions()
	{
		
		quadtree->Clear();
		std::list<Entity*>::iterator entities;
        std::vector<Entity*>* collidables = new std::vector<Entity*>();
		entities = p_entities.begin();
		while (entities != p_entities.end())
		{
			Entity* entity = *entities;
			if (entity->getAlive() && entity->getVisible())
			{
				collidables->push_back(entity);
				quadtree->Insert(*entities);
			}
			entities++;
		}

		if(quadtree->highestObjectCount > 10 && false)
		{
		    std::ostringstream ostr;
		    ostr << quadtree->highestObjectCount;
		    g_engine->message(ostr.str());
		}
		
		std::vector<Entity*>* checks = new std::vector<Entity*>();		
		
		//iterator is slower than using collidables->at(i)
		for(int i = 0; i < collidables->size(); i++)
		{
			Entity* entity1 = collidables->at(i);
			checks->clear();
			checks = quadtree->retrieve(checks, entity1);
			Sprite* sprite1 = (Sprite*)entity1;
			for (int j = 0; j < checks->size(); j++)
			{
				Sprite* sprite2 = (Sprite*)checks->at(j);
				if(collision(sprite1, sprite2))
				{
					//notify game of collision
					game_entityCollision(sprite1, sprite2);
                }
			}
		} 

		delete collidables;
		delete checks;
	}
		

