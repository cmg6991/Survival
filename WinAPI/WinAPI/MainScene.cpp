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
#include "UIManager.h"

#include "Player.h"
#include "GameObject.h"
#include "Transform.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "Interactable.h"
#include "ItemPickUp.h"
#include "Inventory.h"
#include "CampFire.h"
#include "Weapon.h"
#include "ColliderComponent.h"
#include <utility>
#include <memory>

#include "../PhysicsEngine/CircleCollider.h"
#include "../PhysicsEngine/Collider.h"

wstring UTF8ToWString(const string& str)
{
	if (str.empty())
		return L"";

	int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);

	wstring result(sizeNeeded, 0);

	MultiByteToWideChar(
		CP_UTF8,
		0,
		str.c_str(),
		-1,
		result.data(),
		sizeNeeded
	);

	result.pop_back();

	return result;
}

MainScene::MainScene(ResourceManager* resourceManager)
	: Scene("MainScene"), m_tileMap(nullptr),
	m_resourceManager(resourceManager), m_collisionManager(nullptr),
	m_player(nullptr), m_physicsWorld(nullptr)
{
	m_tileMap = new TileMap;
	m_collisionManager = new CollisionManager;
	m_physicsWorld = new PhysicsEngine::PhysicsWorld();
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
	ColliderComponent* collider = new ColliderComponent();
	sprite->SetPivot(230, 370);
	sprite->SetScale(0.3f);
	player->SetTileMap(m_tileMap);
	player->SetCollisionManager(m_collisionManager);

	collider->SetPhysicsWorld(m_physicsWorld);
	collider->SetSyncMode(ColliderSyncMode::TransformDrivesPhysics);

	playerObj->SetElement(tr, ElementType::Transform);
	playerObj->SetElement(player, ElementType::Player);
	playerObj->SetElement(sprite, ElementType::SpriteRenderer);
	playerObj->SetElement(animator, ElementType::Animator);
	playerObj->SetElement(collider, ElementType::Collider);
	sprite->SetResourceManager(m_resourceManager);
	playerObj->Init();
	m_objects.push_back(playerObj);
	m_player = player;

	SaveData data;
	bool hasSave = SaveManager::HasSaveFile();
	if (hasSave)
	{
		SaveManager::Load(data);
		m_collectedItemsIds.insert(data.collectedItemsIds.begin(), data.collectedItemsIds.end());
	}

	RegisterTileHandlers();
	LoadMap(DataManager::GetInstance().GetMap("MainMap")); // 이 시점에 m_collectedItemIds를 참조해서 이미 주운 자리는 스킵

	TimeManager::GetInstance().Init();

	if (hasSave)
	{
		Transform* playerTr = static_cast<Transform*>(m_player->GetGameObject()->GetElement(ElementType::Transform));
		playerTr->SetPosition({ data.playerX, data.playerY });

		TimeManager::GetInstance().SetTime(data.day, data.hour, data.minute);

		m_player->GetInventory()->SetAllItems(data.inventory);
	}

	std::unique_ptr<PhysicsEngine::Collider> circleCollider =
		std::make_unique<PhysicsEngine::CircleCollider>(0.f, 0.f, 0.35f);

	collider->SetCollider(std::move(circleCollider), 1.0f, false);

	UIManager::GetInstance().SetInventory(m_player->GetInventory());
	UIManager::GetInstance().SetResourceManager(m_resourceManager);
	UIManager::GetInstance().SetPlayer(m_player);
	UIManager::GetInstance().SetOnWeaponEquip([this](const string& itemId)
		{
			EquipWeaponToPlayer(itemId);
		});
	UIManager::GetInstance().SetOnWeaponUnequip([this]()
		{
			UnequipWeaponFromPlayer();
		});
}

void MainScene::FixedUpdate()
{
}

void MainScene::Update(float deltaTime)
{
	InputManager::GetInstance().Update();
	UIManager::GetInstance().Update(deltaTime);

	if (InputManager::GetInstance().IsGetKeyDown('I'))
	{
		UIManager::GetInstance().ToggleInventoryWindow();
	}

	if (UIManager::GetInstance().IsInventoryWindowOpen())
	{
		if (InputManager::GetInstance().IsGetKeyDown(VK_LBUTTON))
		{
			MathEngine::Vector2 mousePos =
				InputManager::GetInstance().GetMousePosition();

			UIManager::GetInstance().HandleInventoryClick(
				mousePos.x,
				mousePos.y
			);
		}

		return;
	}
	if (UIManager::GetInstance().IsCraftingOpen())
	{
		string station = InteractTypeToStationString(UIManager::GetInstance().GetCraftingStation());
		vector<RecipeData> recipes = DataManager::GetInstance().GetRecipesByStation(station);

		if (InputManager::GetInstance().IsGetKeyDown(VK_LBUTTON))
		{
			MathEngine::Vector2 mousePos = InputManager::GetInstance().GetMousePosition();
			UIManager::GetInstance().HandleCraftingInventoryClick(mousePos.x, mousePos.y);
		}

		if (InputManager::GetInstance().IsGetKeyDown('W'))
			UIManager::GetInstance().MoveSelection(-1, (int)recipes.size());

		if (InputManager::GetInstance().IsGetKeyDown('S'))
			UIManager::GetInstance().MoveSelection(1, (int)recipes.size());

		if (InputManager::GetInstance().IsGetKeyDown(VK_RETURN) && !recipes.empty())
		{
			const RecipeData& recipe = recipes[UIManager::GetInstance().GetSelectedRecipeIndex()];

			CraftResult result = CraftingManager::Craft(recipe.id, m_player->GetInventory(), m_player->GetWeapon());

			switch (result)
			{
			case CraftResult::Success:
			{
				const ItemData* resultItem = DataManager::GetInstance().FindItem(recipe.resultId);
				bool isWeaponResult = (resultItem != nullptr && resultItem->type == "Weapon");

				if (recipe.isWeaponUpgrade)
				{
					EquipWeaponToPlayer(recipe.resultId, false); // 강화: 재료로 먹힌 자리에 그대로 장착
				}
				else if (isWeaponResult && m_player->GetWeapon() == nullptr)
				{
					EquipWeaponToPlayer(recipe.resultId, true); // 슬롯 비었을 때만 자동장착
				}
				UIManager::GetInstance().ShowMessage(L"제작 성공: " + UTF8ToWString(recipe.resultId));
				break;
			}
			case CraftResult::Failed:
			{
				if (recipe.isWeaponUpgrade)
				{
					// 강화 실패 -> 재료로 쓰인 무기 자체도 파괴
					Weapon* current = m_player->GetWeapon();
					if (current != nullptr)
					{
						DeletePObject(current->GetGameObject());
						m_player->SetWeapon(nullptr);
					}
				}
				UIManager::GetInstance().ShowMessage(L"제작 실패... 재료를 잃었습니다");
				break;
			}
			case CraftResult::None:
			{
				UIManager::GetInstance().ShowMessage(L"재료가 부족합니다");
				break;
			}
			}
		}

		if (InputManager::GetInstance().IsGetKeyDown(VK_ESCAPE))
			UIManager::GetInstance().CloseCrafting();

		return;
	}

	Scene::Update(deltaTime);
	CameraManager::GetInstance().Follow(m_player->GetTransform());
	CheckItemPickUps();

	Interactable* nearby = FindNearByInteractable();
	if (nearby != nullptr)
	{
		if (nearby->GetInteractType() == InteractType::CampFire)
		{
			CampFire* campFire = static_cast<CampFire*>(nearby->GetGameObject()->GetElement(ElementType::CampFire));
			if (campFire->IsLit())
			{
				wchar_t buf[100];
				swprintf_s(buf, L"[E] 요리하기 (남은 시간: %d초)", (int)campFire->GetRemainingTime());
				UIManager::GetInstance().SetInteractionHint(buf);
			}
			else
			{
				UIManager::GetInstance().SetInteractionHint(L"[E] 나무를 넣어 불 붙이기");
			}
		}
		else
		{
			UIManager::GetInstance().SetInteractionHint(L"[E] 상호작용");
		}
	}
	else
	{
		UIManager::GetInstance().ClearInteractionHint();
	}

	if (InputManager::GetInstance().IsGetKeyDown('E'))
	{
		if (nearby != nullptr)
		{
			OnInteract(nearby);
		}
	}

	m_physicsWorld->Step(deltaTime);             // 추가
	m_physicsWorld->DetectCollision(deltaTime);

}

void MainScene::LateUpdate()
{
	//Scene::LateUpdate();
}

void MainScene::PreRender()
{
}

void MainScene::Render(ID2D1DeviceContext* context)
{
	m_tileMap->Render(context, m_resourceManager);

	Scene::Render(context);

	m_collisionManager->RenderDebug(context);
	UIManager::GetInstance().Render(context);
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

	delete m_physicsWorld; // 추가
	m_physicsWorld = nullptr;

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
	data.collectedItemsIds.assign(m_collectedItemsIds.begin(), m_collectedItemsIds.end());

	SaveManager::Save(data);
}

void MainScene::RegisterTileHandlers()
{
	m_tileHandlers['P'] = [this](float x, float y)
		{
			Transform* playerTr = static_cast<Transform*>(m_player->GetGameObject()->GetElement(ElementType::Transform));
			playerTr->SetFloatX(x);
			playerTr->SetFloatY(y);

			// Collider가 이미 붙어있다면 물리 위치도 함께 동기화
			ColliderComponent* col = static_cast<ColliderComponent*>(m_player->GetGameObject()->GetElement(ElementType::Collider));
			if (col && col->GetCollider())
			{
				col->GetCollider()->center = { x, y };
			}
		};

	m_tileHandlers['C'] = [this](float x, float y)
		{
			CreateInteractable(x, y, InteractType::CampFire, "CampFire");
		};

	// 작업대
	m_tileHandlers['T'] = [this](float x, float y)
		{
			CreateInteractable(x, y, InteractType::WorkTable, "WorkTable");
		};
	// 앞으로 몬스터/아이템/모닥불 등이 생기면 여기 계속 추가하면 됨
	// 예시 (해당 클래스들 만드신 뒤 주석 해제):
	// m_tileHandlers['M'] = [this](float x, float y) { CreateMonster(x, y); };
	m_tileHandlers['w'] = [this](float x, float y) { CreateItemPickUp(x, y, "Item_Wood", 1); };
	m_tileHandlers['r'] = [this](float x, float y) { CreateItemPickUp(x, y, "Item_Stone", 1); };
	m_tileHandlers['s'] = [this](float x, float y) { CreateItemPickUp(x, y, "Item_Sword", 1); };
	//m_tileHandlers['m'] = [this](float x, float y) { CreateItemPickUp(x, y, "Meat", 1); };
}

void MainScene::CreateWall(float x, float y, const string& imageName)
{
	GameObject* wallObj = CreateObject("Wall");

	Transform* tr = new Transform();
	tr->SetPosition({ x, y });

	SpriteRenderer* sprite = new SpriteRenderer(imageName);
	//sprite->SetPivot(47, 112);
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
	//sprite->SetPivot(50, 90);
	sprite->SetResourceManager(m_resourceManager);
	sprite->SetScale(0.5f);
	Interactable* interact = new Interactable(type);

	ColliderComponent* collider = new ColliderComponent();
	collider->SetPhysicsWorld(m_physicsWorld);
	collider->SetSyncMode(ColliderSyncMode::TransformDrivesPhysics);

	obj->SetElement(tr, ElementType::Transform);
	obj->SetElement(sprite, ElementType::SpriteRenderer);
	obj->SetElement(interact, ElementType::Interactable);
	obj->SetElement(collider, ElementType::Collider);
	if (type == InteractType::CampFire)
	{
		CampFire* campFire = new CampFire();
		obj->SetElement(campFire, ElementType::CampFire);
	}

	obj->Init();

	auto circleCollider = std::make_unique<PhysicsEngine::CircleCollider>(0.f, 0.f, 0.5f);
	collider->SetCollider(std::move(circleCollider), 1.0f, true);
}

void MainScene::CreateItemPickUp(float x, float y, const string& itemId, int count)
{
	string posId = MakeItemPositionId(x, y);

	if (m_collectedItemsIds.find(posId) != m_collectedItemsIds.end())
	{
		return; // 이미 주운 자리면 생성하지 않음
	}

	const ItemData* itemData = DataManager::GetInstance().FindItem(itemId);
	if (itemData == nullptr) return;

	GameObject* obj = CreateObject("ItemPickUp");

	Transform* tr = new Transform();
	tr->SetPosition({ x, y });

	SpriteRenderer* sprite = new SpriteRenderer(itemData->image);
	//sprite->SetPivot(20, 40);
	//sprite->SetScale(0.5f);
	sprite->SetResourceManager(m_resourceManager);

	ItemPickUp* pickup = new ItemPickUp(itemId, count);
	pickup->SetPositionId(posId);

	obj->SetElement(tr, ElementType::Transform);
	obj->SetElement(sprite, ElementType::SpriteRenderer);
	obj->SetElement(pickup, ElementType::ItemPickUp);
	obj->Init();
}

void MainScene::EquipWeaponToPlayer(const string& weaponId, bool returnInven)
{
	Weapon* current = m_player->GetWeapon();
	if (current != nullptr)
	{
		if (returnInven)
		{
			m_player->GetInventory()->AddItem(current->GetWeaponId(), 1);
		}
		DeletePObject(current->GetGameObject());
		DeletePObject(current->GetGameObject());
	}

	if (m_player->GetInventory()->HasEnough(weaponId, 1))
	{
		m_player->GetInventory()->RemoveItem(weaponId, 1);
	}

	GameObject* weaponObj = CreateObject("Weapon");

	Transform* weaponTr = new Transform();
	SpriteRenderer* weaponSprite = new SpriteRenderer(weaponId);
	weaponSprite->SetResourceManager(m_resourceManager);
	Weapon* weapon = new Weapon(weaponId);

	weaponObj->SetElement(weaponTr, ElementType::Transform);
	weaponObj->SetElement(weaponSprite, ElementType::SpriteRenderer);
	weaponObj->SetElement(weapon, ElementType::Weapon);
	weaponObj->Init(); // 여기서 Weapon::Init()이 호출되면서 m_transform, m_sprite 자동 세팅됨

	m_player->SetWeapon(weapon);
	//m_player->EquipWeapon(weaponId);
	//weapon->SetEquipped(true);
}

void MainScene::UnequipWeaponFromPlayer()
{
	Weapon* current = m_player->GetWeapon();
	if (current == nullptr) return;

	m_player->GetInventory()->AddItem(current->GetWeaponId(), 1);
	DeletePObject(current->GetGameObject());
	m_player->SetWeapon(nullptr);
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
				m_collectedItemsIds.insert(pickup->GetPositionId());
				toRemove.push_back(obj);
			}
		}
	}
	for (GameObject* obj : toRemove)
	{
		DeletePObject(obj);
	}
}

string MainScene::MakeItemPositionId(float x, float y)
{
	return "item_" + to_string((int)x) + "_" + to_string((int)y);
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
	if (target->GetInteractType() == InteractType::CampFire)
	{
		GameObject* obj = target->GetGameObject();
		CampFire* campFire = static_cast<CampFire*>(obj->GetElement(ElementType::CampFire));

		if (!campFire->IsLit())
		{
			// 불이 꺼져 있으면 나무를 넣어서 불붙이기만 하고, 요리 UI는 안 염
			int woodCount = m_player->GetInventory()->GetItemCount("Item_Wood");

			if (woodCount <= 0)
			{
				UIManager::GetInstance().ShowMessage(L"나무가 없습니다");
				return;
			}

			m_player->GetInventory()->RemoveItem("Item_Wood", 1);
			campFire->AddFuel(1);
			UIManager::GetInstance().ShowMessage(L"모닥불에 불을 붙였습니다");
			return;
		}

		UIManager::GetInstance().OpenCrafting(InteractType::CampFire);
		return;
	}

	UIManager::GetInstance().OpenCrafting(target->GetInteractType());
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


string MainScene::InteractTypeToStationString(InteractType type)
{
	switch (type)
	{
	case InteractType::CampFire:  return "CampFire";
	case InteractType::WorkTable: return "WorkTable";
	}
	return "";
}
