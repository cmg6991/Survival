#pragma once
#include "pch.h"
#include "Singleton.h"
#include "CraftData.h"

enum class ImageKey
{
	Player_Idle,
	Player_Walk,
	Player_Attack,
	Player_Die,
	Player_Gun_Idle,
	Player_Gun_Walk,
	Player_Gun_Attack,
	Player_Sword_Idle,
	Player_Sword_Walk,
	Player_Sword_Attack,
	Player_Shield_Idle,
	Player_Shield_Walk,
	Wall_N,
	Wall_S,
	Wall_E,
	Wall_W,
	Tile_W,
	CampFire,
	WorkTable,
	Item_Wood,
	Item_Stone,
	Item_Sword,
	Item_Gun,
	Item_Shield,
	Item_Apple,
	Item_Banana,
	Item_Grape,
	Item_Mushroom,
	Item_Mushroom_Baked,
	Bullet,
	Monster,
	Monster2,
	Monster3,
	SideBarUI,
	Fire,
	GrassRoad,
	WaterTile,
	Grass,
	CrossHair,
	BubbleText,
	Text,
	NightUI,
	DayUI,
	MiniMapFrame,
	HPUI,
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

	vector<RecipeData> GetRecipesByStation(const string& station) const;

	const vector<MonsterData>& GetMonsterList() const { return m_monsters; }
	const MonsterData* FindMonster(const string& id) const;

private:
	void LoadImageData(const string& filePath);
	void LoadMapData(const string& filePath);

	ImageKey StringToImageKey(const string& str) const;

	void LoadItemData(const string& filePath);
	void LoadRecipeData(const string& filePath);
	void LoadMonsterData(const string& filePath);

private:
	vector<ImageData> m_images;
	unordered_map<ImageKey, ImageData> m_imageMap;
	unordered_map<string, vector<string>> m_maps;
	vector<ItemData> m_items;
	vector<RecipeData> m_recipes;
	vector<MonsterData> m_monsters;
private:
	DataManager() {};
	~DataManager() {};

	friend class Singleton<DataManager>;

};

