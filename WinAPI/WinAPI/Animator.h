#pragma once
#include "ElementBase.h"

class SpriteRenderer;

class Animator : public ElementBase
{
public:
	Animator();
	~Animator();
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

	void SetCellSize(int width, int height) { m_cellWidth = width; m_cellHeight = height; }
	void SetAnimation(int column, int row,float frameTime);

	void Play(int row, int frameCount, float frameTime);

	void PlayAndPauseAt(int row, int frameCount, float frameTime, int pauseFrame);
	void ResumeAnimation();

	void Pause() { m_isPaused = true; }
	void Resume() { m_isPaused = false; }
	bool IsPaused() const { return m_isPaused; }
	int GetFrame() const { return m_frame; }

	bool IsFinished() const { return m_isFinished; }
private:
	void ApplyCurrentFrame();
private:
	SpriteRenderer* m_sprite;

	int m_frame;
	int m_column;
	int m_row;
	float m_timer;
	float m_frameTime;
	int m_frameCount = 1;

	int m_cellWidth = 460;
	int m_cellHeight = 460;
	
	bool m_isPaused = false;
	bool m_pauseAtFrame = false;
	int m_pauseFrame = 0;

	bool m_isFinished = false;
};

