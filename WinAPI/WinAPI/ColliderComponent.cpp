#include "ColliderComponent.h"
#include "GameObject.h"
#include "Transform.h"
#include "../PhysicsEngine/PhysicsWorld.h"
#include "../PhysicsEngine/CircleCollider.h"
#include "../PhysicsEngine/RectangleCollider.h"
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
	if (!m_object || !m_transform)
		return;

	if (!m_object->isEnabled)
		return;
	//m_isCollision = m_object->isColliding;

	if (m_syncMode == ColliderSyncMode::TransformDrivesPhysics)
	{
		m_object->position = m_transform->GetPostion();
		m_object->collider->center = m_object->position;
	}
}

void ColliderComponent::Update(float deltaTime)
{
	if (m_object == nullptr || m_object->collider == nullptr|| !m_object->isEnabled) return;

	if (m_syncMode == ColliderSyncMode::TransformDrivesPhysics)
	{
		MathEngine::Vector2 newPos = m_transform->GetPostion();

		//// 이번 프레임 실제 이동량으로 속도 추정
		if (deltaTime > 0.f)
		{
			m_object->velocity = (newPos - m_object->position) / deltaTime;
		}

		// 1. 프레임 시작 시: Transform -> Physics 전달
		m_object->position = m_transform->GetPostion();
		m_object->collider->center = m_object->position;
	}
}

void ColliderComponent::LateUpdate()
{
	//if (!m_object || !m_transform)
	//	return;

	//m_transform->SetPosition(m_object->position);

	//m_isCollision = m_object->isColliding;

	/*if (!m_object || !m_transform)
		return;

	m_isCollision = m_object->isColliding;

	if (m_syncMode == ColliderSyncMode::PhysicsDrivesTransform)
	{
		m_transform->SetPosition(m_object->position);
	}*/
	/*if (!m_object || !m_transform)
		return;

	m_isCollision = m_object->isColliding;

	if (m_syncMode == ColliderSyncMode::PhysicsDrivesTransform)
	{
		m_transform->SetPosition(m_object->position);
	}
	else if (m_syncMode == ColliderSyncMode::TransformDrivesPhysics)
	{
		m_object->position = m_transform->GetPostion();
	}*/
	if (!m_object || !m_transform)
		return;

	m_isCollision = m_object->isColliding;

	//m_transform->SetPosition(m_object->position);
	
	if (m_syncMode == ColliderSyncMode::PhysicsDrivesTransform)
	{
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

	MathEngine::Vector2 screen = TileManager::GetInstance().TileToScreen(center);
	float screenX = screen.x - CameraManager::GetInstance().GetX();
	float screenY = screen.y - CameraManager::GetInstance().GetY();
	float tileSize = 64.0f; // 사용하시는 타일 픽셀 크기

	if (PhysicsEngine::CircleCollider* circle = dynamic_cast<PhysicsEngine::CircleCollider*>(col))
	{
		float drawRadius = circle->radius * tileSize;
		GRAPHICS.DrawCircle(screenX, screenY, drawRadius, debugColor);
	}
	else if (PhysicsEngine::RectangleCollider* rect = dynamic_cast<PhysicsEngine::RectangleCollider*>(col))
	{
		float drawWidth = rect->size.x * tileSize;
		float drawHeight = rect->size.y * tileSize;

		// center 기준이므로 좌상단 좌표로 변환
		float drawX = screenX - drawWidth * 0.5f;
		float drawY = screenY - drawHeight * 0.5f;
		GRAPHICS.DrawRect(drawX, drawY, drawWidth, drawHeight, debugColor);
	}
}

void ColliderComponent::PostRender(ID2D1DeviceContext* context)
{
}

void ColliderComponent::Release()
{
	if (m_world != nullptr && m_object != nullptr)
	{
		OutputDebugStringW(L"[Physics] RemoveObject\n");
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
	m_object->collider->center = startPos;
	m_object->owner = m_gameObject;
	m_object->isKinematic = (m_syncMode == ColliderSyncMode::TransformDrivesPhysics);

	if (m_world != nullptr)
	{
		m_world->AddObject(m_object);
	}
}

void ColliderComponent::SetOnCollisionEnter(std::function<void(GameObject* other)> callback)
{
	if (m_object == nullptr) return;
	m_object->onCollisionEnter = [callback](PhysicsEngine::Object* other)
		{
			if (other != nullptr && other->owner != nullptr)
				callback(static_cast<GameObject*>(other->owner));
		};
}

void ColliderComponent::SetOnCollisionStay(std::function<void(GameObject* other)> callback)
{
	if (m_object == nullptr) return;
	m_object->onCollisionStay = [callback](PhysicsEngine::Object* other)
		{
			if (other != nullptr && other->owner != nullptr)	
				callback(static_cast<GameObject*>(other->owner));
		};
}

void ColliderComponent::SetOnCollisionExit(std::function<void(GameObject* other)> callback)
{
	if (m_object == nullptr) return;
	m_object->onCollisionExit = [callback](PhysicsEngine::Object* other)
		{
			if (other != nullptr && other->owner != nullptr)
				callback(static_cast<GameObject*>(other->owner));
		};
}

bool ColliderComponent::IsPositionBlocked(const MathEngine::Vector2& pos, float radius, ColliderComponent* ignoreTarget, bool onlyStatic) const
{
	if (m_world == nullptr) return false;

	PhysicsEngine::CircleCollider testCollider(0.f, 0.f, radius);
	testCollider.center = pos;

	// 자기 자신은 항상 무시하고, 추가로 무시할 대상(예: 몬스터의 타겟)도 무시
	PhysicsEngine::Object* ignoreObj = (ignoreTarget != nullptr) ? ignoreTarget->GetPhysicsObject() : nullptr;

	if (m_world->IsColliderBlocked(testCollider, m_object, onlyStatic))
		return true;

	PhysicsEngine::Object* ignoreExtra = (ignoreTarget != nullptr) ? ignoreTarget->GetPhysicsObject() : nullptr;

	return m_world->IsColliderBlocked(testCollider, { m_object, ignoreExtra }, onlyStatic);
}

void ColliderComponent::PushNearbyDynamics(const MathEngine::Vector2& testPos, float radius, const MathEngine::Vector2& pusherCenter, ColliderComponent* ignoreTarget)
{
	if (m_world == nullptr) return;

	PhysicsEngine::CircleCollider testCollider(0.f, 0.f, radius);
	testCollider.center = testPos;

	PhysicsEngine::Object* ignoreObj = (ignoreTarget != nullptr) ? ignoreTarget->GetPhysicsObject() : nullptr;
	m_world->PushDynamicObjects(testCollider, pusherCenter, radius, ignoreObj);
}

MathEngine::Vector2 ColliderComponent::GetAvoidVector(const MathEngine::Vector2& pos, float checkRadius, ColliderComponent* ignoreTarget) const
{
	if (m_world == nullptr) return MathEngine::Vector2(0.f, 0.f);

	PhysicsEngine::Object* ignoreObj = (ignoreTarget != nullptr) ? ignoreTarget->GetPhysicsObject() : nullptr;
	return m_world->GetPushAwayVector(pos, 0.f, checkRadius, ignoreObj);
}
