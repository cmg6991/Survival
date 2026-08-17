#include "GameEngine.h"
#include "Graphics.h"

#include "CameraManager.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "CollisionManager.h"
#include "TileManager.h"
#include "SceneManager.h"
#include "TimeManager.h"
#include "DataManager.h"
#include "UIManager.h"

#include "Animator.h"
#include "TileMap.h"
#include "Player.h"
#include "DrawUtil.h"
#include "Tree.h"
#include "Transform.h"
#include "SpriteRenderer.h"
#include "Wall.h"
#include <algorithm>
#include "MainScene.h"
#include "TitleScene.h"


GameEngine::GameEngine()/* : m_tileMap(nullptr),m_resourceManager(nullptr), m_tree(nullptr),m_collisionManager(nullptr)*/
{
	//m_tileMap = new TileMap;
	//m_player = new Player;
	m_resourceManager = new ResourceManager;
	//m_collisionManager = new CollisionManager;
	m_sceneManager = new SceneManager;
}

GameEngine::~GameEngine()
{
	Release();
}

void GameEngine::Init(const HWND hwnd)
{
	InputManager::GetInstance().Init(hwnd);
	CameraManager::GetInstance().Init();
	TimeManager::GetInstance().Init();
	DataManager::GetInstance().Init();
	m_resourceManager->Init();

	m_resourceManager->Init();
	for (const ImageData& img : DataManager::GetInstance().GetImageList())
	{
		m_resourceManager->AddImage(img.keyString, img.path);
	}

	m_sceneManager->Init(m_resourceManager);

	ResourceManager* rm = m_resourceManager;
	SceneManager* sm = m_sceneManager;
	m_sceneManager->CreateScene("Title", [rm,sm]() -> Scene* { return new TitleScene(rm,sm); });
	m_sceneManager->CreateScene("Main", [rm]() -> Scene* { return new MainScene(rm); });
	m_sceneManager->LoadScene("Title");

	
}

void GameEngine::FixedUpdate()
{
	m_sceneManager->FixedUpdate();
}

void GameEngine::Update(float deltaTime)
{
	InputManager::GetInstance().Update();
	//CameraManager::GetInstance().Follow(m_player->GetTransform());
	TimeManager::GetInstance().Update(deltaTime);
	m_sceneManager->Update(deltaTime);
}


void GameEngine::LateUpdate()
{
	m_sceneManager->LateUpdate();
}

void GameEngine::Render(ID2D1DeviceContext* context)
{
	/*m_tileMap->Render(context, m_resourceManager);
	std::sort(m_objects.begin(),m_objects.end(),[](GameObject* a, GameObject* b)
		{
			Transform* ta = static_cast<Transform*>(a->GetElement(ElementType::Transform));
			Transform* tb = static_cast<Transform*>(b->GetElement(ElementType::Transform));

			float sumA = ta->GetPostion().x + ta->GetPostion().y;
			float sumB = tb->GetPostion().x + tb->GetPostion().y;

			return sumA < sumB;
		});


	for (GameObject* obj : m_objects)
	{
		obj->Render(context);
	}
	m_collisionManager->RenderDebug(context);*/
	m_sceneManager->PreRender();
	m_sceneManager->Render(context);
	m_sceneManager->PostRender(context);
	
}

void GameEngine::Release()
{
	m_sceneManager->Release();
	delete m_sceneManager;
	m_sceneManager = nullptr;

	m_resourceManager->Release();
	delete m_resourceManager;
	m_resourceManager = nullptr;
}