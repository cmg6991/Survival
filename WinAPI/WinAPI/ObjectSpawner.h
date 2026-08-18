#pragma once
#include "pch.h"
#include "ChunkType.h"

class TileMap;
class MainScene;
class ResourceManager;
class GameObject;

enum class SpawnObjectType
{
	Tree,
	Rock,
	Grass,
    Iron,
    Flower
};

class ObjectSpawner
{
public:
    ObjectSpawner();
    ~ObjectSpawner();

    void Init(MainScene* scene, ResourceManager* resourceManager, TileMap* tileMap);
    void SpawnChunk(int chunkX, int chunkY, int chunkWidth, int chunkHeight);

    ChunkType GetChunkType(int chunkX, int chunkY) const;

    void UnloadChunk(int chunkX, int chunkY);

private:
    bool CanSpawnAt(int x,int y) const;

    void SpawnObject(SpawnObjectType type,int x,int y, ChunkType chunkType, const string& chunkKey);

    /*void SpawnTree(int x,int y);

    void SpawnRock(int x,int y);*/

    string PickResourceId(const vector<string>& pool);
    GameObject* SpawnResourceObject(const string& resourceId, int x, int y);

    GameObject* SpawnGrass(int x, int y, const string& imageKey, float scale);
    GameObject* SpawnFlower(int x, int y, const string& imageKey, float scale);

private:
    MainScene* m_scene;
    ResourceManager* m_resourceManager;
    TileMap* m_tileMap;

    mt19937 m_random;

    // 중복 생성 방지
    unordered_set<string> m_spawnedPositions;

    unordered_map<ChunkType, vector<string>> m_treePoolByChunk;
    unordered_map<ChunkType, vector<string>> m_rockPoolByChunk;
    unordered_map<ChunkType, vector<string>> m_IronPoolByChunk;

    // 청크 타입별 등장 가능한 잔디 이미지+스케일 목록
    unordered_map<ChunkType, vector<pair<string, float>>> m_grassImagePoolByChunk;
    unordered_map<ChunkType, vector<pair<string, float>>> m_flowerImagePoolByChunk;

    struct ChunkData
    {
        vector<GameObject*> objects;
        vector<string> positionKeys;
    };
    unordered_map<string, ChunkData> m_chunkObjects;
};
