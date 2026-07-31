#include "MainScene.h"
#include "TileMap.h"

#include "ResourceManager.h"
#include "CollisionManager.h"
#include "InputManager.h"
#include "CameraManager.h"
#include "DataManager.h"
#include "TimeManager.h"
#include "SaveManager.h"

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

	for (const ImageData& img : DataManager::GetInstance().GetImageList())
	{
		m_resourceManager->AddImage(img.keyString, img.path);
	}

	GameObject* playerObj = new GameObject("Player");

	Transform* tr = new Transform();

	//tr->SetPosition(8.5);
	//tr->SetFloatX(10);
	//tr->SetFloatY(5);

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

	LoadMap(DataManager::GetInstance().GetMap("MainMap"));

	TimeManager::GetInstance().Init();

	if (SaveManager::HasSaveFile())
	{
		SaveData data;
		SaveManager::Load(data);

		Transform* playerTr = static_cast<Transform*>(m_player->GetGameObject()->GetElement(ElementType::Transform));
		playerTr->SetPosition({ data.playerX, data.playerY });

		TimeManager::GetInstance().SetTime(data.day, data.hour, data.minute);

		//m_waveManager->StartWave(data.currentWave);
	}
	else
	{
		//m_waveManager->StartWave(1);
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
	SaveGame();

	delete m_tileMap;
	m_tileMap = nullptr;

	delete m_collisionManager;
	m_collisionManager = nullptr;

	Scene::Release();
}

void MainScene::SaveGame()
{
	SaveData data;

	Transform* playerTr = static_cast<Transform*>(m_player->GetGameObject()->GetElement(ElementType::Transform));
	data.playerX = playerTr->GetPostion().x;
	data.playerY = playerTr->GetPostion().y;

	data.day = TimeManager::GetInstance().GetDay();
	data.hour = TimeManager::GetInstance().GetHour();
	data.minute = TimeManager::GetInstance().GetMinute();

	//data.currentWave = m_waveManager->GetCurrentWave();

	SaveManager::Save(data);
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

void MainScene::LoadMap(const vector<string>& mapData)
{
	for (int y = 0; y < (int)mapData.size(); y++)
	{
		const string& row = mapData[y];
		for (int x = 0; x < (int)row.size(); x++)
		{
			char tile = row[x];

			if (tile == '#')
			{
				string wallImage = "Wall_N";
				if (y == 0) wallImage = "Wall_N";
				else if (y == (int)mapData.size() - 1) wallImage = "Wall_S";
				else if (x == 0) wallImage = "Wall_W";
				else if (x == (int)row.size() - 1) wallImage = "Wall_E";

				CreateWall((float)x, (float)y, wallImage);
			}
			else if (tile == 'P')
			{
				Transform* playerTr = static_cast<Transform*>(m_player->GetGameObject()->GetElement(ElementType::Transform));
				playerTr->SetFloatX((float)x);
				playerTr->SetFloatY((float)y);
			}
		}
	}
}
