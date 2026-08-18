#include "Wall.h"
#include "Transform.h"
#include "GameObject.h"

Wall::Wall(): m_transform(nullptr), m_gameObject(nullptr)
{
}

Wall::~Wall()
{
}

void Wall::Init()
{
    m_transform =static_cast<Transform*>( m_gameObject->GetElement(ElementType::Transform));
}

void Wall::Update(float deltaTime)
{
}

void Wall::Release()
{
}

ElementType Wall::GetElementType() const
{
    return ElementType::Wall; 
}

GameObject* Wall::GetGameObject() const
{
    return m_gameObject;
}

void Wall::SetGameObject(GameObject* gameObject)
{
    m_gameObject = gameObject;
}