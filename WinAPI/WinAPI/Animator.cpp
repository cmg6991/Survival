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
    if (m_sprite == nullptr)
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
        m_frame * 460,
        m_row * 460,
        460,
        460);

}

void Animator::LateUpdate()
{
}

void Animator::PreRender()
{
}

void Animator::Render(Graphics& graphics)
{
}

void Animator::PostRender(HDC hdc)
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
    if (m_row != row)
    {
        m_row = row;
        m_frame = 0;
        m_timer = 0;
    }
	m_column = column;
	m_frameTime = frameTime;

}

void Animator::Play(int row, int frameCount, float frameTime)
{
    if (m_row != row)
    {
        m_row = row;
        m_frame = 0;
        m_timer = 0;
    }

    m_frameCount = frameCount;
    m_frameTime = frameTime;
}
