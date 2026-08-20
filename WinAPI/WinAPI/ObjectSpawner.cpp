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
#include "DataManager.h"
#include "Chicken.h"
#include "Animator.h"
#include "pch.h"

ObjectSpawner::ObjectSpawner() : m_scene(nullptr), m_resourceManager(nullptr), m_tileMap(nullptr)
{
    random_device rd;
    m_random.seed(rd());

    m_treePoolByChunk[ChunkType::GrassLand] = { "Tree_Normal" };
    m_treePoolByChunk[ChunkType::Lake] = { "Tree_Normal" };
    m_treePoolByChunk[ChunkType::Snow] = { "Tree_Winter" };

    m_rockPoolByChunk[ChunkType::GrassLand] = { "Rock_Normal" };
    m_rockPoolByChunk[ChunkType::Lake] = { "Rock_Normal" };
    //m_rockPoolByChunk[ChunkType::Snow] = { "Rock_Normal" };

    m_IronPoolByChunk[ChunkType::Rock] = { "IronBox" };

    // ── 청크 타입별 잔디(장식) 풀 ──
    m_grassImagePoolByChunk[ChunkType::GrassLand] = { { "Grass", 2.0f } };
    m_grassImagePoolByChunk[ChunkType::Lake] = { { "Grass", 1.5f } };
    m_grassImagePoolByChunk[ChunkType::Snow] = { { "Item_WinterGrass", 1.5f } };

    m_flowerImagePoolByChunk[ChunkType::Lake] = { {"Flower", 1.5f},{"Flower2",2.f},{"Flower3", 1.5f} };
    m_chickenImagePoolByChunk[ChunkType::Lake] = { { "Chicken", 1.f },{ "LightBrownChicken", 1.f },
        {"DarkBrownChicken", 1.f}, {"BlackChicken", 1.f} };
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

void ObjectSpawner::SpawnChunk(int chunkX,int chunkY,int chunkWidth,int chunkHeight)
{
    string chunkKey = to_string(chunkX) + "_" + to_string(chunkY);

    // 이미 스폰된 청크면 스킵
    if (m_chunkObjects.find(chunkKey) != m_chunkObjects.end())
        return;
    int startX = chunkX * chunkWidth;
    int startY = chunkY * chunkHeight;

    ChunkType chunkType = GetChunkType(chunkX, chunkY);
    uniform_real_distribution<float> chance(0.0f, 1.0f);

    m_chunkObjects[chunkKey];

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

            switch (chunkType)
            {
            case ChunkType::GrassLand:
            {
                if (value < 0.04f)
                    SpawnObject(SpawnObjectType::Tree, x, y, chunkType,chunkKey);
                else if (value < 0.06f)
                    SpawnObject(SpawnObjectType::Rock, x, y, chunkType, chunkKey);
                else if (value < 0.13f)
                    SpawnObject(SpawnObjectType::Grass, x, y, chunkType, chunkKey);
                break;
            }
            case ChunkType::Lake:
            {
                if (value < 0.02f)
                    SpawnObject(SpawnObjectType::Grass, x, y, chunkType, chunkKey);
                else if(value <0.05f)
                    SpawnObject(SpawnObjectType::Flower, x, y, chunkType, chunkKey);
                else if (value < 0.055f)
                    SpawnObject(SpawnObjectType::Chicken, x, y, chunkType, chunkKey);
                break;
            }
            case ChunkType::Snow:
            {
                if (value < 0.03f)
                    SpawnObject(SpawnObjectType::Tree, x, y, chunkType, chunkKey);
                else if (value < 0.05f)
                    SpawnObject(SpawnObjectType::Grass, x, y, chunkType, chunkKey);
                break;
            }
            case ChunkType::Rock:
            {
                if (value < 0.01f)
                    SpawnObject(SpawnObjectType::Iron, x, y, chunkType, chunkKey);
                break;
            }
            }
            m_spawnedPositions.insert(key);
            m_chunkObjects[chunkKey].positionKeys.push_back(key);
        }
    }
}

ChunkType ObjectSpawner::GetChunkType(int chunkX, int chunkY) const
{
    if (m_tileMap == nullptr)
        return ChunkType::GrassLand;

    return m_tileMap->GetChunkBiome(chunkX, chunkY);
}

void ObjectSpawner::UnloadChunk(int chunkX, int chunkY)
{
    string chunkKey = to_string(chunkX) + "_" + to_string(chunkY);

    auto it = m_chunkObjects.find(chunkKey);
    if (it == m_chunkObjects.end())
        return;

    for (GameObject* obj : it->second.objects)
    {
        m_scene->DeletePObject(obj);
    }

    for (const string& posKey : it->second.positionKeys)
    {
        m_spawnedPositions.erase(posKey);
    }

    m_chunkObjects.erase(it);
}

bool ObjectSpawner::CanSpawnAt(int x, int y) const
{
    if (m_tileMap == nullptr)
        return false;

        //if (m_tileMap->IsRoad(x, y))
        //return false;

    TileType tile =m_tileMap->GetTile(x, y);

    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            if (m_tileMap->IsRoad(x + dx, y + dy))
                return false;
        }
    }

    // 도로에는 생성하지 않음
    if (tile == TileType::ROAD)
        return false;

    // 물에는 생성하지 않음
    if (tile == TileType::WATER)
        return false;

    // 기본 땅에만 생성
    if (tile != TileType::FLOOR && tile!= TileType::SNOW && tile!=TileType::STONE)
        return false;

    return true;
}

//void ObjectSpawner::SpawnObject(SpawnObjectType type, int x, int y, ChunkType chunkType)
//{
//
//    switch (type)
//    {
//    case SpawnObjectType::Tree:
//        SpawnResourceObject(PickResourceId(m_treePoolByChunk[chunkType]), x, y);
//        break;
//
//    case SpawnObjectType::Rock:
//        SpawnResourceObject(PickResourceId(m_rockPoolByChunk[chunkType]), x, y);
//        break;
//    case SpawnObjectType::Iron:
//        SpawnResourceObject(PickResourceId(m_IronPoolByChunk[chunkType]), x, y);
//        break;
//
//    case SpawnObjectType::Grass:
//    {
//        auto& pool = m_grassImagePoolByChunk[chunkType];
//        if (!pool.empty())
//        {
//            int idx = m_random() % pool.size();
//            const auto& [imageKey, scale] = pool[idx];
//            SpawnGrass(x, y, imageKey, scale);
//        }
//        break;
//    }
//    }
//}

void ObjectSpawner::SpawnObject(SpawnObjectType type, int x, int y, ChunkType chunkType, const string& chunkKey)
{
    GameObject* obj = nullptr;

    switch (type)
    {
    case SpawnObjectType::Tree:
        obj = SpawnResourceObject(PickResourceId(m_treePoolByChunk[chunkType]), x, y);
        break;
    case SpawnObjectType::Rock:
        obj = SpawnResourceObject(PickResourceId(m_rockPoolByChunk[chunkType]), x, y);
        break;
    case SpawnObjectType::Iron:
        obj = SpawnResourceObject(PickResourceId(m_IronPoolByChunk[chunkType]), x, y);
        break;
    case SpawnObjectType::Grass:
    {
        auto& pool = m_grassImagePoolByChunk[chunkType];
        if (!pool.empty())
        {
            int idx = m_random() % pool.size();
            const auto& [imageKey, scale] = pool[idx];
            obj = SpawnGrass(x, y, imageKey, scale);
        }
        break;
    }
    case SpawnObjectType::Flower:
    {
        auto& pool = m_flowerImagePoolByChunk[chunkType];
        if (!pool.empty())
        {
            int idx = m_random() % pool.size();
            const auto& [imageKey, scale] = pool[idx];
            obj = SpawnFlower(x, y, imageKey, scale);
        }
        break;
    }
    case SpawnObjectType::Chicken:
    {
        auto& pool = m_chickenImagePoolByChunk[chunkType];
        if (!pool.empty())
        {
            int idx = m_random() % pool.size();
            const auto& [imageKey, scale] = pool[idx];
            obj = SpawnChicken(x, y, imageKey, scale);
        }
        break;
    }
    }

    if (obj != nullptr)
        m_chunkObjects[chunkKey].objects.push_back(obj);
}
string ObjectSpawner::PickResourceId(const vector<string>& pool)
{
    if (pool.empty()) return "";
    int idx = m_random() % pool.size();
    return pool[idx];
}

//void ObjectSpawner::SpawnResourceObject(const string& resourceId, int x, int y)
//{
//    if (resourceId.empty()) return;
//
//    const ResourceObjectData* data = DataManager::GetInstance().FindResourceObject(resourceId);
//    if (data == nullptr) return;
//
//    GameObject* obj = m_scene->CreateObject(data->id);
//
//    Transform* tr = new Transform();
//    tr->SetPosition({ (float)x, (float)y });
//
//    SpriteRenderer* sprite = new SpriteRenderer(data->image);
//    sprite->SetResourceManager(m_resourceManager);
//    sprite->SetScale(data->scale);
//
//    InteractType interactType = InteractType::Tree;
//    if (data->interactType == "Rock") interactType = InteractType::Rock;
//    else if (data->interactType == "Iron") interactType = InteractType::Iron;
//    Interactable* interact = new Interactable(interactType);
//
//    ResourceNode* resource = new ResourceNode(data->baseItemId, data->baseMinCount, data->baseMaxCount);
//    for (const ResourceDrop& drop : data->bonusDrops)
//    {
//        resource->AddBonusDrop(drop.itemId, drop.chance, drop.minCount, drop.maxCount);
//    }
//
//    obj->SetElement(tr, ElementType::Transform);
//    obj->SetElement(sprite, ElementType::SpriteRenderer);
//    obj->SetElement(interact, ElementType::Interactable);
//    obj->SetElement(resource, ElementType::ResourceNode);
//    obj->Init();
//}
GameObject* ObjectSpawner::SpawnResourceObject(const string& resourceId, int x, int y)
{
    if (resourceId.empty()) return nullptr;

    const ResourceObjectData* data = DataManager::GetInstance().FindResourceObject(resourceId);
    if (data == nullptr) return nullptr;

    GameObject* obj = m_scene->CreateObject(data->id);

    Transform* tr = new Transform();
    tr->SetPosition({ (float)x, (float)y });

    SpriteRenderer* sprite = new SpriteRenderer(data->image);
    sprite->SetResourceManager(m_resourceManager);
    sprite->SetScale(data->scale);
    if (data->interactType == "Iron")
    {
        sprite->SetPivot(149, 253);
    }

    InteractType interactType = InteractType::Tree;
    if (data->interactType == "Rock") interactType = InteractType::Rock;
    else if (data->interactType == "Iron") interactType = InteractType::Iron;
    Interactable* interact = new Interactable(interactType);

    ResourceNode* resource = new ResourceNode(data->baseItemId, data->baseMinCount, data->baseMaxCount);
    for (const ResourceDrop& drop : data->bonusDrops)
    {
        resource->AddBonusDrop(drop.itemId, drop.chance, drop.minCount, drop.maxCount);
    }

    obj->SetElement(tr, ElementType::Transform);
    obj->SetElement(sprite, ElementType::SpriteRenderer);
    obj->SetElement(interact, ElementType::Interactable);
    obj->SetElement(resource, ElementType::ResourceNode);
    obj->Init();

    return obj; // 추가
}

GameObject* ObjectSpawner::SpawnChicken(int x, int y, const string& imageKey, float scale)
{
    GameObject* obj =m_scene->CreateObject("Chicken");

    if (obj == nullptr)
        return nullptr;

    Transform* tr = new Transform();
    tr->SetPosition({ (float)x, (float)y });

    SpriteRenderer* sprite = new SpriteRenderer(imageKey);
    sprite->SetResourceManager(m_resourceManager);
    sprite->SetPivot(16.0f, 32.0f);

    sprite->SetScale(scale * 2.0f);


    Animator* animator = new Animator();

    ResourceNode* resource = new ResourceNode("Item_Egg", 1, 1);

    // Chicken 엘리먼트 생성 및 콜백 연결
    Chicken* chicken = new Chicken();
    chicken->SetResourceNode(resource);
    chicken->SetTileMap(m_tileMap);

    MainScene* scenePtr = m_scene;
    chicken->SetEggDropCallback(
        [scenePtr](const MathEngine::Vector2& pos, const string& itemId, int count)
        {
            scenePtr->CreateItemPickUp(pos.x, pos.y, itemId, count, false);
        });

    // GameObject 구성
    obj->SetElement(tr, ElementType::Transform);
    obj->SetElement(sprite, ElementType::SpriteRenderer);
    obj->SetElement(animator, ElementType::Animator);
    obj->SetElement(resource, ElementType::ResourceNode);
    obj->SetElement(chicken, ElementType::Chicken);
    obj->Init();

    return obj;
}

//void ObjectSpawner::SpawnGrass(int x, int y, const string& imageKey, float scale)
//{
//    GameObject* obj = m_scene->CreateObject("GrassObject");
//
//    Transform* tr = new Transform();
//    tr->SetPosition({ (float)x, (float)y });
//
//    SpriteRenderer* sprite = new SpriteRenderer(imageKey);
//    sprite->SetResourceManager(m_resourceManager);
//    sprite->SetScale(scale);
//
//    obj->SetElement(tr, ElementType::Transform);
//    obj->SetElement(sprite, ElementType::SpriteRenderer);
//    obj->Init();
//}
GameObject* ObjectSpawner::SpawnGrass(int x, int y, const string& imageKey, float scale)
{
    GameObject* obj = m_scene->CreateObject("GrassObject");

    Transform* tr = new Transform();
    tr->SetPosition({ (float)x, (float)y });

    SpriteRenderer* sprite = new SpriteRenderer(imageKey);
    sprite->SetResourceManager(m_resourceManager);
    sprite->SetScale(scale);

    obj->SetElement(tr, ElementType::Transform);
    obj->SetElement(sprite, ElementType::SpriteRenderer);
    obj->Init();

    return obj; // 추가
}

GameObject* ObjectSpawner::SpawnFlower(int x, int y, const string& imageKey, float scale)
{
    GameObject* obj = m_scene->CreateObject("Flower");

    Transform* tr = new Transform();
    tr->SetPosition({ (float)x, (float)y });

    SpriteRenderer* sprite = new SpriteRenderer(imageKey);
    sprite->SetResourceManager(m_resourceManager);
    sprite->SetScale(scale);

    obj->SetElement(tr, ElementType::Transform);
    obj->SetElement(sprite, ElementType::SpriteRenderer);
    obj->Init();

    return obj; // 추가
}
