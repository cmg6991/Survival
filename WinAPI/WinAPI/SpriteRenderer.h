#pragma once
#include "ElementBase.h"
#include "pch.h"
#include "Vector2.h"

class Transform;
class ResourceManager;

class SpriteRenderer : public ElementBase
{
public:
	SpriteRenderer(string imageName);
	~SpriteRenderer();
	virtual void Init() override;

	virtual void FixedUpdate() override;
	virtual void Update(float deltaTime) override;
	virtual void LateUpdate() override;

	virtual void PreRender() override;
	virtual void Render(Graphics& graphics) override;
	virtual void PostRender(HDC hdc) override;

	virtual void Release() override;

	virtual ElementType GetElementType() const override;

	virtual GameObject* GetGameObject() const override;
	virtual void SetGameObject(GameObject* gameObject) override;

	void SetResourceManager(ResourceManager* resourceManager) {m_resourceManager = resourceManager; }
	void SetPivot(float x, float y) { m_pivot.x = x, m_pivot.y = y; }

	void SetFrameRect(int srcX, int srcY, int frameWidth, int frameHeight)
	{
		m_srcX = srcX;
		m_srcY = srcY;
		m_frameWidth = frameWidth;
		m_frameHeight = frameHeight;
	}

	//void SetFrame(int row, int frame) { m_row = row; m_frame = frame; }

	void SetFlip(bool flip) { m_flip = flip; }

	Bitmap* CreateFlip(Bitmap* src);

private:
	string m_imageName;
	Transform* m_transform;
	ResourceManager* m_resourceManager;

	MathEngine::Vector2 m_pivot;

	int m_width;
	int m_height;

//private:
//	Bitmap* m_frames[5][6];
//	Bitmap* m_flipFrames[5][6];
//
//	int m_row;
//	int m_frame;

private:
	int m_srcX = 0;
	int m_srcY = 0;
	int m_frameWidth = 0;
	int m_frameHeight = 0;
	bool m_flip = false;
};
