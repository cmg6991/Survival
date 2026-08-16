#include "SpriteRenderer.h"
#include "Transform.h"
#include "CameraManager.h"
#include "GameObject.h"
#include "Vector2.h"
#include "ResourceManager.h"
#include "TileManager.h"

SpriteRenderer::SpriteRenderer(string imageName)
    : m_imageName(imageName), m_transform(nullptr), m_resourceManager(nullptr), m_pivot(-1, -1),
    m_width(0), m_height(0)
{
}

SpriteRenderer::~SpriteRenderer()
{
}

void SpriteRenderer::Init()
{
    m_transform = static_cast<Transform*>(m_gameObject->GetElement(ElementType::Transform));
    ID2D1Bitmap* bitmap = m_resourceManager->GetImage(m_imageName);

    if (bitmap == nullptr)
    {
        OutputDebugStringA(("[SpriteRenderer::Init] 이미지 로드 실패, 키: " + m_imageName + "\n").c_str());
        return; // 크래시 방지
    }
    D2D1_SIZE_U size = bitmap->GetPixelSize();
    m_width = size.width;
    m_height = size.height;

    if (m_pivot.x < 0)
    {
        m_pivot.x = m_width * 0.5f;
    }

    if (m_pivot.y < 0)
    {
        m_pivot.y = (float)m_height;
    }

    if (m_frameWidth == 0)
        m_frameWidth = m_width;

    if (m_frameHeight == 0)
        m_frameHeight = m_height;

    /*int frameWidth = bitmap->GetWidth() / 6;
    int frameHeight = bitmap->GetHeight() / 5;

    for (int row = 0; row < 5; row++)
    {
        for (int col = 0; col < 6; col++)
        {
            m_frames[row][col] = bitmap->Clone(col * frameWidth, row * frameHeight,frameWidth,frameHeight, PixelFormat32bppARGB);
        }
    }

    for (int row = 0; row < 5; row++)
    {
        for (int col = 0; col < 6; col++)
        {
            m_flipFrames[row][col] = CreateFlip(m_frames[row][col]);
        }
    }*/
}

void SpriteRenderer::FixedUpdate()
{
}

void SpriteRenderer::Update(float deltaTime)
{
}

void SpriteRenderer::LateUpdate()
{
}

void SpriteRenderer::PreRender()
{
}

void SpriteRenderer::Render(ID2D1DeviceContext* context)
{
    if (!m_isVisible)
        return;
    int TILE_W = TileManager::GetInstance().GetTileWidth();
    int TILE_H = TileManager::GetInstance().GetTileHeight();

    MathEngine::Vector2 screen = TileManager::GetInstance().TileToScreen(m_transform->GetPostion());

    float screenX = screen.x - CameraManager::GetInstance().GetX();
    float screenY = screen.y - CameraManager::GetInstance().GetY();

    screenY += TILE_H / 2.0f;

    ID2D1Bitmap* bitmap = m_resourceManager->GetImage(m_imageName);
    if (bitmap == nullptr) return;

    static int logCount = 0;
    if (m_imageName == "Bullet" && logCount < 5)
    {
        logCount++;
        char buf[256];
        sprintf_s(buf, "[SpriteRender] frameW=%d frameH=%d scale=%.2f bitmap=%p\n",
            m_frameWidth, m_frameHeight, m_scale, (void*)bitmap);
        OutputDebugStringA(buf);
    }

    float scaledWidth = m_frameWidth * m_scale;
    float scaledHeight = m_frameHeight * m_scale;
    float pivotX = m_flip ? (m_frameWidth - m_pivot.x) : m_pivot.x;
    float scaledPivotX = m_pivot.x * m_scale;
    float scaledPivotY = m_pivot.y * m_scale;

    float left = screenX - scaledPivotX;
    float top = screenY - scaledPivotY;

    D2D1_RECT_F destRect = D2D1::RectF(
        left,
        top,
        left + scaledWidth,   // right = left + width
        top + scaledHeight    // bottom = top + height
    );

    D2D1_RECT_F srcRect = D2D1::RectF(
        (float)m_srcX,
        (float)m_srcY,
        (float)(m_srcX + m_frameWidth),
        (float)(m_srcY + m_frameHeight)
    );

    /*if (!m_flip)
    {
        context->DrawBitmap(
            bitmap,
            destRect,
            1.0f,
            D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
            srcRect
        );
    }
    else
    {
        D2D1_MATRIX_3X2_F oldTransform;
        context->GetTransform(&oldTransform);

        float centerX = (destRect.left + destRect.right) * 0.5f;
        float centerY = (destRect.top + destRect.bottom) * 0.5f;

        D2D1_MATRIX_3X2_F flipMatrix =
            D2D1::Matrix3x2F::Scale(-1.0f, 1.0f, D2D1::Point2F(centerX, centerY)) * oldTransform;

        context->SetTransform(flipMatrix);

        context->DrawBitmap(
            bitmap,
            destRect,
            1.0f,
            D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
            srcRect
        );

        context->SetTransform(oldTransform);
    }*/
    D2D1_MATRIX_3X2_F oldTransform;
    context->GetTransform(&oldTransform);

    float centerX = (destRect.left + destRect.right) * 0.5f;
    float centerY = (destRect.top + destRect.bottom) * 0.5f;

    D2D1_MATRIX_3X2_F finalTransform = oldTransform;

    if (m_flip)
    {
        finalTransform = D2D1::Matrix3x2F::Scale(-1.0f, 1.0f, D2D1::Point2F(centerX, centerY)) * finalTransform;
    }

    if (m_rotDeg != 0.0f)
    {
        finalTransform = D2D1::Matrix3x2F::Rotation(m_rotDeg, D2D1::Point2F(centerX, centerY)) * finalTransform;
    }

    context->SetTransform(finalTransform);

    context->DrawBitmap(
        bitmap,
        destRect,
        1.0f,
        D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
        srcRect
    );

    context->SetTransform(oldTransform);
}

void SpriteRenderer::PostRender(ID2D1DeviceContext* context)
{
}

void SpriteRenderer::Release()
{
}

ElementType SpriteRenderer::GetElementType() const
{
	return ElementType::SpriteRenderer;
}

GameObject* SpriteRenderer::GetGameObject() const
{
	return m_gameObject;
}

void SpriteRenderer::SetGameObject(GameObject* gameObject)
{
    m_gameObject = gameObject;
}
