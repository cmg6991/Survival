#include "DataManager.h"
#include "pch.h"

void DataManager::Init()
{
	LoadImageData("Resource/Data/ImageData.json");
	LoadMapData("Resource/Data/maps/MapData.json");
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

	return ImageKey::Count;
}
