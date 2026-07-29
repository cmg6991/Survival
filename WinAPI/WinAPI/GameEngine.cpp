#include "GameEngine.h"
#include "Graphics.h"

#include "CameraManager.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "CollisionManager.h"
#include "TileManager.h"

#include "Animator.h"
#include "TileMap.h"
#include "Player.h"
#include "DrawUtil.h"
#include "Tree.h"
#include "Transform.h"
#include "SpriteRenderer.h"
#include "Wall.h"
#include <algorithm>

GameEngine::GameEngine() : m_tileMap(nullptr),m_resourceManager(nullptr), m_tree(nullptr),m_collisionManager(nullptr)
{
	m_tileMap = new TileMap;
	m_player = new Player;
	m_resourceManager = new ResourceManager;
	m_collisionManager = new CollisionManager;
	//m_tree = new Tree;
}

GameEngine::~GameEngine()
{
	Release();
}

void GameEngine::Init(const HWND hwnd)
{
	InputManager::GetInstance().Init(hwnd);
	CameraManager::GetInstance().Init();

	m_tileMap->Init();
	m_resourceManager->Init();
	m_resourceManager->AddImage("Player", "Resource/Character0_Walk.png");
	m_resourceManager->AddImage("Tile", "Resource/Tile.bmp");
	//m_resourceManager->AddImage("Wall", "Resource/Wall.bmp");
	//m_resourceManager->AddImage("Tree", "Resource/Streelamp.bmp");
	m_resourceManager->AddImage("Wall_E", "Resource/castle_wall(7).png");
	m_resourceManager->AddImage("Wall_N", "Resource/castle_wall(7).png");
	m_resourceManager->AddImage("Wall_S", "Resource/castle_wall(7).png");
	m_resourceManager->AddImage("Wall_W", "Resource/castle_wall(7).png");
	m_resourceManager->AddImage("Tile_W", "Resource/tile1.png");

	//TileManager::GetInstance().Init(m_resourceManager->GetImage("Tile_W"));

	GameObject* playerObj = new GameObject("Player");

	Transform* tr = new Transform();

	//tr->SetPosition(8.5);
	tr->SetFloatX(10);
	tr->SetFloatY(5);

	Player* player = new Player();
	SpriteRenderer* sprite =new SpriteRenderer("Player");
	Animator* animator = new Animator();
	sprite->SetPivot(230, 370);
	player->SetTileMap(m_tileMap);
	player->SetCollisionManager(m_collisionManager);
	playerObj->SetElement(tr,ElementType::Transform);
	playerObj->SetElement(player,ElementType::Player);
	playerObj->SetElement(sprite,ElementType::SpriteRenderer);
	playerObj->SetElement(animator, ElementType::Animator);
	sprite->SetResourceManager(m_resourceManager);
	playerObj->Init();
	m_objects.push_back(playerObj);
	m_player = player;

	/*GameObject* treeObj =new GameObject("Tree");
	Transform* treeTr =new Transform();
	treeTr->SetPosition(MathEngine::Vector2(12.f, 12.f));
	SpriteRenderer* treeSprite = new SpriteRenderer("Tree");
	treeObj->SetElement(treeTr,ElementType::Transform);
	treeObj->SetElement(treeSprite,ElementType::SpriteRenderer);
	treeSprite->SetResourceManager(m_resourceManager);
	treeObj->Init();
	m_objects.push_back(treeObj);*/

	for (int x = 1; x <= 20; x+=2)
	{
		CreateWall(x, 1, "Wall_N");
	}

	for (int x = 1; x <= 20; x+=2)
	{
		CreateWall(x, 20, "Wall_S");
	}
	for (int y = 2; y < 20; y+=2)
	{
		CreateWall(1, y, "Wall_W");
	}
	for (int y = 2; y < 20; y+=2)
	{
		CreateWall(20, y, "Wall_E");
	}
}

void GameEngine::Update()
{
	InputManager::GetInstance().Update();

	for (GameObject* obj : m_objects)
	{
		obj->Update(0.016f);
	}

	CameraManager::GetInstance().Follow(m_player->GetTransform());
}

void GameEngine::Render(ID2D1DeviceContext* context)
{
	std::sort(m_objects.begin(),m_objects.end(),[](GameObject* a, GameObject* b)
		{
			Transform* ta = static_cast<Transform*>(a->GetElement(ElementType::Transform));
			Transform* tb = static_cast<Transform*>(b->GetElement(ElementType::Transform));

			return ta->GetPostion().y <tb->GetPostion().y;
		});


	for (GameObject* obj : m_objects)
	{
		obj->Render(context);
	}
	//m_collisionManager->RenderDebug(context);
}

void GameEngine::Release()
{
	for (GameObject* obj : m_objects)
	{
		delete obj;
	}

	m_objects.clear();
	delete m_tileMap;
	m_tileMap = nullptr;

	m_resourceManager->Release();
	delete m_resourceManager;
	m_resourceManager = nullptr;

	delete m_collisionManager;
	m_collisionManager = nullptr;
}

void GameEngine::CreateWall(float x, float y,const string& imageName)
{
	GameObject* wallObj =new GameObject("Wall");
	Transform* tr =new Transform();
	tr->SetPosition({ x, y });
	//Wall* wall =new Wall();
	SpriteRenderer* sprite =new SpriteRenderer(imageName);
	sprite->SetPivot(47, 112);
	sprite->SetResourceManager(m_resourceManager);
	wallObj->SetElement(tr,ElementType::Transform);
	wallObj->SetElement(sprite, ElementType::Wall);
	wallObj->SetElement(sprite,ElementType::SpriteRenderer);
	wallObj->Init();

	m_objects.push_back(wallObj);
	m_collisionManager->SetBlocked(x, y,true);
}
