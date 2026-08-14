#pragma once
#include "pch.h"

class TileMap;
class MainScene;
class ResourceManager;

enum class ChunkType
{
    GrassLand,
    Lake,
    Snow
};

enum class SpawnObjectType
{
	Tree,
	Rock,
	Grass
};

class ObjectSpawner
{
public:
    ObjectSpawner();
    ~ObjectSpawner();

    void Init(MainScene* scene, ResourceManager* resourceManager, TileMap* tileMap);
    void SpawnChunk(int chunkX, int chunkY, int chunkWidth, int chunkHeight);

    ChunkType GetChunkType(int chunkX, int chunkY) const;

private:
    bool CanSpawnAt(int x,int y) const;

    void SpawnObject(SpawnObjectType type,int x,int y);

    void SpawnTree(int x,int y);

    void SpawnRock(int x,int y);

    void SpawnGrass(int x,int y);

private:
    MainScene* m_scene;
    ResourceManager* m_resourceManager;
    TileMap* m_tileMap;

    mt19937 m_random;

    // 중복 생성 방지
    unordered_set<string> m_spawnedPositions;

};


//class MainScene;
//class TileMap;
//class ResourceManager;
//class CollisionManager;
//
//class ObjectSpawner
//{
//public:
//    ObjectSpawner();
//    ~ObjectSpawner();
//
//    void Init(
//        MainScene* scene,
//        TileMap* tileMap,
//        ResourceManager* resourceManager,
//        CollisionManager* collisionManager
//    );
//
//    // 청크 하나가 생성될 때 호출
//    void SpawnChunk(int chunkX, int chunkY);
//
//private:
//
//    void SpawnTree(float x, float y);
//    void SpawnRock(float x, float y);
//
//private:
//
//    MainScene* m_scene;
//    TileMap* m_tileMap;
//    ResourceManager* m_resourceManager;
//    CollisionManager* m_collisionManager;
//
//    unordered_set<string> m_spawnedChunks;
//
//    mt19937 m_rng;
//};