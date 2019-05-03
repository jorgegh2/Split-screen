#ifndef __j1OBJMANAGER_H__
#define __j1OBJMANAGER_H__

#include <list>
#include <vector>

#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"

#include "j1Module.h"
#include "Point.h"

enum class ObjectType
{
	NO_TYPE,
	PLAYER
};

class Object;
class Camera;
class Obj_Player;

class j1ObjManager : public j1Module
{
public:

	j1ObjManager();
	~j1ObjManager();

	bool PreUpdate() override;

	bool Update(float dt) override;

	bool PostUpdate() override;

	bool CleanUp() override;

	Object* CreateObject(ObjectType type, fPoint map_pos);

	void DeleteObjects();


private:
	std::list<Object*> objects;


};

#endif

