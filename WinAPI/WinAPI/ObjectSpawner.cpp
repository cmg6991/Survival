#include "ObjectSpawner.h"
#include "MainScene.h"
#include "TileMap.h"
#include "TileManager.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Transform.h"
#include "SpriteRenderer.h"
#include "Interactable.h"
#include "ResourceNode.h"
#include "pch.h"

ObjectSpawner::ObjectSpawner() : m_scene(nullptr), m_resourceManager(nullptr), m_tileMap(nullptr)
{
    random_device rd;
    m_random.seed(rd());
}

ObjectSpawner::~ObjectSpawner()
{
}

void ObjectSpawner::Init(MainScene* scene, ResourceManager* resourceManager, TileMap* tileMap)
{
    m_scene = scene;
    m_resourceManager = resourceManager;
    m_tileMap = tileMap;
}

void ObjectSpawner::SpawnChunk(int startX,int startY,int chunkWidth,int chunkHeight)
{
    OutputDebugStringA("=== ObjectSpawner::SpawnChunk ===\n");
    uniform_real_distribution<float> chance(0.0f, 1.0f);

    for (int y = startY;y < startY + chunkHeight;y++)
    {
        for (int x = startX;x < startX + chunkWidth;x++)
        {
            // 이미 생성된 자리면 무시
            string key =to_string(x) + "_" +to_string(y);

            if (m_spawnedPositions.find(key)
                != m_spawnedPositions.end())
            {
                continue;
            }

            // 물/도로 등에는 생성하지 않음
            if (!CanSpawnAt(x, y))
                continue;

            float value = chance(m_random);

            if (value < 0.05f)
            {
                SpawnObject(SpawnObjectType::Tree,x,y);
            }
            else if (value < 0.08f)
            {
                SpawnObject(SpawnObjectType::Rock,x,y);
            }
            else if (value < 0.13f)
            {
                SpawnObject(SpawnObjectType::Grass,x,y);
            }

            m_spawnedPositions.insert(key);
        }
    }


}

bool ObjectSpawner::CanSpawnAt(int x, int y) const
{
    if (m_tileMap == nullptr)
        return false;

    TileType tile =m_tileMap->GetTile(x, y);

    // 도로에는 생성하지 않음
    if (tile == TileType::ROAD)
        return false;

    // 물에는 생성하지 않음
    if (tile == TileType::WATER)
        return false;

    // 기본 땅에만 생성
    if (tile != TileType::FLOOR)
        return false;

    return true;
}

void ObjectSpawner::SpawnObject(SpawnObjectType type, int x, int y)
{
    switch (type)
    {
    case SpawnObjectType::Tree:
        SpawnTree(x, y);
        break;

    case SpawnObjectType::Rock:
        SpawnRock(x, y);
        break;

    case SpawnObjectType::Grass:
        SpawnGrass(x, y);
        break;
    }
}

void ObjectSpawner::SpawnTree(int x, int y)
{
    //MathEngine::Vector2 world =TileManager::GetInstance().TileToScreen({(float)x,(float)y});

    GameObject* obj =m_scene->CreateObject("Tree");

    Transform* tr =new Transform();

    //tr->SetPosition(world);
    tr->SetPosition({ (float)x, (float)y });
    SpriteRenderer* sprite =new SpriteRenderer("Item_Wood");

    sprite->SetResourceManager(m_resourceManager);
 ///*   sprite->SetPivot(64,120 );

 //   sprite->SetScale(0.5f);*/

    Interactable* interact =new Interactable(InteractType::Tree);

    ResourceNode* resource =new ResourceNode("Item_Wood",1,3);


    obj->SetElement(tr,ElementType::Transform);

    obj->SetElement(sprite, ElementType::SpriteRenderer);
    obj->SetElement(interact,ElementType::Interactable);
    obj->SetElement(resource,ElementType::ResourceNode);
    obj->Init();
}

void ObjectSpawner::SpawnRock(int x, int y)
{
    //MathEngine::Vector2 world =TileManager::GetInstance().TileToScreen({(float)x,(float)y});

    GameObject* obj =m_scene->CreateObject("Rock");


    Transform* tr =new Transform();
    //tr->SetPosition(world);
    tr->SetPosition({ (float)x, (float)y });
    SpriteRenderer* sprite =new SpriteRenderer("Item_Stone");

    sprite->SetResourceManager(m_resourceManager);

    //sprite->SetPivot(32,32);

    //sprite->SetScale(0.5f);

    Interactable* interact =new Interactable(InteractType::Rock);
    ResourceNode* resource =new ResourceNode("Item_Stone",1,2);

    obj->SetElement(tr,ElementType::Transform);
    obj->SetElement(sprite,ElementType::SpriteRenderer);
    obj->SetElement(interact,ElementType::Interactable);
    obj->SetElement(resource,ElementType::ResourceNode);
    obj->Init();
}

void ObjectSpawner::SpawnGrass(int x, int y)
{
   // MathEngine::Vector2 world =TileManager::GetInstance().TileToScreen({(float)x,(float)y});

    GameObject* obj =m_scene->CreateObject("GrassObject");
    Transform* tr =new Transform();
    //tr->SetPosition(world);
    tr->SetPosition({ (float)x, (float)y });
    SpriteRenderer* sprite =new SpriteRenderer("Grass");

    sprite->SetResourceManager(m_resourceManager);
    //sprite->SetPivot(16,32);
    //sprite->SetScale(0.5f);
    obj->SetElement(tr,ElementType::Transform);
    obj->SetElement(sprite, ElementType::SpriteRenderer);
    obj->Init();
}


//#include "ObjectSpawner.h"
//
//#include "MainScene.h"
//#include "TileMap.h"
//#include "ResourceManager.h"
//#include "CollisionManager.h"
//
//#include "GameObject.h"
//#include "Transform.h"
//#include "SpriteRenderer.h"
//
//ObjectSpawner::ObjectSpawner()
//    : m_scene(nullptr)
//    , m_tileMap(nullptr)
//    , m_resourceManager(nullptr)
//    , m_collisionManager(nullptr)
//{
//    random_device rd;
//    m_rng.seed(rd());
//}
//
//ObjectSpawner::~ObjectSpawner()
//{
//}
//
//void ObjectSpawner::Init(
//    MainScene* scene,
//    TileMap* tileMap,
//    ResourceManager* resourceManager,
//    CollisionManager* collisionManager)
//{
//    m_scene = scene;
//    m_tileMap = tileMap;
//    m_resourceManager = resourceManager;
//    m_collisionManager = collisionManager;
//}
//void ObjectSpawner::SpawnTree(float x, float y)
//{
//    if (m_scene == nullptr)
//        return;
//
//    GameObject* obj =
//        m_scene->CreateObject("Tree");
//
//    Transform* tr = new Transform();
//    tr->SetPosition({ x, y });
//
//    SpriteRenderer* sprite =
//        new SpriteRenderer("Item_Wood");
//
//    sprite->SetResourceManager(
//        m_resourceManager
//    );
//
//    obj->SetElement(
//        tr,
//        ElementType::Transform
//    );
//
//    obj->SetElement(
//        sprite,
//        ElementType::SpriteRenderer
//    );
//
//    obj->Init();
//}
//void ObjectSpawner::SpawnRock(float x, float y)
//{
//    if (m_scene == nullptr)
//        return;
//
//    GameObject* obj =
//        m_scene->CreateObject("Rock");
//
//    Transform* tr = new Transform();
//    tr->SetPosition({ x, y });
//
//    SpriteRenderer* sprite =
//        new SpriteRenderer("Item_Stone");
//
//    sprite->SetResourceManager(
//        m_resourceManager
//    );
//
//    obj->SetElement(
//        tr,
//        ElementType::Transform
//    );
//
//    obj->SetElement(
//        sprite,
//        ElementType::SpriteRenderer
//    );
//
//    obj->Init();
//}
//void ObjectSpawner::SpawnChunk(
//    int chunkX,
//    int chunkY)
//{
//    string chunkId =
//        to_string(chunkX) +
//        "_" +
//        to_string(chunkY);
//
//    // 이미 생성한 청크면 다시 생성하지 않음
//    if (m_spawnedChunks.find(chunkId)
//        != m_spawnedChunks.end())
//    {
//        return;
//    }
//
//    m_spawnedChunks.insert(chunkId);
//
//    const int CHUNK_SIZE = 16;
//
//    int startX =
//        chunkX * CHUNK_SIZE;
//
//    int startY =
//        chunkY * CHUNK_SIZE;
//
//    // =====================================
//    // 테스트용
//    // =====================================
//
//    SpawnTree(
//        startX + 3.0f,
//        startY + 3.0f
//    );
//
//    SpawnTree(
//        startX + 7.0f,
//        startY + 5.0f
//    );
//
//    SpawnTree(
//        startX + 12.0f,
//        startY + 8.0f
//    );
//
//    SpawnRock(
//        startX + 5.0f,
//        startY + 10.0f
//    );
//
//    SpawnRock(
//        startX + 10.0f,
//        startY + 12.0f
//    );
//}