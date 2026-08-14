#include "DataManager.h"
#include "pch.h"

void DataManager::Init()
{
	LoadImageData("Resource/Data/ImageData.json");
	LoadMapData("Resource/Data/maps/MapData.json");
	LoadItemData("Resource/Data/Items.json");
	LoadRecipeData("Resource/Data/Recipes.json");
	LoadMonsterData("Resource/Data/Monsters.json");
}

const string& DataManager::GetImagePath(ImageKey key) const
{
	static string empty = "";
	auto it = m_imageMap.find(key);
	if (it == m_imageMap.end())
		return empty;
	return it->second.path;
}

const string& DataManager::GetImageKeyString(ImageKey key) const
{
	static string empty = "";
	auto it = m_imageMap.find(key);
	if (it == m_imageMap.end())
		return empty;
	return it->second.keyString;
}

const vector<string>& DataManager::GetMap(const string& mapName) const
{
	static vector<string> empty;
	auto it = m_maps.find(mapName);
	if (it == m_maps.end())
		return empty;
	return it->second;
}

const ItemData* DataManager::FindItem(const string& id) const
{
	for (const ItemData& item : m_items)
	{
		if (item.id == id) return &item;
	}
	return nullptr;
}

void DataManager::LoadImageData(const string& filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open()) return;

	json j;
	file >> j;

	for (auto& item : j)
	{
		ImageData data;
		data.keyString = item["key"].get<string>();
		data.path = item["path"].get<string>();
		data.key = StringToImageKey(data.keyString);

		if (data.key == ImageKey::Count)
		{
			continue;
		}

		m_images.push_back(data);
		m_imageMap[data.key] = data;
	}
}

void DataManager::LoadMapData(const string& filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		return;
	}

	json j;
	file >> j;

	string mapName = j["name"].get<string>();
	vector<string> tiles;

	for (auto& row : j["tiles"])
	{
		tiles.push_back(row.get<string>());
	}

	m_maps[mapName] = tiles;
}

ImageKey DataManager::StringToImageKey(const string& str) const
{
	if (str == "Player_Idle")  return ImageKey::Player_Idle;
	if (str == "Player_Walk")  return ImageKey::Player_Walk;
	if (str == "Player_Attack")  return ImageKey::Player_Attack;
	if (str == "Player_Die")  return ImageKey::Player_Die;
	if (str == "Player_Gun_Idle")  return ImageKey::Player_Gun_Idle;
	if (str == "Player_Gun_Walk") return ImageKey::Player_Gun_Walk;
	if (str == "Player_Gun_Attack") return ImageKey::Player_Gun_Attack;
	if (str == "Player_Sword_Idle")  return ImageKey::Player_Sword_Idle;
	if (str == "Player_Sword_Walk") return ImageKey::Player_Sword_Walk;
	if (str == "Player_Sword_Attack") return ImageKey::Player_Sword_Attack;
	if (str == "Player_Shield_Idle") return ImageKey::Player_Shield_Idle;
	if (str == "Player_Shield_Walk") return ImageKey::Player_Shield_Walk;
	if (str == "Wall_N")  return ImageKey::Wall_N;
	if (str == "Wall_S")  return ImageKey::Wall_S;
	if (str == "Wall_E")  return ImageKey::Wall_E;
	if (str == "Wall_W")  return ImageKey::Wall_W;
	if (str == "Tile_W")  return ImageKey::Tile_W;
	if (str == "CampFire")  return ImageKey::CampFire;
	if (str == "WorkTable")  return ImageKey::WorkTable;
	if (str == "Item_Wood")  return ImageKey::Item_Wood;
	if (str == "Item_Stone")  return ImageKey::Item_Stone;
	if (str == "Item_Sword")  return ImageKey::Item_Sword;
	if (str == "Item_Gun")  return ImageKey::Item_Gun;
	if (str == "Item_Shield")  return ImageKey::Item_Shield;
	if (str == "Item_Apple")  return ImageKey::Item_Apple;
	if (str == "Item_Banana")  return ImageKey::Item_Banana;
	if (str == "Item_Grape")  return ImageKey::Item_Grape;
	if (str == "Item_Mushroom")  return ImageKey::Item_Mushroom;
	if (str == "Item_Mushroom_Baked")  return ImageKey::Item_Mushroom_Baked;
	if (str == "Bullet")  return ImageKey::Bullet;
	if (str == "Monster")  return ImageKey::Monster;
	if (str == "Monster2")  return ImageKey::Monster2;
	if (str == "Monster3")  return ImageKey::Monster3;
	if (str == "SideBarUI")  return ImageKey::SideBarUI;
	if (str == "Fire")  return ImageKey::Fire;
	if (str == "GrassRoad")  return ImageKey::GrassRoad;
	if (str == "WaterTile")  return ImageKey::WaterTile;
	if (str == "Grass")  return ImageKey::Grass;
	if (str == "CrossHair")  return ImageKey::CrossHair;
	if (str == "bubbleText")  return ImageKey::BubbleText;
	if (str == "Text")  return ImageKey::Text;
	if (str == "Night")  return ImageKey::NightUI;
	if (str == "Day")  return ImageKey::DayUI;

	return ImageKey::Count;
}

void DataManager::LoadItemData(const string& filePath)
{
	ifstream file(filePath);
	if (!file.is_open()) return;

	json j;
	file >> j;

	for (auto& item : j)
	{
		ItemData data;
		data.id = item["id"].get<string>();
		data.name = item["name"].get<string>();
		data.image = item["image"].get<string>();
		data.type = item["type"].get<string>();
		data.weaponType = item.value("weaponType", "");
		data.weaponSpriteKey = item.value("weaponSpriteKey", "");
		data.defenseValue = item.value("defenseValue", 0);
		data.damage = item.value("damage", 0);
		data.range = item.value("range", 0.f);
		data.speed = item.value("speed", 0.f);
		data.healAmount = item.value("healAmount", 0);
		m_items.push_back(data);
	}
}

void DataManager::LoadRecipeData(const string& filePath)
{
	ifstream file(filePath);
	if (!file.is_open()) return;

	json j;
	file >> j;

	for (auto& recipe : j)
	{
		RecipeData data;
		data.id = recipe["id"].get<string>();
		data.resultId = recipe["result"].get<string>();
		data.resultCount = recipe.value("resultCount", 1);
		data.station = recipe.value("station", "");
		data.isWeaponUpgrade = recipe.value("isWeaponUpgrade", false);
		data.successRate = recipe.value("successRate", 1.f);
		for (auto& ing : recipe["ingredients"])
		{
			Ingredient ingredient;
			ingredient.ingredientId = ing["id"].get<string>();
			ingredient.count = ing["count"].get<int>();
			data.ingredients.push_back(ingredient);
		}

		m_recipes.push_back(data);
	}
}

void DataManager::LoadMonsterData(const string& filePath)
{
	ifstream file(filePath);
	if (!file.is_open()) return;

	json j;
	file >> j;

	for (auto& item : j)
	{
		MonsterData data;
		data.id = item["id"].get<string>();
		data.name = item.value("name", "");
		data.image = item["image"].get<string>();
		data.health = item.value("health", 30);
		data.moveSpeed = item.value("moveSpeed", 1.5f);
		data.contactDamage = item.value("contactDamage", 5);
		data.cellWidth = item.value("cellWidth", 46);
		data.cellHeight = item.value("cellHeight", 33);
		data.animColumn = item.value("animColumn", 6);
		data.colliderRadius = item.value("colliderRadius", 0.4f);
		data.scale = item.value("scale", 1.f);
		m_monsters.push_back(data);
	}
}

vector<RecipeData> DataManager::GetRecipesByStation(const string& station) const
{
	vector<RecipeData> result;
	for (const RecipeData& recipe : m_recipes)
	{
		if (recipe.station == station)
			result.push_back(recipe);
	}
	return result;
}

const MonsterData* DataManager::FindMonster(const string& id) const
{
	for (const MonsterData& monster : m_monsters)
	{
		if (monster.id == id) return &monster;
	}
	return nullptr;
}
