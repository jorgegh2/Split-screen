#include "Log.h"
#include "App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include <math.h>
#include "Camera.h"

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.assign("map");
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.assign(config.child("folder").child_value());

	return ret;
}

bool j1Map::PostUpdate()
{
	if(map_loaded == false)
		return false; 

	
	for (std::vector<Camera*>::iterator item_cam = App->render->cameras.begin(); item_cam != App->render->cameras.end(); ++item_cam)
	{
		SDL_RenderSetClipRect(App->render->renderer, &(*item_cam)->screen_section);

		for (std::list<MapLayer*>::iterator item = data.layers.begin(); item != data.layers.end(); ++item)
		{
			MapLayer* layer = *item;

			if (layer->properties.Get("Nodraw") != 0)
				continue;

			for (int y = 0; y < data.height; ++y)
			{
				for (int x = 0; x < data.width; ++x)
				{
					int tile_id = layer->Get(x, y);
					if (tile_id > 0)
					{
						
						TileSet* tileset = GetTilesetFromTileId(tile_id);

						SDL_Rect r = tileset->GetTileRect(tile_id);
						iPoint pos = MapToScreenI(x, y);

						if (App->render->IsOnCamera(pos.x, pos.y, data.tile_width, data.tile_height+32, (*item_cam)))
						{
							App->render->Blit(tileset->texture, pos.x, pos.y, (*item_cam), &r);
						}
					}
				}
			}
		}
		SDL_RenderSetClipRect(App->render->renderer, nullptr);

	}

	return true;
}

TileSet* j1Map::GetTilesetFromTileId(int id) const
{
	std::list<TileSet*>::const_iterator item = data.tilesets.begin();
	TileSet* set = *item;

	while (item != data.tilesets.end())
	{
		if (id < (*item)->firstgid)
		{
			set = *prev(item);
			break;
		}
		set = *item;
		++item;
	}

	return set;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	return rect;
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	std::list<TileSet*>::iterator item;
	item = data.tilesets.begin();

	while (item != data.tilesets.end())
	{
		App->tex->UnLoad((*item)->texture);
		RELEASE(*item);
		++item;
	}
	data.tilesets.clear();

	// Remove all layers
	std::list<MapLayer*>::iterator item2;
	item2 = data.layers.begin();

	while (item2 != data.layers.end())
	{
		RELEASE(*item2);
		++item2;
	}
	data.layers.clear();


	map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;

	std::string tmp = folder.data();
	tmp += file_name;
	pugi::xml_parse_result result = map_file.load_file(tmp.data());

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if (ret == true)
	{
		ret = LoadMap();
	}ret = LoadMap();
	

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for (tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if (ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if (ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.push_back(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for (layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);

		if (ret == true)
			data.layers.push_back(lay);
	}

	// Load objects info --------------------------------------------
	pugi::xml_node objectGroup;
	pugi::xml_node object;

	//Load Map Properties -------------------------------------------------------


	if (ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		std::list<TileSet*>::iterator item = data.tilesets.begin();
		while (item != data.tilesets.end())
		{
			TileSet* s = *item;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.data(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			++item;
		}

		std::list<MapLayer*>::iterator item_layer = data.layers.begin();
		while (item_layer != data.layers.end())
		{
			MapLayer* l = *item_layer;
			LOG("Layer ----");
			LOG("name: %s", l->name.data());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			++item_layer;
		}
	}

	map_loaded = ret;

	return ret;
}




// Load map general properties
bool j1Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if (map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		std::string bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if (bg_color.size() > 0)
		{
			std::string red, green, blue;
			red = bg_color.substr(1, 2);
			green = bg_color.substr(3, 4);
			blue = bg_color.substr(5, 6);

			int v = 0;

			sscanf_s(red.data(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.data(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.data(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.b = v;
		}

		std::string orientation(map.attribute("orientation").as_string());

		if (orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if (orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if (orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.assign(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if (offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if (image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.data(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if (set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if (set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

bool j1Map::LoadLayer(pugi::xml_node& node, MapLayer* layer) {

	bool ret = true;
	layer->name.assign(node.attribute("name").as_string());
	layer->width = node.attribute("width").as_int();
	layer->height = node.attribute("height").as_int();
	LoadProperties(node, layer->properties);
	pugi::xml_node data = node.child("data");
	layer->data = new uint[layer->width*layer->height];
	memset(layer->data, 0, sizeof(uint)*layer->height*layer->width);

	int i = 0;
	pugi::xml_node tile = node.child("data");

	for (tile = tile.child("tile"); tile; tile = tile.next_sibling("tile")) {
		layer->data[i] = tile.attribute("gid").as_int(0);
		i++;
	}

	return ret;
}

bool j1Map::LoadProperties(pugi::xml_node & node, Properties & properties)
{
	bool ret = false;

	pugi::xml_node data = node.child("properties");

	if (data != NULL)
	{
		pugi::xml_node prop;

		for (prop = data.child("property"); prop; prop = prop.next_sibling("property"))
		{
			Properties::Property* p = new Properties::Property();

			p->name = prop.attribute("name").as_string();
			p->value = prop.attribute("value").as_int();

			properties.list.push_back(p);
		}
	}

	return ret;
}

int Properties::Get(const char * value, int default_value) const
{
	std::list<Property*>::const_iterator item = list.cbegin();

	while (item != list.cend())
	{
		if ((*item)->name == value)
			return (*item)->value;
		++item;
	}

	return default_value;
}

iPoint j1Map::MapToScreenI(int column, int row) const
{

	iPoint screen_pos(0, 0);
	switch (data.type) {
	case MapTypes::MAPTYPE_ORTHOGONAL:
		screen_pos.x = column * data.tile_width;
		screen_pos.y = row * data.tile_height;
		break;
	case MapTypes::MAPTYPE_ISOMETRIC:
		screen_pos.x = (column - row) * data.tile_width * 0.5f;
		screen_pos.y = (column + row) * data.tile_height * 0.5f;
		break;
	default:
		LOG("ERROR: Map type not identified.");
		break;
	}

	return screen_pos;
}

fPoint j1Map::MapToScreenF(const fPoint & map_pos)
{
	fPoint screen_pos(0.0F, 0.0F);
	switch (data.type) {
	case MapTypes::MAPTYPE_ORTHOGONAL:
		screen_pos.x = map_pos.x * data.tile_width;
		screen_pos.y = map_pos.y * data.tile_height;
		break;
	case MapTypes::MAPTYPE_ISOMETRIC:
		screen_pos.x = (map_pos.x - map_pos.y) * (data.tile_width * 0.5f);
		screen_pos.y = (map_pos.x + map_pos.y) * (data.tile_height * 0.5f);
		break;
	default:
		LOG("ERROR: Map type not identified.");
		break;
	}

	return screen_pos;
}

iPoint j1Map::ScreenToMapI(int x, int y) const
{
	iPoint ret(0, 0);

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{

		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
		ret.x = int((x / half_width + y / half_height) * 0.5f);
		ret.y = int((y / half_height - (x / half_width)) * 0.5f);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}
	return ret;
}

fPoint j1Map::ScreenToMapF(float x, float y)
{
	fPoint ret(0, 0);

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{

		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
		ret.x = ((x / half_width) + y / half_height) * 0.5f;
		ret.y = ((y / half_height) - x / half_width) * 0.5f;
		return ret;
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}
	return ret;
}