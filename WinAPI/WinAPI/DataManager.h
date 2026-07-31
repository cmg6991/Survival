#pragma once
#include "pch.h"
#include "Singleton.h"

enum class ImageKey
{
	Player,
	Wall_N,
	Wall_S,
	Wall_E,
	Wall_W,
	Tile_W,
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

private:
	void LoadImageData(const string& filePath);
	void LoadMapData(const string& filePath);

	ImageKey StringToImageKey(const string& str) const;

private:
	vector<ImageData> m_images;
	unordered_map<ImageKey, ImageData> m_imageMap;
	unordered_map<string, vector<string>> m_maps;

private:
	DataManager() {};
	~DataManager() {};

	friend class Singleton<DataManager>;

};

