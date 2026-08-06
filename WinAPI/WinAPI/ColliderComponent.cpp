#include "ColliderComponent.h"
#include "GameObject.h"
#include "Transform.h"
#include "../PhysicsEngine/PhysicsWorld.h"
#include "../PhysicsEngine/CircleCollider.h"
#include "Graphics.h"
#include <memory>
#include <utility>
#include "TileManager.h"
#include "CameraManager.h"

ColliderComponent::ColliderComponent()
{
}

ColliderComponent::~ColliderComponent()
{
}

void ColliderComponent::Init()
{
	m_transform = static_cast<Transform*>(m_gameObject->GetElement(ElementType::Transform));
}

void ColliderComponent::FixedUpdate()
{
}

void ColliderComponent::Update(float deltaTime)
{
	if (m_object == nullptr || m_object->collider == nullptr) return;

	if (m_syncMode == ColliderSyncMode::TransformDrivesPhysics)
	{
		// 1. 프레임 시작 시: Transform -> Physics 전달
		m_object->position = m_transform->GetPostion();
		m_object->collider->center = m_object->position;
	}
}

void ColliderComponent::LateUpdate()
{
	if (m_object && m_transform)
	{
		// Physics World가 밀어낸 최신 좌표를 Transform(화면 위치)에 반영
		m_transform->SetPosition(m_object->position);
	}
}

void ColliderComponent::PreRender()
{
}

void ColliderComponent::Render(ID2D1DeviceContext* context)
{
	if (m_object == nullptr || m_object->collider == nullptr) return;

	PhysicsEngine::Collider* col = m_object->collider;
	MathEngine::Vector2 center = col->center;

	D2D1::ColorF debugColor = m_isCollision ? D2D1::ColorF::Red : D2D1::ColorF::Yellow;

	if (PhysicsEngine::CircleCollider* circle = dynamic_cast<PhysicsEngine::CircleCollider*>(col))
	{
		// 1. Player와 동일하게 Tile -> Screen 좌표 변환
		MathEngine::Vector2 screen = TileManager::GetInstance().TileToScreen(center);

		// 2. 카메라 오프셋 적용
		float screenX = screen.x - CameraManager::GetInstance().GetX();
		float screenY = screen.y - CameraManager::GetInstance().GetY();

		// 3. 반지름(radius)도 타일/스크린 단위에 맞게 변환
		// (만약 TileManager에 타일 픽셀 크기를 가져오는 함수가 있다면 그것을 활용하고, 
		//  일반적으로 64px 기준이라면 radius * 64.f 로 계산합니다)
		float tileSize = 64.0f; // 사용하시는 타일 픽셀 크기
		float drawRadius = circle->radius * tileSize;

		GRAPHICS.DrawCircle(screenX, screenY, drawRadius, debugColor);
	}
}

void ColliderComponent::PostRender(ID2D1DeviceContext* context)
{
}

void ColliderComponent::Release()
{
	if (m_world != nullptr && m_object != nullptr)
	{
		m_world->RemoveObject(m_object); // World가 delete까지 처리 (Object 소멸자가 collider도 정리)
		m_object = nullptr;
	}
}

ElementType ColliderComponent::GetElementType() const
{
	return ElementType::Collider;
}

GameObject* ColliderComponent::GetGameObject() const
{
	return m_gameObject;
}

void ColliderComponent::SetGameObject(GameObject* gameObject)
{
	m_gameObject = gameObject;
}

PhysicsEngine::Collider* ColliderComponent::GetCollider()
{
	return (m_object != nullptr) ? m_object->collider : nullptr;
}

void ColliderComponent::SetCollider(std::unique_ptr<PhysicsEngine::Collider> collider, float mass, bool isStatic)
{
	MathEngine::Vector2 startPos = (m_transform != nullptr) ? m_transform->GetPostion() : MathEngine::Vector2(0.f, 0.f);

	m_object = new PhysicsEngine::Object(
		startPos,
		MathEngine::Vector2(0.f, 0.f),
		MathEngine::Vector2(0.f, 0.f),
		mass,
		isStatic
	);

	m_object->collider = collider.release();
	m_object->collider->center = startPos; // 중심 위치 초기화

	if (m_world != nullptr)
	{
		m_world->AddObject(m_object);
	}
}
