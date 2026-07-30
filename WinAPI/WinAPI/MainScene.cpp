#include "MainScene.h"
#include "TileMap.h"

#include "ResourceManager.h"
#include "CollisionManager.h"
#include "InputManager.h"
#include "CameraManager.h"

#include "Player.h"
#include "GameObject.h"
#include "Transform.h"
#include "SpriteRenderer.h"
#include "Animator.h"

MainScene::MainScene(ResourceManager* resourceManager) 
	: Scene("MainScene"), m_tileMap(nullptr), 
	m_resourceManager(resourceManager), m_collisionManager(nullptr),
	m_player(nullptr)
{
	m_tileMap = new TileMap;
	m_collisionManager = new CollisionManager;
}

MainScene::~MainScene()
{
}

void MainScene::Init()
{
	m_tileMap->Init();
	m_resourceManager->Init();
	m_resourceManager->AddImage("Player", "Resource/Character0_Walk.png");
	m_resourceManager->AddImage("Wall_E", "Resource/castle_wall(7).png");
	m_resourceManager->AddImage("Wall_N", "Resource/castle_wall(7).png");
	m_resourceManager->AddImage("Wall_S", "Resource/castle_wall(7).png");
	m_resourceManager->AddImage("Wall_W", "Resource/castle_wall(7).png");
	m_resourceManager->AddImage("Tile_W", "Resource/Tile1.png");
	GameObject* playerObj = new GameObject("Player");

	Transform* tr = new Transform();

	//tr->SetPosition(8.5);
	tr->SetFloatX(10);
	tr->SetFloatY(5);

	Player* player = new Player();
	SpriteRenderer* sprite = new SpriteRenderer("Player");
	Animator* animator = new Animator();
	sprite->SetPivot(230, 370);
	sprite->SetScale(0.3f);
	player->SetTileMap(m_tileMap);
	player->SetCollisionManager(m_collisionManager);
	playerObj->SetElement(tr, ElementType::Transform);
	playerObj->SetElement(player, ElementType::Player);
	playerObj->SetElement(sprite, ElementType::SpriteRenderer);
	playerObj->SetElement(animator, ElementType::Animator);
	sprite->SetResourceManager(m_resourceManager);
	playerObj->Init();
	m_objects.push_back(playerObj);
	m_player = player;

	for (int x = 1; x <= 20; x++)
	{
		CreateWall(x, 1, "Wall_N");
	}

	for (int x = 1; x <= 20; x++)
	{
		CreateWall(x, 20, "Wall_S");
	}
	for (int y = 2; y < 20; y++)
	{
		CreateWall(1, y, "Wall_W");
	}
	for (int y = 2; y < 20; y++)
	{
		CreateWall(20, y, "Wall_E");
	}
}

void MainScene::FixedUpdate()
{
}

void MainScene::Update(float deltaTime)
{
	InputManager::GetInstance().Update();

	Scene::Update(deltaTime);

	CameraManager::GetInstance().Follow(m_player->GetTransform());
}

void MainScene::LateUpdate()
{
}

void MainScene::PreRender()
{
}

void MainScene::Render(ID2D1DeviceContext* context)
{
	m_tileMap->Render(context, m_resourceManager);

	Scene::Render(context);

	m_collisionManager->RenderDebug(context);
}

void MainScene::PostRender(ID2D1DeviceContext* context)
{
}

void MainScene::Release()
{
	delete m_tileMap;
	m_tileMap = nullptr;

	delete m_collisionManager;
	m_collisionManager = nullptr;

	Scene::Release();
}

void MainScene::CreateWall(float x, float y, const string& imageName)
{
	GameObject* wallObj = CreateObject("Wall");

	Transform* tr = new Transform();
	tr->SetPosition({ x, y });

	SpriteRenderer* sprite = new SpriteRenderer(imageName);
	sprite->SetPivot(47, 112);
	sprite->SetResourceManager(m_resourceManager);

	wallObj->SetElement(tr, ElementType::Transform);
	wallObj->SetElement(sprite, ElementType::Wall);
	wallObj->SetElement(sprite, ElementType::SpriteRenderer);
	wallObj->Init();

	m_collisionManager->SetBlocked(x, y, true);
}