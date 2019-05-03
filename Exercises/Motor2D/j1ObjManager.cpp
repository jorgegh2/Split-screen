
#include <string>
#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"

#include "j1ObjManager.h"
#include "App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "Object.h"
#include "j1Window.h"
#include "j1Scene.h"

#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"

#include "Obj_Player.h"

#include "j1Map.h"
#include "Camera.h"


j1ObjManager::j1ObjManager()
{
	name.assign("object_manager");
}

// Destructor
j1ObjManager::~j1ObjManager()
{

}

bool j1ObjManager::PreUpdate()
{
	std::list<Object*>::iterator iterator;

	for (iterator = objects.begin(); iterator != objects.end(); iterator++)
	{
		if ((*iterator) != nullptr)
		{
			(*iterator)->PreUpdate();
		}
	}
	return true;
}

bool j1ObjManager::Update(float dt)
{

	for (std::list<Object*>::iterator iterator = objects.begin(); iterator != objects.end(); ++iterator)
	{
		if ((*iterator) != nullptr)
		{
			(*iterator)->Update(dt);
		}
	}
	
	return true;
}

bool j1ObjManager::PostUpdate()
{
	for (std::vector<Camera*>::iterator item_cam = App->render->cameras.begin(); item_cam != App->render->cameras.end(); ++item_cam)
	{
		SDL_RenderSetClipRect(App->render->renderer, &(*item_cam)->screen_section);
		
		for (std::list<Object*>::iterator item = objects.begin(); item != objects.end(); ++item)
		{
			(*item)->Draw(*item_cam);
		}
    }
	SDL_RenderSetClipRect(App->render->renderer, nullptr);
   
	return true;
}

// Called before quitting
bool j1ObjManager::CleanUp()
{
	DeleteObjects();

	return true;
}

Object* j1ObjManager::CreateObject(ObjectType type, fPoint pos)
{
	Object* ret = nullptr;
	switch (type)
	{
	case ObjectType::PLAYER:
		ret = new Obj_Player(pos);
		ret->type = ObjectType::PLAYER;
		break;
	}
	  
	if (ret != nullptr)
	{
		ret->Start();
		objects.push_back(ret);
	}
  
	return ret;
}


void j1ObjManager::DeleteObjects()
{
	for (std::list<Object*>::iterator iterator = objects.begin(); iterator != objects.end(); ++iterator)
	{
		if ((*iterator) != nullptr)
		{
			(*iterator)->CleanUp();
			delete (*iterator);
			(*iterator) = nullptr;
		}
	}

	objects.clear();
}

