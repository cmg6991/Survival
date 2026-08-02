#include "MainScene.h"
#include "TileMap.h"

#include "ResourceManager.h"
#include "CollisionManager.h"
#include "InputManager.h"
#include "CameraManager.h"
#include "DataManager.h"
#include "TimeManager.h"
#include "SaveManager.h"
#include "CraftingManager.h"

#include "Player.h"
#include "GameObject.h"
#include "Transform.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "Interactable.h"
#include "ItemPickUp.h"
#include "Inventory.h"

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

	RegisterTileHandlers();
	LoadMap(DataManager::GetInstance().GetMap("MainMap"));

	TimeManager::GetInstance().Init();

	if (SaveManager::HasSaveFile())
	{
		SaveData data;
		SaveManager::Load(data);

		Transform* playerTr = static_cast<Transform*>(m_player->GetGameObject()->GetElement(ElementType::Transform));
		playerTr->SetPosition({ data.playerX, data.playerY });

		TimeManager::GetInstance().SetTime(data.day, data.hour, data.minute);

		m_player->GetInventory()->SetAllItems(data.inventory);
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
	if (m_isCraftingOpen)
	{
		const vector<RecipeData>& recipes = DataManager::GetInstance().GetRecipeList();

		if (InputManager::GetInstance().IsGetKeyDown('W') && m_selectedRecipeIndex > 0)
			m_selectedRecipeIndex--;

		if (InputManager::GetInstance().IsGetKeyDown('S') && m_selectedRecipeIndex < (int)recipes.size() - 1)
			m_selectedRecipeIndex++;

		if (InputManager::GetInstance().IsGetKeyDown(VK_RETURN) && !recipes.empty())
		{
			const RecipeData& recipe = recipes[m_selectedRecipeIndex];

			/*if (CraftingManager::Craft(recipe.id, m_player->GetInventory()))
			{
				wstring resultId(recipe.resultId.begin(), recipe.resultId.end());
				m_lastMessage = L"제작 성공: " + resultId;
			}
			else
			{
				m_lastMessage = L"재료가 부족합니다";
			}*/
		}

		if (InputManager::GetInstance().IsGetKeyDown(VK_ESCAPE))
		{
			m_isCraftingOpen = false;
		}

		return;
	}

	Scene::Update(deltaTime);

	CameraManager::GetInstance().Follow(m_player->GetTransform());

	CheckItemPickUps();

	if (InputManager::GetInstance().IsGetKeyDown('E'))
	{
		Interactable* nearby = FindNearByInteractable();
		if (nearby != nullptr)
		{
			OnInteract(nearby);
		}
	}
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
	data.inventory = m_player->GetInventory()->GetAllItems();

	SaveManager::Save(data);
}

void MainScene::RegisterTileHandlers()
{
	m_tileHandlers['P'] = [this](float x, float y)
		{
			Transform* playerTr = static_cast<Transform*>(m_player->GetGameObject()->GetElement(ElementType::Transform));
			playerTr->SetFloatX(x);
			playerTr->SetFloatY(y);
		};

	m_tileHandlers['C'] = [this](float x, float y)
		{
			//CreateInteractable(x, y, InteractType::CampFire, "Campfire"); // "Campfire" 이미지는 images.json에 등록 필요
		};

	// 작업대
	m_tileHandlers['T'] = [this](float x, float y)
		{
			//CreateInteractable(x, y, InteractType::WorkTable, "WorkTable");
		};
	// 앞으로 몬스터/아이템/모닥불 등이 생기면 여기 계속 추가하면 됨
	// 예시 (해당 클래스들 만드신 뒤 주석 해제):
	// m_tileHandlers['M'] = [this](float x, float y) { CreateMonster(x, y); };
	// m_tileHandlers['w'] = [this](float x, float y) { CreateItemPickup(x, y, "Wood", 1); };
	// m_tileHandlers['C'] = [this](float x, float y) { CreateInteractable(x, y, InteractType::Campfire, "Campfire"); };
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

void MainScene::CreateInteractable(float x, float y, InteractType type, const string& imageKey)
{
	GameObject* obj = CreateObject("Interactable");

	Transform* tr = new Transform();
	tr->SetPosition({ x, y });

	SpriteRenderer* sprite = new SpriteRenderer(imageKey);
	sprite->SetPivot(50, 90);
	sprite->SetResourceManager(m_resourceManager);

	Interactable* interact = new Interactable(type);

	obj->SetElement(tr, ElementType::Transform);
	obj->SetElement(sprite, ElementType::SpriteRenderer);
	obj->SetElement(interact, ElementType::Interactable);
	obj->Init();
}

void MainScene::CreateItemPickUp(float x, float y, const string& itemId, int count)
{
	const ItemData* itemData = DataManager::GetInstance().FindItem(itemId);
	if (itemData == nullptr) return;

	GameObject* obj = CreateObject("ItemPickUp");

	Transform* tr = new Transform();
	tr->SetPosition({ x, y });

	SpriteRenderer* sprite = new SpriteRenderer(itemData->image);
	sprite->SetPivot(20, 40);
	sprite->SetScale(0.2f);
	sprite->SetResourceManager(m_resourceManager);

	ItemPickUp* pickup = new ItemPickUp(itemId, count);

	obj->SetElement(tr, ElementType::Transform);
	obj->SetElement(sprite, ElementType::SpriteRenderer);
	obj->SetElement(pickup, ElementType::ItemPickUp);
	obj->Init();
}

void MainScene::CheckItemPickUps()
{
	Transform* playerTr = static_cast<Transform*>(m_player->GetGameObject()->GetElement(ElementType::Transform));
	MathEngine::Vector2 playerPos = playerTr->GetPostion();

	vector<GameObject*> toRemove;

	for (GameObject* obj : m_objects)
	{
		ItemPickUp* pickup = static_cast<ItemPickUp*>(obj->GetElement(ElementType::ItemPickUp));
		if (pickup == nullptr) continue;

		Transform* itemTr = static_cast<Transform*>(obj->GetElement(ElementType::Transform));
		MathEngine::Vector2 diff = itemTr->GetPostion() - playerPos;

		if (diff.Magnitude() < 0.6f)
		{
			int added = m_player->GetInventory()->AddItem(pickup->GetItemId(), pickup->GetCount());
			if (added > 0)
			{
				toRemove.push_back(obj);
			}
		}
	}
}

Interactable* MainScene::FindNearByInteractable()
{
	Transform* playerTr = static_cast<Transform*>(m_player->GetGameObject()->GetElement(ElementType::Transform));
	MathEngine::Vector2 playerPos = playerTr->GetPostion();

	for (GameObject* obj : m_objects)
	{
		Interactable* interact = static_cast<Interactable*>(obj->GetElement(ElementType::Interactable));
		if (interact == nullptr) continue;

		Transform* tr = static_cast<Transform*>(obj->GetElement(ElementType::Transform));
		if ((tr->GetPostion() - playerPos).Magnitude() < 1.5f)
			return interact;
	}
	return nullptr;
}

void MainScene::OnInteract(Interactable* target)
{
	m_currentCraftingType = target->GetInteractType();
	m_isCraftingOpen = true;
	m_selectedRecipeIndex = 0;
	//m_lastMessage.clear();
}

void MainScene::LoadMap(const vector<string>& mapData)
{
	for (int y = 0; y < (int)mapData.size(); y++)
	{
		const string& row = mapData[y];
		for (int x = 0; x < (int)row.size(); x++)
		{
			char tile = row[x];

			// 벽은 방향 판별이 필요해서 예외로 별도 처리
			if (tile == '#')
			{
				string wallImage = "Wall_N";
				if (y == 0) wallImage = "Wall_N";
				else if (y == (int)mapData.size() - 1) wallImage = "Wall_S";
				else if (x == 0) wallImage = "Wall_W";
				else if (x == (int)row.size() - 1) wallImage = "Wall_E";

				CreateWall((float)x, (float)y, wallImage);
				continue;
			}

			// 그 외 문자는 등록된 핸들러로 위임
			auto it = m_tileHandlers.find(tile);
			if (it != m_tileHandlers.end())
			{
				it->second((float)x, (float)y);
			}
		}
	}
}
