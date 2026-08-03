#pragma once
#include "pch.h"
#include "Graphics.h"

class GameObject;

class Scene
{
public:
	Scene(string sceneName);
	virtual ~Scene();

	virtual void Init();
	
	virtual void FixedUpdate();
	virtual void Update(float deltaTime);
	virtual void LateUpdate();

	virtual void PreRender();
	virtual void Render(ID2D1DeviceContext* context);
	virtual void PostRender(ID2D1DeviceContext* context);

	virtual void Release();

	GameObject* CreateObject(string name);
	void AddObject(GameObject* gameObject);
	void DeleteObject(string name);
	void DeletePObject(GameObject* target);
	GameObject* FindObject(string name);

	void DeleteAllObject();

	void SortObjects();

	string GetName() const { return m_name; }

protected:
	string m_name;
	vector<GameObject*> m_objects;
};

