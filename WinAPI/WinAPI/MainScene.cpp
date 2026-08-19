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
#include "EnvironmentManager.h"
#include "TileManager.h"
#include "FlowFieldManager.h"
#include "MonsterSpawner.h"
#include "ObjectSpawner.h"
#include "SceneManager.h"

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
#include "Bullet.h"
#include "ColliderComponent.h"
#include "Monster.h"
#include "AttackHitBox.h"
#include "Fire.h"
#include <utility>
#include <memory>
#include "ResourceNode.h"

#include "MiniMap.h"

#include "../PhysicsEngine/CircleCollider.h"
#include "../PhysicsEngine/RectangleCollider.h"
#include "../PhysicsEngine/Collider.h"
#include "../PhysicsEngine/Positionsolver.h"
#include "../PhysicsEngine/ImpulseSolver.h"

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

MainScene::MainScene(ResourceManager* resourceManager, SceneManager* sceneManager)
	: Scene("MainScene"), m_tileMap(nullptr),
	m_resourceManager(resourceManager), m_collisionManager(nullptr),
	m_player(nullptr), m_physicsWorld(nullptr), m_monsterSpawner(nullptr), m_miniMap(nullptr),m_objectSpawner(nullptr),
	m_sceneManager(sceneManager)
{
	m_tileMap = new TileMap;
	m_collisionManager = new CollisionManager;
	m_physicsWorld = new PhysicsEngine::PhysicsWorld();
	m_physicsWorld->AddSolver(new PhysicsEngine::ImpulseSolver());
	m_physicsWorld->AddSolver(new PhysicsEngine::PositionSolver());
	m_monsterSpawner = new MonsterSpawner();
	m_objectSpawner = new ObjectSpawner();
}

MainScene::~MainScene()
{
}

void MainScene::Init()
{
	DWORD startTime = GetTickCount64();
	/*m_resourceManager->Init();
	for (const ImageData& img : DataManager::GetInstance().GetImageList())
	{
		m_resourceManager->AddImage(img.keyString, img.path);
	}*/

	m_tileMap->Init();
	m_objectSpawner->Init(
		this,
		m_resourceManager,
		m_tileMap
	);
	m_tileMap->SetObjectSpawner(m_objectSpawner);

	GameObject* playerObj = new GameObject("Player");

	Transform* tr = new Transform();

	Player* player = new Player();
	SpriteRenderer* sprite = new SpriteRenderer("Player_Idle");
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
	SaveData data;

	bool isLoadGame = m_sceneManager->GetGameStartType() == GameStartType::LoadGame;

	int saveSlot = m_sceneManager->GetSaveSlot();
	bool hasSave = false;

	if (isLoadGame)
	{
		hasSave = SaveManager::HasSaveSlot(saveSlot);

		if (hasSave)
		{
			SaveManager::LoadSlot(data, saveSlot);
			m_collectedItemsIds.insert(data.collectedItemsIds.begin(), data.collectedItemsIds.end());
			tr->SetPosition({ data.playerX, data.playerY });
			wchar_t buf[256];
			swprintf_s(buf, L"[LOAD DEBUG] isLoadGame=%d saveSlot=%d hasSave=%d loadedX=%.2f loadedY=%.2f\n",
				isLoadGame, saveSlot, hasSave, data.playerX, data.playerY);
			OutputDebugStringW(buf);
		}
	}
	playerObj->Init();
	m_objects.push_back(playerObj);
	m_player = player;
	EnvironmentManager::GetInstance().Init(m_player);
	std::unique_ptr<PhysicsEngine::Collider> circleCollider =
		std::make_unique<PhysicsEngine::CircleCollider>(0.f, 0.f, 0.5f);

	collider->SetCollider(std::move(circleCollider), 1.0f, false);

	RegisterTileHandlers();
	const vector<string>& mapData = DataManager::GetInstance().GetMap("MainMap");
	LoadMap(mapData);
	m_tileMap->LoadFromMapData(mapData);

	TimeManager::GetInstance().Init();

	/*if (hasSave)
	{
		Transform* playerTr = static_cast<Transform*>(m_player->GetGameObject()->GetElement(ElementType::Transform));
		playerTr->SetPosition({ data.playerX, data.playerY });

		TimeManager::GetInstance().SetTime(data.day, data.hour, data.minute);

		me_player->GetInventory()->SetAllItems(data.inventory);
	}*/
	if (isLoadGame && hasSave)
	{
		//Transform* playerTr =static_cast<Transform*>(m_player->GetGameObject()->GetElement(ElementType::Transform));

		//playerTr->SetPosition({data.playerX,data.playerY});

		TimeManager::GetInstance().SetTime(
			data.day,
			data.hour,
			data.minute
		);

		m_player->GetInventory()->SetAllItems(data.inventory);
		m_playTime = static_cast<float>(data.playTimeSeconds);
		m_player->SetGlowRing(data.hasGlowRing);
		if (!data.equippedWeaponId.empty())
			EquipWeaponToPlayer(data.equippedWeaponId);

		LoadEquipShield(data.equippedShieldId);
	}

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
	UIManager::GetInstance().SetOnShieldEquip([this](const string& itemId)
		{
			EquipShieldToPlayer(itemId);
		});
	UIManager::GetInstance().SetOnShieldUnequip([this]()
		{
			UnequipShieldFromPlayer();
		});
	UIManager::GetInstance().SetOnItemUse([this](const string& itemId)
		{
			UseItem(itemId);
		});
	UIManager::GetInstance().SetOnSave([this](){SaveGame();});
	UIManager::GetInstance().SetOnExit([this](){PostQuitMessage(0);});
	UIManager::GetInstance().SetOnTitle([this](){m_sceneManager->LoadScene("Title");});
	m_flowField.Init(m_collisionManager, 100, 100);
	m_monsterSpawner->Init(m_collisionManager, 100, 100);
	m_monsterSpawner->SetSpawnPool({ "Monster2", "Monster", "Monster3" });  // ★ 추가

	m_monsterSpawner->SetOnSpawnRequest([this](const string& monsterId, float x, float y)   // ★ 시그니처 변경
		{
			CreateMonster(monsterId, x, y);
		});
	m_monsterSpawner->SetOnClearAllMonsters([this]()
		{
			ClearAllMonsters();
		});
	m_miniMap = new MiniMap();

	m_miniMap->Init(
		m_player,
		m_resourceManager);
	Transform* playerTr = static_cast<Transform*>(m_player->GetGameObject()->GetElement(ElementType::Transform));
	CameraManager::GetInstance().Follow(tr);
	const int BULLET_POOL_SIZE = 20;

	for (int i = 0; i < BULLET_POOL_SIZE; ++i)
	{
		GameObject* bullet = AcquireBullet();

		if (bullet != nullptr)
		{
			m_bulletPool.push_back(bullet);
		}
	}
}

void MainScene::FixedUpdate()
{
	Scene::FixedUpdate();
}

void MainScene::Update(float deltaTime)
{
	//InputManager::GetInstance().Update();
	TimeManager::GetInstance().Update(deltaTime);
	UIManager::GetInstance().Update(deltaTime);
	m_playTime += deltaTime;

	if (InputManager::GetInstance().IsGetKeyDown(VK_ESCAPE))
	{
		if (UIManager::GetInstance().IsCraftingOpen())
		{
			UIManager::GetInstance().CloseCrafting();
			return;
		}
		if (UIManager::GetInstance().IsPauseMenuOpen())
		{
			UIManager::GetInstance().TogglePauseMenu();
			return;
		}
		UIManager::GetInstance().TogglePauseMenu();
		return;
	}

	// Pause 상태라면 게임 업데이트 정지
	if (UIManager::GetInstance().IsPauseMenuOpen())
	{
		if (InputManager::GetInstance().IsGetKeyDown(VK_LBUTTON))
		{
			MathEngine::Vector2 mousePos = InputManager::GetInstance().GetMousePosition();

			UIManager::GetInstance().HandlePauseMenuClick(mousePos.x, mousePos.y);
		}
		return;
	}

	if (InputManager::GetInstance().IsGetKeyDown('I'))
	{
		UIManager::GetInstance().ToggleInventoryWindow();
	}

	if (UIManager::GetInstance().IsInventoryWindowOpen())
	{
		if (InputManager::GetInstance().IsGetKeyDown(VK_LBUTTON))
		{
			MathEngine::Vector2 mousePos =InputManager::GetInstance().GetMousePosition();

			UIManager::GetInstance().HandleInventoryClick(mousePos.x,mousePos.y);
		}
	}

	if (UIManager::GetInstance().IsCraftingOpen())
	{
		string station = InteractTypeToStationString(UIManager::GetInstance().GetCraftingStation());
		vector<RecipeData> recipes = DataManager::GetInstance().GetRecipesByStation(station);

		if (InputManager::GetInstance().IsGetKeyDown(VK_LBUTTON))
		{
			MathEngine::Vector2 mousePos = InputManager::GetInstance().GetMousePosition();
			//UIManager::GetInstance().HandleCraftingInventoryClick(mousePos.x, mousePos.y);

			bool selected = UIManager::GetInstance().HandleCraftingRecipeClick(mousePos.x, mousePos.y);
			if (selected)
			{
				const RecipeData& recipe =recipes[UIManager::GetInstance().GetSelectedRecipeIndex()];

				CraftResult result =
					CraftingManager::Craft(
						recipe.id,
						m_player->GetInventory(),
						m_player->GetWeapon()
					);

				switch (result)
				{
				case CraftResult::Success:
				{
					if (recipe.resultId == "Item_GlowRing")
					{
						if (m_player != nullptr)
						{
							m_player->SetGlowRing(true);

							UIManager::GetInstance().ShowMessage(
								L"발광석 반지를 제작하고 장착했습니다."
							);
						}

						break;
					}

					const ItemData* resultItem =DataManager::GetInstance().FindItem(recipe.resultId);
					bool isWeaponResult =resultItem != nullptr &&resultItem->type == "Weapon";
					bool isShieldResult = resultItem != nullptr && resultItem->type == "Shield";
					if (isShieldResult)
					{
						EquipShieldToPlayer(recipe.resultId);
					}
					else if (recipe.isWeaponUpgrade)
					{
						EquipWeaponToPlayer(recipe.resultId, false);
					}
					else if (isWeaponResult /*&& m_player->GetWeapon() == nullptr*/)
					{
						EquipWeaponToPlayer(recipe.resultId, true);
					}

					UIManager::GetInstance().ShowMessage(L"제작 성공: " + UTF8ToWString(resultItem->name));
					break;
				}
				case CraftResult::Failed:
				{
					if (recipe.isWeaponUpgrade)
					{
						Weapon* current =m_player->GetWeapon();

						if (current)
						{
							DeletePObject(current->GetGameObject());
							m_player->SetWeapon(nullptr);
						}
					}

					UIManager::GetInstance().ShowMessage(L"제작 실패...");
					break;
				}

				case CraftResult::None:
				{
					UIManager::GetInstance().ShowMessage(L"재료 부족");
					break;
				}
				}
			}
		}

		//if (InputManager::GetInstance().IsGetKeyDown('W'))
		//	UIManager::GetInstance().MoveSelection(-1, (int)recipes.size());

		//if (InputManager::GetInstance().IsGetKeyDown('S'))
		//	UIManager::GetInstance().MoveSelection(1, (int)recipes.size());

		//if (InputManager::GetInstance().IsGetKeyDown(VK_RETURN) && !recipes.empty())
		//{
		//	const RecipeData& recipe = recipes[UIManager::GetInstance().GetSelectedRecipeIndex()];

		//	CraftResult result = CraftingManager::Craft(recipe.id, m_player->GetInventory(), m_player->GetWeapon());

		//	switch (result)
		//	{
		//	case CraftResult::Success:
		//	{
		//		const ItemData* resultItem = DataManager::GetInstance().FindItem(recipe.resultId);
		//		bool isWeaponResult = (resultItem != nullptr && resultItem->type == "Weapon");

		//		if (recipe.isWeaponUpgrade)
		//		{
		//			EquipWeaponToPlayer(recipe.resultId, false); // 강화: 재료로 먹힌 자리에 그대로 장착
		//		}
		//		else if (isWeaponResult && m_player->GetWeapon() == nullptr)
		//		{
		//			EquipWeaponToPlayer(recipe.resultId, true); // 슬롯 비었을 때만 자동장착
		//		}
		//		UIManager::GetInstance().ShowMessage(L"제작 성공: " + UTF8ToWString(recipe.resultId));
		//		break;
		//	}
		//	case CraftResult::Failed:
		//	{
		//		if (recipe.isWeaponUpgrade)
		//		{
		//			// 강화 실패 -> 재료로 쓰인 무기 자체도 파괴
		//			Weapon* current = m_player->GetWeapon();
		//			if (current != nullptr)
		//			{
		//				DeletePObject(current->GetGameObject());
		//				m_player->SetWeapon(nullptr);
		//			}
		//		}
		//		UIManager::GetInstance().ShowMessage(L"제작 실패... 재료를 잃었습니다");
		//		break;
		//	}
		//	case CraftResult::None:
		//	{
		//		UIManager::GetInstance().ShowMessage(L"재료가 부족합니다");
		//		break;
		//	}
		//	}
		//}

		return;
	}

	UpdateMonsterSeparation();
	Scene::Update(deltaTime);
	CameraManager::GetInstance().Follow(m_player->GetTransform());
	CheckItemPickUps();

	m_monsterSpawner->Update(deltaTime, m_player->GetTransform()->GetPostion());

	Transform* playerTr = m_player->GetTransform();
	int playerTileX = (int)round(playerTr->GetPostion().x);
	int playerTileY = (int)round(playerTr->GetPostion().y);

	/*if (playerTileX != m_lastPlayerTileX || playerTileY != m_lastPlayerTileY)
	{
		m_flowField.Recompute(playerTileX, playerTileY);
		m_lastPlayerTileX = playerTileX;
		m_lastPlayerTileY = playerTileY;
	}*/

	m_flowFieldRecomputeTimer -= deltaTime;
	if (m_flowFieldRecomputeTimer <= 0.0f)
	{
		if (playerTileX != m_lastPlayerTileX || playerTileY != m_lastPlayerTileY)
		{
			m_flowField.Recompute(playerTileX, playerTileY);
			m_lastPlayerTileX = playerTileX;
			m_lastPlayerTileY = playerTileY;
		}
		m_flowFieldRecomputeTimer = m_flowFieldRecomputeInterval;
	}
	Interactable* nearby = FindNearByInteractable();
	if (nearby != nullptr)
	{
		if (nearby->GetInteractType() == InteractType::CampFire)
		{
			CampFire* campFire = static_cast<CampFire*>(nearby->GetGameObject()->GetElement(ElementType::CampFire));
			if (campFire->IsLit())
			{
				wchar_t buf[100];
				swprintf_s(buf, L"요리하기 (남은 시간: %d초)", (int)campFire->GetRemainingTime());
				UIManager::GetInstance().SetInteractionHint(buf);
			}
			else
			{
				UIManager::GetInstance().SetInteractionHint(L"나무를 넣어 불 붙이기");
			}
		}
		else if (nearby->GetInteractType() == InteractType::Tree ||
			nearby->GetInteractType() == InteractType::Rock)
		{
			UIManager::GetInstance().SetInteractionHint(L"채집하기");
		}
		else
		{
			UIManager::GetInstance().SetInteractionHint(L"상호작용");
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

	CheckBullets();
	CheckMonsters();
	CheckAttackHitBoxes();

	EnvironmentManager::GetInstance().Update(deltaTime);
}

void MainScene::LateUpdate()
{
	Scene::LateUpdate();
}

void MainScene::PreRender()
{
}

void MainScene::Render(ID2D1DeviceContext* context)
{
	m_tileMap->Render(context, m_resourceManager);

	Scene::Render(context);
	m_collisionManager->RenderDebug(context);

	Weapon* weapon = m_player->GetWeapon();
	if (weapon != nullptr && weapon->GetWeaponType() == WeaponType::Ranged)
	{
		RenderAimLine(context);
	}

	EnvironmentManager::GetInstance().Render(context);
	UIManager::GetInstance().Render(context);
	if (m_miniMap)
		m_miniMap->Render(context,m_objects);
}

void MainScene::PostRender(ID2D1DeviceContext* context)
{
}

void MainScene::Release()
{
	OutputDebugStringW(L"========== MainScene::Release() ==========\n");

	if (m_player != nullptr)
	{
		SaveGame();
	}

	delete m_tileMap;
	m_tileMap = nullptr;

	delete m_collisionManager;
	m_collisionManager = nullptr;

	delete m_monsterSpawner;
	m_monsterSpawner = nullptr;

	delete m_objectSpawner;
	m_objectSpawner = nullptr;

	delete m_miniMap;
	m_miniMap = nullptr;

	/*for (GameObject* obj : m_bulletPool)
	{
		delete obj;
	}

	for (auto& pair : m_monsterPool)
	{
		for (GameObject* obj : pair.second)
		{
			delete obj;
		}

		pair.second.clear();
	}*/
	m_monsterPool.clear();

	m_bulletPool.clear();

	Scene::Release();

	delete m_physicsWorld; // 추가
	m_physicsWorld = nullptr;
	OutputDebugStringW(
		L"========== MainScene::Release END ==========\n"
	);
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

	data.playTimeSeconds = static_cast<int>(m_playTime);
	data.hasGlowRing = m_player->HasGlowRing();

	if (m_player->GetWeapon() != nullptr)
		data.equippedWeaponId = m_player->GetWeapon()->GetWeaponId();
	else
		data.equippedWeaponId = "";

	data.equippedShieldId = m_player->GetEquippedShieldId();

	int saveSlot = m_sceneManager->GetSaveSlot();
	if (saveSlot < 1 || saveSlot > 3)
	{
		OutputDebugStringW(L"SaveGame 실패: 잘못된 슬롯 번호\n");
		return;
	}

	bool result =SaveManager::SaveSlot(data,saveSlot);

	if (result)
	{
		wchar_t buf[128];
		swprintf_s(buf,L"게임 저장 성공 - Slot %d\n",saveSlot);
		OutputDebugStringW(buf);
	}
	else
	{
		OutputDebugStringW(L"게임 저장 실패\n");
	}
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

				PhysicsEngine::Object* physObj = col->GetPhysicsObject();
				if (physObj != nullptr)
				{
					physObj->position = { x, y };
					physObj->velocity = MathEngine::Vector2(0.f, 0.f);
				}
			}
		};

	m_tileHandlers['C'] = [this](float x, float y)
		{
			CreateInteractableFire(x, y, InteractType::CampFire, "CampFire");
		};

	// 작업대
	m_tileHandlers['T'] = [this](float x, float y)
		{
			CreateInteractableWorkTable(x, y, InteractType::WorkTable, "WorkTable");
		};
	// 앞으로 몬스터/아이템/모닥불 등이 생기면 여기 계속 추가하면 됨
	// 예시 (해당 클래스들 만드신 뒤 주석 해제):
	m_tileHandlers['M'] = [this](float x, float y) { CreateMonster("Monster", x, y); };
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
	sprite->SetResourceManager(m_resourceManager);

	wallObj->SetElement(tr, ElementType::Transform);
	wallObj->SetElement(sprite, ElementType::SpriteRenderer);

	wallObj->Init();

	m_collisionManager->SetBlocked(x, y, true);
}

void MainScene::CreateInteractableWorkTable(float x, float y, InteractType type, const string& imageKey)
{
	GameObject* obj = CreateObject("WorkTable");

	Transform* tr = new Transform();
	tr->SetPosition({ x, y });

	SpriteRenderer* sprite = new SpriteRenderer(imageKey);
	sprite->SetPivot(50, 50);
	sprite->SetResourceManager(m_resourceManager);
	sprite->SetScale(2.f);
	Interactable* interact = new Interactable(type);

	ColliderComponent* collider = new ColliderComponent();
	collider->SetPhysicsWorld(m_physicsWorld);
	collider->SetSyncMode(ColliderSyncMode::PhysicsDrivesTransform);

	obj->SetElement(tr, ElementType::Transform);
	obj->SetElement(sprite, ElementType::SpriteRenderer);
	obj->SetElement(interact, ElementType::Interactable);
	obj->SetElement(collider, ElementType::Collider);
	obj->Init();

	auto rectCollider = std::make_unique<PhysicsEngine::RectangleCollider>(0.f, 0.f, 1.5f,1.f);
	collider->SetCollider(std::move(rectCollider), 1.0f, true);
}

void MainScene::CreateInteractableFire(float x, float y, InteractType type, const string& imageKey)
{
	GameObject* obj = CreateObject("CampFire");

	Transform* tr = new Transform();
	tr->SetPosition({ x, y });

	SpriteRenderer* sprite = new SpriteRenderer(imageKey);
	sprite->SetPivot(128,300);
	sprite->SetResourceManager(m_resourceManager);
	sprite->SetScale(0.5f);
	Interactable* interact = new Interactable(type);

	ColliderComponent* collider = new ColliderComponent();
	collider->SetPhysicsWorld(m_physicsWorld);
	collider->SetSyncMode(ColliderSyncMode::PhysicsDrivesTransform);

	obj->SetElement(tr, ElementType::Transform);
	obj->SetElement(sprite, ElementType::SpriteRenderer);
	obj->SetElement(interact, ElementType::Interactable);
	obj->SetElement(collider, ElementType::Collider);
	if (type == InteractType::CampFire)
	{
		CampFire* campFire = new CampFire();
		obj->SetElement(campFire, ElementType::CampFire);
		GameObject* fireObj = CreateObject("Fire");

		Transform* fireTr = new Transform();
		fireTr->SetPosition({ x+0.5f, y + 0.5f });
		SpriteRenderer* fireSprite = new SpriteRenderer("Fire");
		fireSprite->SetPivot(16, 32);
		fireSprite->SetResourceManager(m_resourceManager);
		fireSprite->SetScale(4.f);

		Animator* fireAnimator = new Animator();
		Fire* fire = new Fire();
		fireObj->SetElement(fireTr, ElementType::Transform);
		fireObj->SetElement(fireSprite, ElementType::SpriteRenderer);
		fireObj->SetElement(fireAnimator, ElementType::Animator);
		fireObj->SetElement(fire, ElementType::Fire);
		// Fire 초기화
		fireObj->Init();

		// CampFire와 Fire 연결
		campFire->SetFire(fire);
	}

	obj->Init();

	auto rectCollider = std::make_unique<PhysicsEngine::RectangleCollider>(0.5f, 0.5f, 1.f, 1.f);
	collider->SetCollider(std::move(rectCollider), 1.0f, true);
}

void MainScene::CreateItemPickUp(float x, float y, const string& itemId, int count, bool trackAsCollected)
{
	string posId = MakeItemPositionId(x, y);

	//if (m_collectedItemsIds.find(posId) != m_collectedItemsIds.end())
	//{
	//	return; // 이미 주운 자리면 생성하지 않음
	//}

	if (trackAsCollected)
	{
		posId = MakeItemPositionId(x, y);
		if (m_collectedItemsIds.find(posId) != m_collectedItemsIds.end())
		{
			return; // 이미 주운 자리면 생성하지 않음
		}
	}
	else
	{
		// 알처럼 반복 드랍되는 아이템은 매번 유니크한 posId 부여 (dedup 대상 아님)
		posId = MakeItemPositionId(x, y) + "_" + to_string(m_transientPickupCounter++);
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

void MainScene::CreateBullet(const MathEngine::Vector2& startPos, const MathEngine::Vector2& dir, int damage, float speed, float range)
{
	//GameObject* obj = CreateObject("Bullet");
	//Transform* tr = new Transform();
	//tr->SetPosition(startPos);

	//Bullet* bullet = new Bullet(dir, speed, range, damage);
	//bullet->SetCollisionManager(m_collisionManager);

	//SpriteRenderer* sprite = new SpriteRenderer("Bullet");
	//sprite->SetResourceManager(m_resourceManager);
	//sprite->SetScale(5.f);

	//ColliderComponent* collider = new ColliderComponent();           
	//collider->SetPhysicsWorld(m_physicsWorld);                       
	//collider->SetSyncMode(ColliderSyncMode::TransformDrivesPhysics); 

	//obj->SetElement(tr, ElementType::Transform);
	//obj->SetElement(bullet, ElementType::Bullet);
	//obj->SetElement(sprite, ElementType::SpriteRenderer);
	//obj->SetElement(collider, ElementType::Collider);             
	//obj->Init();

	//auto circleCollider = std::make_unique<PhysicsEngine::CircleCollider>(0.f, 0.f, 0.15f);  // ★ 추가
	//collider->SetCollider(std::move(circleCollider), 0.1f, false);                            // ★ 추가

	//collider->SetOnCollisionEnter([this, bullet](GameObject* other)  
	//	{
	//		Monster* monster = static_cast<Monster*>(other->GetElement(ElementType::Monster));
	//		if (monster != nullptr && !monster->IsDead())
	//		{
	//			monster->TakeDamage(bullet->GetDamage());
	//			bullet->Kill();
	//		}
	//	});

	GameObject* obj = AcquireBullet();

	Transform* tr = static_cast<Transform*>(obj->GetElement(ElementType::Transform));
	tr->SetPosition(startPos);

	Bullet* bullet = static_cast<Bullet*>(obj->GetElement(ElementType::Bullet));
	bullet->Reset(dir, speed, range, damage);

	ColliderComponent* collider = static_cast<ColliderComponent*>(obj->GetElement(ElementType::Collider));
	if (collider != nullptr)
	{
		collider->SetEnabled(true);   // 다시 충돌 검사 대상에 포함
		//if (collider->GetCollider() != nullptr)
		//{
		//	collider->GetCollider()->center = startPos;   // 콜라이더 위치도 즉시 갱신
		//}
		PhysicsEngine::Object* physObj = collider->GetPhysicsObject();
		if (physObj != nullptr)
		{
			physObj->position = startPos;           
			physObj->velocity = MathEngine::Vector2(0.f, 0.f);  
			physObj->collider->center = startPos;
		}
	}

	obj->SetActive(true);   // 다시 Update/Render 시작
}

void MainScene::CreateMonster(const string& monsterId, float x, float y)
{
	//GameObject* obj = CreateObject("Monster");

	//Transform* tr = new Transform();
	//tr->SetPosition({ x, y });

	//SpriteRenderer* sprite = new SpriteRenderer("Monster"); // 이미지 키 등록 필요
	//sprite->SetResourceManager(m_resourceManager);

	//sprite->SetPivot(23, 30);

	//Animator* animator = new Animator();
	//Monster* monster = new Monster(health);

	//ColliderComponent* collider = new ColliderComponent();
	//collider->SetPhysicsWorld(m_physicsWorld);
	//collider->SetSyncMode(ColliderSyncMode::TransformDrivesPhysics);

	//obj->SetElement(tr, ElementType::Transform);
	//obj->SetElement(sprite, ElementType::SpriteRenderer);
	//obj->SetElement(animator, ElementType::Animator);
	//obj->SetElement(monster, ElementType::Monster);
	//obj->SetElement(collider, ElementType::Collider);
	//obj->Init();

	//auto circleCollider = std::make_unique<PhysicsEngine::CircleCollider>(0.f, 0.f, 0.4f);
	//collider->SetCollider(std::move(circleCollider), 1.0f, false);
	//collider->SetTrigger(true);

	//monster->SetTarget(m_player->GetTransform());
	//monster->SetCollisionManager(m_collisionManager);
	//monster->SetFlowField(&m_flowField);
	//monster->SetStats(1.5f, 5);   // 이동속도, 접촉 데미지

	//ColliderComponent* playerCollider =
	//	static_cast<ColliderComponent*>(m_player->GetGameObject()->GetElement(ElementType::Collider));
	//monster->SetTargetCollider(playerCollider);

	//auto contactDamage = [this, monster](GameObject* other)
	//	{
	//		if (other == m_player->GetGameObject() && monster->CanDealDamage())
	//		{
	//			m_player->TakeDamage(monster->GetContactDamage());
	//			monster->ResetDamageCooldown();
	//		}
	//	};

	//collider->SetOnCollisionEnter(contactDamage);
	//collider->SetOnCollisionStay(contactDamage);
	const MonsterData* data = DataManager::GetInstance().FindMonster(monsterId);
	if (data == nullptr) return;
	// 몬스터가 생성될 타일
	int tileX = (int)round(x);
	int tileY = (int)round(y);

	// 물이면 스폰하지 않음
	if (m_tileMap != nullptr &&
		m_tileMap->IsWater(tileX, tileY))
	{
		return;
	}

	GameObject* obj = AcquireMonster(monsterId);

	Transform* tr = static_cast<Transform*>(obj->GetElement(ElementType::Transform));
	tr->SetPosition({ x, y });

	//날짜에 따른 몬스터 체력이랑 데미지가 점점 올라감 
	int day = TimeManager::GetInstance().GetDay();
	int dayFactor = max(0, day - 1);
	float healthScale = 1.0f + dayFactor * 0.12f;
	float damageScale = 1.0f + dayFactor * 0.08f;

	Monster* monster = static_cast<Monster*>(obj->GetElement(ElementType::Monster));
	monster->Reset(data->health, data->cellWidth, data->cellHeight, data->animColumn);
	monster->SetStats(data->moveSpeed, data->contactDamage);
	monster->SetTarget(m_player->GetTransform());
	monster->SetTileMap(m_tileMap);
	ColliderComponent* collider = static_cast<ColliderComponent*>(obj->GetElement(ElementType::Collider));
	if (collider != nullptr)
	{
		collider->SetEnabled(true);
		PhysicsEngine::Object* physObj = collider->GetPhysicsObject();
		if (physObj != nullptr)
		{
			physObj->position = { x, y };
			physObj->velocity = MathEngine::Vector2(0.f, 0.f);
			physObj->collider->center = { x, y };
		}
	}

	obj->SetActive(true);
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

	// ★ 아이템 데이터 기반으로 원거리/근접 자동 판별
	const ItemData* itemData = DataManager::GetInstance().FindItem(weaponId);

	weaponSprite = nullptr;
	if (itemData != nullptr && itemData->weaponType == "Gun")
	{
		weapon->SetWeaponType(WeaponType::Ranged);
		weapon->SetBulletStat(15.0f, 10.0f, 15); // 속도, 사거리, 데미지 (추후 itemData에서 읽어도 됨)
		weapon->SetOnFire([this](const MathEngine::Vector2& startPos, const MathEngine::Vector2& dir,
			int damage, float speed, float range)
			{
				CreateBullet(startPos, dir, damage, speed, range);
			});
		weaponSprite = new SpriteRenderer(weaponId);
		weaponSprite->SetResourceManager(m_resourceManager);
		weaponSprite->SetScale(1.f);
		weaponSprite->SetPivot(50.0f, 33.5f);
	}
	else if (itemData != nullptr && itemData->weaponType == "Fishing")
	{
		weapon->SetWeaponType(WeaponType::Fishing);
		// 낚싯대 전용 pivot/scale 조정
	}
	else
	{
		weapon->SetWeaponType(WeaponType::Melee);
		weapon->SetMeleeStat(1.5f, 10);
		weapon->SetOnMeleeAttack([this](const MathEngine::Vector2& startPos, const MathEngine::Vector2& dir,
			int damage, float range)
			{
				ApplyMeleeDamage(startPos, dir, damage, range);
			});
	}

	if (weaponSprite != nullptr)
	{
		weaponObj->SetElement(weaponSprite, ElementType::SpriteRenderer);
	}
	weaponObj->SetElement(weaponTr, ElementType::Transform);
	weaponObj->SetElement(weapon, ElementType::Weapon);
	weaponObj->Init(); // 여기서 Weapon::Init()이 호출되면서 m_transform, m_sprite 자동 세팅됨

	m_player->SetWeapon(weapon);
	if (itemData != nullptr && !itemData->weaponSpriteKey.empty())
	{
		m_player->SetArmedVisaul(itemData->weaponSpriteKey);
	}
}
void MainScene::UnequipWeaponFromPlayer()
{
	Weapon* current = m_player->GetWeapon();
	if (current == nullptr) return;

	m_player->GetInventory()->AddItem(current->GetWeaponId(), 1);
	DeletePObject(current->GetGameObject());
	m_player->SetWeapon(nullptr);
	m_player->ClearArmedVisual();
}

void MainScene::EquipShieldToPlayer(const string& shieldId)
{
	string currentShieldId = m_player->GetEquippedShieldId();
	if (!currentShieldId.empty())
	{
		m_player->GetInventory()->AddItem(currentShieldId, 1);
	}

	if (!m_player->GetInventory()->HasEnough(shieldId, 1)) return;

	const ItemData* itemData = DataManager::GetInstance().FindItem(shieldId);
	if (itemData == nullptr) return;

	m_player->GetInventory()->RemoveItem(shieldId, 1);
	m_player->SetShield(itemData->weaponSpriteKey, itemData->defenseValue);
	m_player->SetEquippedShieldId(shieldId);
}

void MainScene::UnequipShieldFromPlayer()
{
	string shieldId = m_player->GetEquippedShieldId();
	if (shieldId.empty()) return;

	m_player->GetInventory()->AddItem(shieldId, 1);
	m_player->ClearShield();
	m_player->SetEquippedShieldId("");
}

void MainScene::LoadEquipShield(const string& shieldId)
{
	if (shieldId.empty()) return;

	const ItemData* itemData = DataManager::GetInstance().FindItem(shieldId);
	if (itemData == nullptr) return;

	// 세이브 복원 시에는 인벤토리 차감/보유 체크 없이 상태만 복원
	m_player->SetShield(itemData->weaponSpriteKey, itemData->defenseValue);
	m_player->SetEquippedShieldId(shieldId);
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

		if (diff.Magnitude() < 1.0f)
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
	if (target->GetInteractType() == InteractType::Tree ||
		target->GetInteractType() == InteractType::Rock||
		target->GetInteractType() == InteractType::Iron)
	{
		GameObject* obj = target->GetGameObject();
		ResourceNode* resource =
			static_cast<ResourceNode*>(obj->GetElement(ElementType::ResourceNode));

		HarvestResource(resource);
		return;
	}

	UIManager::GetInstance().OpenCrafting(target->GetInteractType());
}

void MainScene::HarvestResource(ResourceNode* resource)
{
	if (resource == nullptr)
		return;

	GameObject* obj = resource->GetGameObject();
	if (obj == nullptr)
		return;

	Transform* tr = static_cast<Transform*>(obj->GetElement(ElementType::Transform));
	MathEngine::Vector2 pos = tr->GetPostion();

	static mt19937 rng(random_device{}());

	// 기본 아이템 드롭 (나무/돌 등)
	if (!resource->GetItemId().empty())
	{
		uniform_int_distribution<int> dist(resource->GetMinCount(), resource->GetMaxCount());
		int count = dist(rng);
		SpawnItemDrop(pos, resource->GetItemId(), count);   // ★ 바로 인벤토리 대신 드롭
	}

	// 보너스 드롭 (사과/바나나/버섯 등)
	uniform_real_distribution<float> chanceDist(0.0f, 1.0f);
	for (const ResourceDrop& drop : resource->GetBonusDrops())
	{
		if (chanceDist(rng) > drop.chance)
			continue;

		uniform_int_distribution<int> bonusCountDist(drop.minCount, drop.maxCount);
		int bonusCount = bonusCountDist(rng);
		SpawnItemDrop(pos, drop.itemId, bonusCount);   // ★ 드롭
	}

	DeletePObject(obj);   // 나무/바위 자체는 그대로 사라짐
}

GameObject* MainScene::AcquireBullet()
{
	//if (!m_bulletPool.empty())
	//{
	//	GameObject* obj = m_bulletPool.back();
	//	m_bulletPool.pop_back();
	//	return obj;
	//}

	//// 없으면 새로 생성 (최초 워밍업 또는 풀이 부족할 때만 여기로 옴)
	//GameObject* obj = CreateObject("Bullet");

	//Transform* tr = new Transform();
	//Bullet* bullet = new Bullet({ 0, 0 }, 0.f, 0.f, 0);
	//bullet->SetCollisionManager(m_collisionManager);

	//SpriteRenderer* sprite = new SpriteRenderer("Bullet");
	//sprite->SetResourceManager(m_resourceManager);
	//sprite->SetPivot(1, 10);
	//sprite->SetScale(2.f);

	//ColliderComponent* collider = new ColliderComponent();
	//collider->SetPhysicsWorld(m_physicsWorld);
	//collider->SetSyncMode(ColliderSyncMode::TransformDrivesPhysics);

	//obj->SetElement(tr, ElementType::Transform);
	//obj->SetElement(bullet, ElementType::Bullet);
	//obj->SetElement(sprite, ElementType::SpriteRenderer);
	//obj->SetElement(collider, ElementType::Collider);
	//obj->Init();

	//auto circleCollider = std::make_unique<PhysicsEngine::CircleCollider>(0.f, 0.f, 0.2f);
	//collider->SetCollider(std::move(circleCollider), 0.1f, false);
	//collider->SetTrigger(true);

	//collider->SetOnCollisionEnter([this, bullet](GameObject* other)
	//	{
	//		Monster* monster = static_cast<Monster*>(other->GetElement(ElementType::Monster));
	//		if (monster != nullptr && !monster->IsDead())
	//		{
	//			monster->TakeDamage(bullet->GetDamage());
	//			bullet->Despawn();
	//		}
	//		Interactable* interactable = static_cast<Interactable*>(other->GetElement(ElementType::Interactable));
	//		if (interactable != nullptr)
	//		{
	//			bullet->Despawn();
	//			return;
	//		}
	//	});

	//return obj;
	if (!m_bulletPool.empty())
	{
		GameObject* obj = m_bulletPool.back();
		m_bulletPool.pop_back();

		return obj;
	}

	// 풀이 없을 때만 생성
	GameObject* obj = CreateObject("Bullet");

	if (obj == nullptr)
		return nullptr;

	Transform* tr = new Transform();

	Bullet* bullet =
		new Bullet({ 0, 0 }, 0.f, 0.f, 0);

	bullet->SetCollisionManager(m_collisionManager);

	SpriteRenderer* sprite =
		new SpriteRenderer("Bullet");

	sprite->SetResourceManager(m_resourceManager);

	// 10 x 3 이미지라면 중앙 기준
	sprite->SetPivot(1.0f, 10.f);
	sprite->SetScale(2.f);

	ColliderComponent* collider =
		new ColliderComponent();

	collider->SetPhysicsWorld(m_physicsWorld);

	collider->SetSyncMode(
		ColliderSyncMode::TransformDrivesPhysics);

	obj->SetElement(tr, ElementType::Transform);
	obj->SetElement(bullet, ElementType::Bullet);
	obj->SetElement(sprite, ElementType::SpriteRenderer);
	obj->SetElement(collider, ElementType::Collider);

	obj->Init();

	auto circleCollider =
		std::make_unique<PhysicsEngine::CircleCollider>(
			0.f,
			0.f,
			0.2f);

	collider->SetCollider(
		std::move(circleCollider),
		0.1f,
		false);

	collider->SetTrigger(true);

	collider->SetOnCollisionEnter(
		[this, bullet](GameObject* other)
		{
			if (other == nullptr)
				return;

			Monster* monster =
				static_cast<Monster*>(
					other->GetElement(ElementType::Monster));

			if (monster != nullptr && !monster->IsDead())
			{
				monster->TakeDamage(
					bullet->GetDamage());

				bullet->Despawn();
				return;
			}

			Interactable* interactable =
				static_cast<Interactable*>(
					other->GetElement(ElementType::Interactable));

			if (interactable != nullptr)
			{
				bullet->Despawn();
				return;
			}
		});

	// 생성 직후에는 비활성
	obj->SetActive(false);
	collider->SetEnabled(false);

	return obj;
}

void MainScene::ReleaseBullet(GameObject* obj)
{
	obj->SetActive(false);   // Update/Render 멈춤

	ColliderComponent* collider = static_cast<ColliderComponent*>(obj->GetElement(ElementType::Collider));
	if (collider != nullptr)
	{
		collider->SetEnabled(false);   // 물리 충돌 검사에서 제외
	}

	m_bulletPool.push_back(obj);   // delete 대신 풀로 반납
}

GameObject* MainScene::AcquireMonster(const string& monsterId)
{
	auto& pool = m_monsterPool[monsterId];

	if (!pool.empty())
	{
		GameObject* obj = pool.back();
		pool.pop_back();
		return obj;
	}

	const MonsterData* data = DataManager::GetInstance().FindMonster(monsterId);
	if (data == nullptr)
	{
		return nullptr;
	}

	GameObject* obj = CreateObject("Monster");

	Transform* tr = new Transform();
	SpriteRenderer* sprite = new SpriteRenderer(data->image);
	sprite->SetResourceManager(m_resourceManager);
	sprite->SetPivot(data->cellWidth / 2, data->cellHeight);
	sprite->SetScale(data->scale);

	Animator* animator = new Animator();
	Monster* monster = new Monster(data->health);

	ColliderComponent* collider = new ColliderComponent();
	collider->SetPhysicsWorld(m_physicsWorld);
	collider->SetSyncMode(ColliderSyncMode::TransformDrivesPhysics);

	obj->SetElement(tr, ElementType::Transform);
	obj->SetElement(sprite, ElementType::SpriteRenderer);
	obj->SetElement(animator, ElementType::Animator);
	obj->SetElement(monster, ElementType::Monster);
	obj->SetElement(collider, ElementType::Collider);
	obj->Init();

	auto circleCollider = std::make_unique<PhysicsEngine::CircleCollider>(0.f, 0.f, data->colliderRadius);
	collider->SetCollider(std::move(circleCollider), 1.0f, false);
	collider->SetTrigger(true);

	monster->SetMonsterId(monsterId);   // ★ 자기 id 기억 (반납 시 필요)
	monster->SetCollisionManager(m_collisionManager);
	monster->SetFlowField(&m_flowField);

	ColliderComponent* playerCollider =
		static_cast<ColliderComponent*>(m_player->GetGameObject()->GetElement(ElementType::Collider));
	monster->SetTargetCollider(playerCollider);
	monster->SetTarget(m_player->GetTransform());

	auto contactDamage = [this, monster](GameObject* other)
		{
			if (other == m_player->GetGameObject() && monster->CanDealDamage())
			{
				m_player->TakeDamage(monster->GetContactDamage());
				monster->ResetDamageCooldown();
			}
		};
	collider->SetOnCollisionEnter(contactDamage);
	collider->SetOnCollisionStay(contactDamage);

	return obj;
}

void MainScene::ReleaseMonster(GameObject* obj, const string& monsterId)
{
	obj->SetActive(false);

	ColliderComponent* collider = static_cast<ColliderComponent*>(obj->GetElement(ElementType::Collider));
	if (collider != nullptr)
	{
		collider->SetEnabled(false);
	}

	m_monsterPool[monsterId].push_back(obj);
}

void MainScene::ClearAllMonsters()
{
	for (GameObject* obj : m_objects)
	{
		if (!obj->GetActive()) continue;

		Monster* monster = static_cast<Monster*>(obj->GetElement(ElementType::Monster));
		if (monster != nullptr)
		{
			monster->ForceStartFadeOut();
		}
	}
}

void MainScene::LoadMap(const vector<string>& mapData)
{
	for (int y = 0; y < (int)mapData.size(); y++)
	{
		const string& row = mapData[y];

		for (int x = 0; x < (int)row.size(); x++)
		{
			char tile = row[x];
			// Wall
			if (tile == '#')
			{
				string wallImage =
					GetWallImage(mapData, x, y);

				CreateWall(
					(float)x,
					(float)y,
					wallImage
				);

				continue;
			}

			// 기타 타일
			auto it = m_tileHandlers.find(tile);

			if (it != m_tileHandlers.end())
			{
				it->second(
					(float)x,
					(float)y
				);
			}
		}
	}
}

void MainScene::ApplyMeleeDamage(const MathEngine::Vector2& startPos, const MathEngine::Vector2& dir, int damage, float range)
{
	GameObject* obj = CreateObject("AttackHitbox");

	Transform* tr = new Transform();
	tr->SetPosition(startPos + dir * (range * 0.5f));

	AttackHitBox* hitbox = new AttackHitBox(damage, 0.1f);

	ColliderComponent* collider = new ColliderComponent();
	collider->SetPhysicsWorld(m_physicsWorld);
	collider->SetSyncMode(ColliderSyncMode::TransformDrivesPhysics);

	obj->SetElement(tr, ElementType::Transform);
	obj->SetElement(hitbox, ElementType::AttackHitBox);
	obj->SetElement(collider, ElementType::Collider);
	obj->Init();

	auto rectCollider = std::make_unique<PhysicsEngine::RectangleCollider>(0.5f, 0.5f, range, range);
	collider->SetCollider(std::move(rectCollider), 0.1f, false);

	collider->SetOnCollisionEnter([this, hitbox](GameObject* other)
		{
			Monster* monster = static_cast<Monster*>(other->GetElement(ElementType::Monster));
			if (monster != nullptr && !monster->IsDead())
			{
				monster->TakeDamage(hitbox->GetDamage());
			}
		});
}

void MainScene::CheckBullets()
{
	vector<GameObject*> toRelease;

	for (GameObject* obj : m_objects)
	{
		if (!obj->GetActive()) continue;   // 이미 비활성(풀에 있는) 건 건너뜀

		Bullet* bullet = static_cast<Bullet*>(obj->GetElement(ElementType::Bullet));
		if (bullet == nullptr) continue;

		if (bullet->IsDead())
		{
			toRelease.push_back(obj);
		}
	}

	for (GameObject* obj : toRelease)
	{
		ReleaseBullet(obj);   // DeletePObject 대신 풀로 반납
	}
}

void MainScene::CheckMonsters()
{
	vector<pair<GameObject*, string>> toRelease;

	Transform* playerTr = m_player->GetTransform();
	MathEngine::Vector2 playerPos = playerTr->GetPostion();

	for (GameObject* obj : m_objects)
	{
		if (!obj->GetActive()) continue;

		Monster* monster = static_cast<Monster*>(obj->GetElement(ElementType::Monster));
		if (monster == nullptr) continue;

		if (monster->IsReadyToRemove())
		{
			toRelease.push_back({ obj, monster->GetMonsterId() });
			continue;
		}

		//플레이어에게서 너무 멀어지면 즉시 회수
		Transform* monsterTr = monster->GetTransform();
		if (monsterTr != nullptr)
		{
			float dist = (monsterTr->GetPostion() - playerPos).Magnitude();
			if (dist > m_monsterDespawnDistance)
			{
				toRelease.push_back({ obj, monster->GetMonsterId() });
			}
		}
	}

	for (auto& [obj, id] : toRelease)
	{
		ReleaseMonster(obj, id);
	}
}

void MainScene::CheckAttackHitBoxes()
{
	vector<GameObject*> toRemove;
	for (GameObject* obj : m_objects)
	{
		AttackHitBox* hitbox = static_cast<AttackHitBox*>(obj->GetElement(ElementType::AttackHitBox));
		if (hitbox != nullptr && hitbox->IsExpired())
		{
			toRemove.push_back(obj);
		}
	}
	for (GameObject* obj : toRemove) DeletePObject(obj);
}

void MainScene::RenderAimLine(ID2D1DeviceContext* context)
{
	Weapon* weapon = m_player->GetWeapon();
	//Transform* playerTr = m_player->GetTransform();
	//MathEngine::Vector2 playerWorld = playerTr->GetPostion();

	MathEngine::Vector2 startWorld;
	if (weapon != nullptr && weapon->GetTransform() != nullptr)
	{
		startWorld = weapon->GetTransform()->GetPostion();
	}
	else
	{
		Transform* playerTr = m_player->GetTransform();
		startWorld = playerTr->GetPostion();
	}

	MathEngine::Vector2 playerScreen = TileManager::GetInstance().TileToScreen(startWorld);

	float startX = playerScreen.x - CameraManager::GetInstance().GetX();
	float startY = playerScreen.y - CameraManager::GetInstance().GetY();

	MathEngine::Vector2 mouseScreen = InputManager::GetInstance().GetMousePosition();

	bool isFlashing = (weapon != nullptr && weapon->IsMuzzleFlashActive());


	static float glowTime = 0.0f;
	glowTime += 0.016f;
	// 0 ~ 1 사이로 부드럽게 반복
	float pulse =
		(sinf(glowTime * 8.0f) + 1.0f) * 0.5f;

	D2D1_COLOR_F glowColor =
		D2D1::ColorF(
			1.0f,
			0.05f,
			0.1f,
			0.15f + pulse * 0.15f
		);

	D2D1_COLOR_F midColor =
		D2D1::ColorF(
			1.0f,
			0.1f,
			0.15f,
			0.35f + pulse * 0.2f
		);

	D2D1_COLOR_F coreColor =
		D2D1::ColorF(
			1.0f,
			0.35f,
			0.35f,
			0.8f + pulse * 0.2f
		);

	if (isFlashing)
	{
		glowColor = D2D1::ColorF(
			1.0f,
			0.05f,
			0.1f,
			0.35f
		);

		midColor = D2D1::ColorF(
			1.0f,
			0.1f,
			0.15f,
			0.75f
		);

		coreColor = D2D1::ColorF(
			1.0f,
			0.4f,
			0.4f,
			1.0f
		);
	}

	ID2D1SolidColorBrush* brush = nullptr;

	context->CreateSolidColorBrush(glowColor,&brush);

	if (brush != nullptr)
	{
		context->DrawLine(
			D2D1::Point2F(startX, startY),
			D2D1::Point2F(
				mouseScreen.x,
				mouseScreen.y
			),
			brush,
			10.0f + pulse * 4.0f
		);

		brush->Release();
	}
	context->CreateSolidColorBrush(midColor,&brush);

	if (brush != nullptr)
	{
		context->DrawLine(
			D2D1::Point2F(startX, startY),
			D2D1::Point2F(
				mouseScreen.x,
				mouseScreen.y
			),
			brush,
			5.0f + pulse * 2.0f
		);

		brush->Release();
	}


	context->CreateSolidColorBrush(coreColor,&brush);

	if (brush != nullptr)
	{
		context->DrawLine(
			D2D1::Point2F(startX, startY),
			D2D1::Point2F(
				mouseScreen.x,
				mouseScreen.y
			),
			brush,
			1.5f
		);

		brush->Release();
	}

	ID2D1Bitmap* crosshair = m_resourceManager->GetImage("CrossHair");

	if (crosshair != nullptr)
	{
		const float size = 32.0f;

		D2D1_RECT_F destRect =
		{
			mouseScreen.x - size*2.f,
			mouseScreen.y - size*2.f,
			mouseScreen.x + size*2.f,
			mouseScreen.y + size*2.f
		};

		D2D1_SIZE_F bitmapSize = crosshair->GetSize();
		D2D1_RECT_F srcRect ={0.0f,0.0f,bitmapSize.width,bitmapSize.height};

		DrawBitmap(context,crosshair,destRect,srcRect,false);
	}
}

void MainScene::UpdateMonsterSeparation()
{
	vector<Monster*> monsters;

	// 1. 살아있는 몬스터 수집
	for (GameObject* obj : m_objects)
	{
		if (!obj->GetActive())
			continue;

		Monster* monster = static_cast<Monster*>(obj->GetElement(ElementType::Monster));

		if (monster != nullptr && !monster->IsDead())
		{
			monsters.push_back(monster);
		}
	}
	const float separationRadius = 0.9f;
	const float separationRadiusSq = separationRadius * separationRadius;

	for (size_t i = 0; i < monsters.size(); i++)
	{
		Monster* monster = monsters[i];
		Transform* myTransform = monster->GetTransform();
		if (myTransform == nullptr) continue;

		MathEngine::Vector2 myPos = myTransform->GetPostion();
		MathEngine::Vector2 separation(0.0f, 0.0f);

		for (size_t j = 0; j < monsters.size(); j++)
		{
			if (i == j) continue;

			Monster* other = monsters[j];
			Transform* otherTransform = other->GetTransform();
			if (otherTransform == nullptr) continue;

			MathEngine::Vector2 diff = myPos - otherTransform->GetPostion();
			float distSq = diff.x * diff.x + diff.y * diff.y; // ★ sqrt 없이 먼저 판별

			if (distSq < separationRadiusSq && distSq > 0.000001f)
			{
				float distance = sqrtf(distSq); // ★ 진짜 반경 안에 들어온 쌍만 sqrt
				float strength = (separationRadius - distance) / separationRadius;
				separation += diff.Normalize() * strength; // Normalize 내부에서도 sqrt 있으니 필요시 diff/distance로 직접 계산 가능
			}
		}

		monster->SetSeparation(separation);
		//// 2. 각각의 몬스터에 대해 주변 몬스터를 계산
		//for (Monster* monster : monsters)
		//{
		//	Transform* myTransform = monster->GetTransform();

		//	if (myTransform == nullptr)
		//		continue;

		//	MathEngine::Vector2 myPos = myTransform->GetPostion();

		//	MathEngine::Vector2 separation(0.0f, 0.0f);

		//	for (Monster* other : monsters)
		//	{
		//		if (monster == other)
		//			continue;

		//		Transform* otherTransform = other->GetTransform();

		//		if (otherTransform == nullptr)
		//			continue;

		//		MathEngine::Vector2 diff =
		//			myPos - otherTransform->GetPostion();

		//		float distance = diff.Magnitude();

		//		// 몬스터끼리 너무 가까워졌을 때만 밀어냄
		//		const float separationRadius = 0.9f;

		//		if (distance < separationRadius && distance > 0.001f)
		//		{
		//			float strength =(separationRadius - distance) / separationRadius;

		//			separation += diff.Normalize() * strength;
		//		}
		//	}

		//	monster->SetSeparation(separation);
	}
}

void MainScene::UseItem(const string& itemId)
{
	const ItemData* itemData = DataManager::GetInstance().FindItem(itemId);
	if (itemData == nullptr) return;

	// 회복 아이템이 아니면 무시
	if (itemData->healAmount <= 0) return;

	if (!m_player->GetInventory()->HasEnough(itemId, 1))
		return;

	m_player->GetInventory()->RemoveItem(itemId, 1);
	m_player->Heal(itemData->healAmount);

	wchar_t buf[128];
	swprintf_s(buf, L"%s 사용! 체력 +%d", UTF8ToWString(itemData->name).c_str(), itemData->healAmount);
	UIManager::GetInstance().ShowMessage(buf);
}

void MainScene::SpawnItemDrop(const MathEngine::Vector2& centerPos, const string& itemId, int count)
{
	const ItemData* itemData = DataManager::GetInstance().FindItem(itemId);
	if (itemData == nullptr) return;

	// 드롭 위치를 살짝 랜덤으로 흩뿌려서 여러 개가 겹쳐 보이지 않게
	static mt19937 rng(random_device{}());
	uniform_real_distribution<float> offsetDist(-0.3f, 0.3f);

	MathEngine::Vector2 dropPos =
	{
		centerPos.x + offsetDist(rng),
		centerPos.y + offsetDist(rng)
	};

	GameObject* obj = CreateObject("ItemDrop");

	Transform* tr = new Transform();
	tr->SetPosition(dropPos);

	SpriteRenderer* sprite = new SpriteRenderer(itemData->image);
	sprite->SetResourceManager(m_resourceManager);
	sprite->SetScale(1.f);   // 채집물이니 살짝 작게 (원하는 크기로 조정)

	ItemPickUp* pickup = new ItemPickUp(itemId, count);
	// ★ posId를 굳이 세이브 추적용으로 안 씀 (임시 드롭이므로)

	obj->SetElement(tr, ElementType::Transform);
	obj->SetElement(sprite, ElementType::SpriteRenderer);
	obj->SetElement(pickup, ElementType::ItemPickUp);
	obj->Init();
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

bool MainScene::IsWall(const vector<string>& mapData, int x, int y)
{
	if (y < 0 || y >= (int)mapData.size())
		return false;

	if (x < 0 || x >= (int)mapData[y].size())
		return false;

	return mapData[y][x] == '#';
}

string MainScene::GetWallImage(const vector<string>& mapData, int x, int y)
{
	bool north = IsWall(mapData, x, y - 1);
	bool east = IsWall(mapData, x + 1, y);
	bool south = IsWall(mapData, x, y + 1);
	bool west = IsWall(mapData, x - 1, y);
	if (east || west)
	{
		return "Wall_E";
	}

	if (north || south)
	{
		return "Wall_N";
	}

	return "Wall_E";
}
