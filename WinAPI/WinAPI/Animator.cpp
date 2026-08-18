#include "Animator.h"
#include "SpriteRenderer.h"
#include "GameObject.h"

Animator::Animator() : m_sprite(nullptr), m_frame(0), m_column(0), m_row(0), m_timer(0), m_frameTime(0.15f)
{
}

Animator::~Animator()
{
}

void Animator::Init()
{
	m_sprite = static_cast<SpriteRenderer*>(m_gameObject->GetElement(ElementType::SpriteRenderer));
}

void Animator::FixedUpdate()
{
}

void Animator::Update(float deltaTime)
{
    /*if (m_sprite == nullptr)
        return;

    m_timer += deltaTime;

    if (m_timer >= m_frameTime)
    {
        m_timer = 0;

        m_frame++;

        if (m_frame >= m_column)
            m_frame = 0;
    }
    m_sprite->SetFrameRect(
        m_frame * m_cellWidth,
        m_row * m_cellHeight,
        m_cellWidth,
        m_cellHeight);
    ApplyCurrentFrame();*/
    if (m_isPaused)
        return;

    if (m_sprite == nullptr)
        return;

    m_timer += deltaTime;

    if (m_timer >= m_frameTime)
    {
        m_timer -= m_frameTime;

        m_frame++;
        if (m_pauseAtFrame)
        {
            if (m_frame >= m_pauseFrame)
            {
                m_frame = m_pauseFrame;
                m_isPaused = true;
                m_isFinished = true;
            }
        }
        else
        {
            if (m_frame >= m_column)
                m_frame = 0;
        }
    }

    ApplyCurrentFrame();
}

void Animator::LateUpdate()
{
}

void Animator::PreRender()
{
}

void Animator::Render(ID2D1DeviceContext* context)
{
}

void Animator::PostRender(ID2D1DeviceContext* context)
{
}

void Animator::Release()
{
}

ElementType Animator::GetElementType() const
{
	return ElementType::Animator;
}

GameObject* Animator::GetGameObject() const
{
	return m_gameObject;
}

void Animator::SetGameObject(GameObject* gameObject)
{
	m_gameObject = gameObject;
}

void Animator::SetAnimation(int column, int row,float frameTime)
{
 //   if (m_row != row)
 //   {
 //       m_row = row;
 //       m_frame = 0;
 //       m_timer = 0;
 //   }
	//m_column = column;
	//m_frameTime = frameTime;
    //bool changed = (m_row != row);

    //if (changed)
    //{
    //    m_row = row;
    //    m_frame = 0;
    //    m_timer = 0;
    //}
    //m_column = column;
    //m_frameTime = frameTime;

    //if (changed)
    //{
    //    ApplyCurrentFrame();   // ★ 즉시 첫 프레임 반영
    //}
    bool changed =
        (m_column != column) ||
        (m_row != row) ||
        (m_frameTime != frameTime);

    m_column = column;
    m_row = row;
    m_frameTime = frameTime;

    m_pauseAtFrame = false;

    if (changed)
    { 
        m_frame = 0;
        m_timer = 0.0f;

        m_isPaused = false;

        ApplyCurrentFrame();
    }

}

void Animator::Play(int row, int frameCount, float frameTime)
{
    m_row = row;
    m_column = frameCount;
    m_frameCount = frameCount;
    m_frameTime = frameTime;

    m_frame = 0;
    m_timer = 0.0f;

    m_pauseAtFrame = false;

    m_isPaused = false;
    m_isFinished = false;

    ApplyCurrentFrame();
}

void Animator::PlayAndPauseAt(int row, int frameCount, float frameTime, int pauseFrame)
{
    m_row = row;
    m_column = frameCount;
    m_frameCount = frameCount;
    m_frameTime = frameTime;

    // 멈출 프레임
    m_pauseFrame = pauseFrame;

    // 잘못된 값 방지
    if (m_pauseFrame < 0)
        m_pauseFrame = 0;

    if (pauseFrame >= frameCount)
        pauseFrame = frameCount - 1;
    m_pauseAtFrame = pauseFrame;

    m_pauseAtFrame = true;
    m_isPaused = false;
    m_frame = 0;
    m_timer = 0.0f;

    ApplyCurrentFrame();
}

void Animator::ResumeAnimation()
{
    if (!m_pauseAtFrame)
    {
        m_isPaused = false;
        return;
    }
    m_isPaused = false;
    m_isFinished = false;
    // 현재 프레임에서 이어서 진행
    m_pauseAtFrame = false;
}

void Animator::ApplyCurrentFrame()
{
    if (m_sprite == nullptr)
        return;

    m_sprite->SetFrameRect(
        m_frame * m_cellWidth,
        m_row * m_cellHeight,
        m_cellWidth,
        m_cellHeight);
}
