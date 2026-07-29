#include "Transform.h"

Transform::Transform()
{
}

Transform::~Transform()
{
}

void Transform::Init()
{
}

void Transform::FixedUpdate()
{
}

void Transform::Update(float deltaTime)
{
}

void Transform::LateUpdate()
{
}

void Transform::PreRender()
{
}

void Transform::Render(Graphics& graphics)
{
}

void Transform::PostRender(HDC hdc)
{
}

void Transform::Release()
{
}

ElementType Transform::GetElementType() const
{
	return ElementType::Transform;
}

GameObject* Transform::GetGameObject() const
{
	return m_gameObject;
}

void Transform::SetGameObject(GameObject* gameObject)
{
	m_gameObject = gameObject;
}
