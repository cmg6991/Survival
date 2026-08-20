#include "GameObject.h"
#include "pch.h"
#include "Graphics.h"

GameObject::GameObject(std::string name) :m_name(name), m_elements((int)ElementType::End, nullptr)
{
}

GameObject::~GameObject()
{
	OutputDebugStringA(
		("DELETE GameObject: " + m_name + "\n").c_str()
	);
	Release();
}

void GameObject::Init()
{
	for (int i = 0; i < (int)ElementType::End; i++)
	{
		if (m_elements[i] != nullptr)
		{
			m_elements[i]->Init();
		}
	}
}

void GameObject::FixedUpdate()
{
	for (int i = 0; i < (int)ElementType::End; i++)
	{
		if (m_elements[i] != nullptr)
		{
			m_elements[i]->FixedUpdate();
		}
	}
}

void GameObject::Update(float deltaTime)
{
	if (m_isActive)
	{
		for (int i = 0; i < (int)ElementType::End; i++)
		{
			if (m_elements[i] != nullptr)
			{
				m_elements[i]->Update(deltaTime);
			}
		}
	}
}

void GameObject::LateUpdate()
{
	for (int i = 0; i < (int)ElementType::End; i++)
	{
		if (m_elements[i] != nullptr)
		{
			m_elements[i]->LateUpdate();
		}
	}
}

void GameObject::PreRender()
{
	for (int i = 0; i < (int)ElementType::End; i++)
	{
		if (m_elements[i] != nullptr)
		{
			m_elements[i]->PreRender();
		}
	}
}

void GameObject::Render(ID2D1DeviceContext* context)
{
	if (m_isDraw)
	{
		for (int i = 0; i < (int)ElementType::End; i++)
		{
			if (m_elements[i] != nullptr)
			{
				m_elements[i]->Render(context);
			}
		}
	}
}

void GameObject::PostRender(ID2D1DeviceContext* context)
{
	for (int i = 0; i < (int)ElementType::End; i++)
	{
		if (m_elements[i] != nullptr)
		{
			m_elements[i]->PostRender(context);
		}
	}
}

void GameObject::Release()
{
	for (int i = 0; i < (int)ElementType::End; i++)
	{
		if (m_elements[i] != nullptr)
		{
			m_elements[i]->Release();
			delete m_elements[i];
			m_elements[i] = nullptr;
		}
	}
}

void GameObject::SetElement(ElementBase* element, ElementType type)
{
	m_elements[(int)type] = element;
	element->SetGameObject(this);
}
