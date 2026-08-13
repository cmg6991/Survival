//#include "ObjectSpawner.h"
//#include "TileMap.h"
//#include "MainScene.h"
//
//namespace
//{
//    long long MakeChunkKey(int x,int y)
//    {
//        return(static_cast<long long>(x) << 32)^static_cast<unsigned int>(y);
//    }
//}
//
//void ObjectSpawner::Init(TileMap* tileMap, MainScene* scene)
//{
//    m_tileMap = tileMap;
//    m_scene = scene;
//
//    std::random_device rd;
//
//    m_rng.seed(rd());
//}
//
//void ObjectSpawner::GenerateChunk(int chunkX,int chunkY)
//{
//    if (IsChunkGenerated(chunkX,chunkY))
//    {
//        return;
//    }
//
//    m_generatedChunks.insert(MakeChunkKey(chunkX,chunkY));
//    const int CHUNK_SIZE = 16;
//    // Chunk 안에서 몇 개의 위치를 검사할지
//    const int ATTEMPTS = 30;
//
//    std::uniform_int_distribution<int> randomTile(0, CHUNK_SIZE - 1);
//    std::uniform_real_distribution<float> randomChance(0.0f, 1.0f);
//
//    for (int i = 0;i < ATTEMPTS;i++)
//    {
//        int localX =randomTile(m_rng);
//        int localY =randomTile(m_rng);
//        int worldX =chunkX * CHUNK_SIZE+ localX;
//        int worldY =chunkY * CHUNK_SIZE+ localY;
//
//        if (!CanSpawnAt(worldX,worldY))
//        {
//            continue;
//        }
//
//        float chance = randomChance(m_rng);
//
//        if (chance < 0.25f)
//        {
//            SpawnObject(SpawnObjectType::DecorationTree,worldX,worldY
//            );
//        }
//        else if (chance < 0.40f)
//        {
//            SpawnObject(SpawnObjectType::DecorationRock,worldX,worldY);
//        }
//
//        else if (chance < 0.65f)
//        {
//            SpawnObject(SpawnObjectType::Grass,worldX,worldY);
//        }
//
//        else if (chance < 0.80f)
//        {
//            SpawnObject(SpawnObjectType::ResourceTree,worldX,worldY);
//        }
//        else
//        {
//            SpawnObject(SpawnObjectType::ResourceRock,worldX,worldY);
//        }
//    }
//}
//
//void ObjectSpawner::SpawnObject(SpawnObjectType type, int tileX, int tileY)
//{
//    //i/*f (m_scene == nullptr)
//    //    return;
//
//
//    //MathEngine::Vector2 worldPos =
//    //    TileManager::GetInstance()
//    //    .TileToScreen(
//    //        {
//    //            static_cast<float>(tileX),
//    //            static_cast<float>(tileY)
//    //        }
//    //    );
//
//
//    //float x = worldPos.x;
//    //float y = worldPos.y;
//
//
//    //switch (type)
//    //{
//    //case SpawnObjectType::DecorationTree:
//
//    //    m_scene->CreateDecorationTree(
//    //        x,
//    //        y
//    //    );
//
//    //    break;
//
//
//    //case SpawnObjectType::DecorationRock:
//
//    //    m_scene->CreateDecorationRock(
//    //        x,
//    //        y
//    //    );
//
//    //    break;
//
//
//    //case SpawnObjectType::Grass:
//
//    //    m_scene->CreateDecorationGrass(
//    //        x,
//    //        y
//    //    );
//
//    //    break;
//
//
//    //case SpawnObjectType::ResourceTree:
//
//    //    m_scene->CreateResourceTree(
//    //        x,
//    //        y
//    //    );
//
//    //    break;
//
//
//    //case SpawnObjectType::ResourceRock:
//
//    //    m_scene->CreateResourceRock(
//    //        x,
//    //        y
//    //    );
//
//    //    break;
//    //}*/
//}
//
//bool ObjectSpawner::CanSpawnAt(int tileX, int tileY)
//{
//    if (m_tileMap == nullptr)
//        return false;
//
//    TileType tile =m_tileMap->GetTile(tileX,tileY);
//    // 물
//    if (tile == TileType::WATER)
//        return false;
//    // 도로
//    if (tile == TileType::ROAD)
//        return false;
//    // 일반 바닥만 허용
//    if (tile != TileType::FLOOR)
//        return false;
//
//
//    return true;
//}
