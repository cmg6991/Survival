#pragma once
#include "pch.h"
#include "Singleton.h"
#include "CraftData.h"

enum class ImageKey
{
	Player,
	Wall_N,
	Wall_S,
	Wall_E,
	Wall_W,
	Tile_W,
	CampFire,
	WorkTable,
	Item_Wood,
	Count
};

struct ImageData
{
	ImageKey key;
	string keyString; // ResourceManager::AddImage에 넘길 문자열 키
	string path;
};


class DataManager : public Singleton<DataManager>
{
public:
	void Init();

	const vector<ImageData>& GetImageList() const { return m_images; }
	const string& GetImagePath(ImageKey key) const;
	const string& GetImageKeyString(ImageKey key) const;

	const vector<string>& GetMap(const string& mapName) const;

	const vector<ItemData>& GetItemList() const { return m_items; }
	const vector<RecipeData>& GetRecipeList() const { return m_recipes; }
	const ItemData* FindItem(const string& id) const;

private:
	void LoadImageData(const string& filePath);
	void LoadMapData(const string& filePath);

	ImageKey StringToImageKey(const string& str) const;

	void LoadItemData(const string& filePath);
	void LoadRecipeData(const string& filePath);

private:
	vector<ImageData> m_images;
	unordered_map<ImageKey, ImageData> m_imageMap;
	unordered_map<string, vector<string>> m_maps;
	vector<ItemData> m_items;
	vector<RecipeData> m_recipes;

private:
	DataManager() {};
	~DataManager() {};

	friend class Singleton<DataManager>;

};

