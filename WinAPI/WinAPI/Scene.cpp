#include "Scene.h"
#include "Graphics.h"
#include "GameObject.h"
#include <algorithm>
#include "Transform.h"
#include "CameraManager.h"
#include "TileManager.h"

Scene::Scene(string sceneName)
{
	m_name = sceneName;
}

Scene::~Scene()
{
	Release();
}

void Scene::Init()
{
	for (auto& object : m_objects)
	{
		object->Init();
	}
}

void Scene::FixedUpdate()
{
	size_t count = m_objects.size();
	for (size_t i = 0; i < count; i++)
	{
		m_objects[i]->FixedUpdate();
	}
}

void Scene::Update(float deltaTime)
{
	size_t count = m_objects.size(); // 이번 프레임 시작 시점 크기만 순회
	for (size_t i = 0; i < count; i++)
	{
		m_objects[i]->Update(deltaTime);
	}
}

void Scene::LateUpdate()
{
	size_t count = m_objects.size();
	for (size_t i = 0; i < count; i++)
	{
		m_objects[i]->LateUpdate();
	}
}

void Scene::PreRender()
{
	size_t count = m_objects.size();
	for (size_t i = 0; i < count; i++)
	{
		m_objects[i]->PreRender();
	}
}
void Scene::Render(ID2D1DeviceContext* context)
{
	SortObjects();

	size_t count = m_objects.size();

	for (size_t i = 0; i < count; i++)
	{
		GameObject* obj = m_objects[i];

		if (obj == nullptr)
			continue;

		if (!IsVisible(obj))
			continue;

		obj->Render(context);
	}
}
void Scene::PostRender(ID2D1DeviceContext* context)
{
	size_t count = m_objects.size();
	for (size_t i = 0; i < count; i++)
	{
		m_objects[i]->PostRender(context);
	}
}

void Scene::Release()
{
	DeleteAllObject(); 
}

GameObject* Scene::CreateObject(string name)
{
	GameObject* newObject = new GameObject(name);
	AddObject(newObject);

	return newObject;
}

void Scene::AddObject(GameObject* gameObject)
{
	m_objects.push_back(gameObject);
}

void Scene::DeleteObject(string name)
{
	vector<GameObject*>::iterator findObject = find_if(m_objects.begin(), m_objects.end(), [=](GameObject* gameObject) {return gameObject->GetName() == name; });
	GameObject* deleteObject = *findObject;

	m_objects.erase(std::remove_if(m_objects.begin(), m_objects.end(), [=](GameObject* gameObject) {return gameObject->GetName() == name; }), m_objects.end());
	delete deleteObject;
}

void Scene::DeletePObject(GameObject* target)
{
	auto it = std::find(m_objects.begin(), m_objects.end(), target);
	if (it == m_objects.end()) return;

	m_objects.erase(it);
	delete target;
}

GameObject* Scene::FindObject(string name)
{
	vector<GameObject*>::iterator findObject = find_if(m_objects.begin(), m_objects.end(), [=](GameObject* gameObject) {return gameObject->GetName() == name; });
	return *findObject;
}

void Scene::DeleteAllObject()
{
	for (auto& object : m_objects)
	{
		delete object;
	}
	m_objects.clear();
}

bool Compare(GameObject* a, GameObject* b)
{
	Transform* ta = static_cast<Transform*>(a->GetElement(ElementType::Transform));
	Transform* tb = static_cast<Transform*>(b->GetElement(ElementType::Transform));

	float sumA = ta->GetPostion().x + ta->GetPostion().y;
	float sumB = tb->GetPostion().x + tb->GetPostion().y;

	return sumA < sumB;
}

void Scene::SortObjects()
{
	sort(m_objects.begin(), m_objects.end(), Compare);
}

bool Scene::IsVisible(GameObject* obj)
{
	if (obj == nullptr)
		return false;

	Transform* tr =
		static_cast<Transform*>(obj->GetElement(ElementType::Transform));

	if (tr == nullptr)
		return false;

	MathEngine::Vector2 worldPos = tr->GetPostion();

	// 월드 좌표 → 화면 좌표
	MathEngine::Vector2 screen =
		TileManager::GetInstance().TileToScreen(worldPos);

	float screenX =
		screen.x - CameraManager::GetInstance().GetX();

	float screenY =
		screen.y - CameraManager::GetInstance().GetY();

	// 화면 크기
	const float SCREEN_W = 1280.0f;
	const float SCREEN_H = 720.0f;

	// 오브젝트가 화면 밖으로 완전히 나간 경우
	const float MARGIN = 150.0f;

	if (screenX < -MARGIN ||
		screenX > SCREEN_W + MARGIN ||
		screenY < -MARGIN ||
		screenY > SCREEN_H + MARGIN)
	{
		return false;
	}

	return true;
}