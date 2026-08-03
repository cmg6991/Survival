#include "DataManager.h"
#include "pch.h"

void DataManager::Init()
{
	LoadImageData("Resource/Data/ImageData.json");
	LoadMapData("Resource/Data/maps/MapData.json");
	LoadItemData("Resource/Data/Items.json");
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
	if (str == "Player")  return ImageKey::Player;
	if (str == "Wall_N")  return ImageKey::Wall_N;
	if (str == "Wall_S")  return ImageKey::Wall_S;
	if (str == "Wall_E")  return ImageKey::Wall_E;
	if (str == "Wall_W")  return ImageKey::Wall_W;
	if (str == "Tile_W")  return ImageKey::Tile_W;
	if (str == "CampFire")  return ImageKey::CampFire;
	if (str == "WorkTable")  return ImageKey::WorkTable;
	if (str == "Item_Wood")  return ImageKey::Item_Wood;

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
