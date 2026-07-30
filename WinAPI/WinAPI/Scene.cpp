#include "Scene.h"
#include "Graphics.h"
#include "GameObject.h"
#include <algorithm>
#include "Transform.h"

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
	for (auto& object : m_objects)
	{
		object->FixedUpdate();
	}
}

void Scene::Update(float deltaTime)
{
	for (auto& object : m_objects)
	{
		object->Update(deltaTime);
	}
}

void Scene::LateUpdate()
{
	for (auto& object : m_objects)
	{
		object->LateUpdate();
	}
}

void Scene::PreRender()
{
	for (auto& object : m_objects)
	{
		object->PreRender();
	}
}
void Scene::Render(ID2D1DeviceContext* context)
{
	SortObjects();
	for (auto& object : m_objects)
	{
		object->Render(context);
	}
}
void Scene::PostRender(ID2D1DeviceContext* context)
{
	for (auto& object : m_objects)
	{
		object->PostRender(context);
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
