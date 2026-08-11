#pragma once

#include "pch.h"
#include <memory>
#include "ElementBase.h"
#include "../PhysicsEngine/Collider.h"
#include "../PhysicsEngine/Object.h"
#include <utility>

namespace PhysicsEngine
{
	struct Collider;
	struct Object;
	class PhysicsWorld;
}

enum class ColliderSyncMode
{
	PhysicsDrivesTransform, // 물리가 위치를 정함 (몬스터, 투사체 등)
	TransformDrivesPhysics  // 게임 로직이 이미 위치를 정함 (플레이어처럼 직접 이동하는 애)
};

class Transform;

class ColliderComponent : public ElementBase
{
public:
	ColliderComponent();
	~ColliderComponent();

	virtual void Init() override;
	virtual void FixedUpdate() override;
	virtual void Update(float deltaTime) override;
	virtual void LateUpdate() override;
	virtual void PreRender() override;
	virtual void Render(ID2D1DeviceContext* context) override;
	virtual void PostRender(ID2D1DeviceContext* context) override;
	virtual void Release() override;
	virtual ElementType GetElementType() const override;
	virtual GameObject* GetGameObject() const override;
	virtual void SetGameObject(GameObject* gameObject) override;

	/// 접근자 & 설정자
	PhysicsEngine::Collider* GetCollider();
	PhysicsEngine::Object* GetPhysicsObject() { return m_object; }

	// 물리엔진 연동 시점에 호출
	void SetPhysicsWorld(PhysicsEngine::PhysicsWorld* world) { m_world = world; }
	void SetSyncMode(ColliderSyncMode mode) { m_syncMode = mode; }

	// Transform 위치를 기준으로 Object + Collider를 생성하고 World에 등록
	void SetCollider(std::unique_ptr<PhysicsEngine::Collider> collider, float mass, bool isStatic);

	bool GetIsCollision() const { return m_isCollision; }
	void SetIsCollision(bool isCollision) { m_isCollision = isCollision; }

	void SetEnabled(bool enabled) { if (m_object) m_object->isEnabled = enabled; }
	void SetTrigger(bool isTrigger) { if (m_object) m_object->isTrigger = isTrigger; }
public:
	void SetOnCollisionEnter(std::function<void(GameObject* other)> callback);
	void SetOnCollisionStay(std::function<void(GameObject* other)> callback);
	void SetOnCollisionExit(std::function<void(GameObject* other)> callback);

private:
	// 충돌 여부
	bool m_isCollision = false;

	Transform* m_transform = nullptr;

	// 물리엔진 연동
	PhysicsEngine::PhysicsWorld* m_world = nullptr;
	PhysicsEngine::Object* m_object = nullptr; // Object가 collider도 소유함 (Object 소멸자가 delete)
	ColliderSyncMode m_syncMode = ColliderSyncMode::PhysicsDrivesTransform;
};