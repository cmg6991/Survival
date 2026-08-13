#pragma once
#include "pch.h"

class TileMap;
class MainScene;

enum class SpawnObjectType
{
	DecorationTree,
	DecorationRock,
	Grass,

	ResourceTree,
	ResourceRock,
	End
};

struct SpawnRule
{
	SpawnObjectType type;
	float probability;
	int minCount;
	int maxCount;
};

class ObjectSpawner
{
public:
	void Init(TileMap* tileMap, MainScene* scene);
	void GenerateChunk(int chunkX, int chunkY);

private:
	void SpawnObject(SpawnObjectType type, int tileX, int tileY);
	bool CanSpawnAt(int tileX, int tileY);
	bool IsChunkGenerated(int chunkX, int chunkY) const;

private:
	TileMap* m_tileMap = nullptr;
	MainScene* m_scene = nullptr;

	std::unordered_set<long long> m_generatedChunks;

	std::mt19937 m_rng;
};

