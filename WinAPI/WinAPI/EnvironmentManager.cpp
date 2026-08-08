#include "EnvironmentManager.h"
#include "Graphics.h"
#include "TimeManager.h"
#include "CampFire.h"
#include "TileManager.h"
#include "CameraManager.h"
#include "GameObject.h"
#include "Transform.h"

void EnvironmentManager::Init()
{
	m_nightAlpha = 0.0f;
	m_nightBrush = nullptr;
    m_fireLightBrush = nullptr;
    m_campFires.clear();
}

void EnvironmentManager::Update(float deltaTime)
{
    float targetAlpha = GetNightAlpha();

    float speed = 0.5f;

    if (m_nightAlpha < targetAlpha)
    {
        m_nightAlpha += speed * deltaTime;

        if (m_nightAlpha > targetAlpha)
            m_nightAlpha = targetAlpha;
    }
    else if (m_nightAlpha > targetAlpha)
    {
        m_nightAlpha -= speed * deltaTime;

        if (m_nightAlpha < targetAlpha)
            m_nightAlpha = targetAlpha;
    }
}

void EnvironmentManager::Render(ID2D1DeviceContext* context)
{
    // 1. 밤 오버레이 (낮엔 알파 0이라 사실상 안 그려지는 것과 같음)
    if (!m_nightBrush)
    {
        context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_nightBrush);
    }
    m_nightBrush->SetOpacity(m_nightAlpha);
    context->FillRectangle(D2D1::RectF(0.0f, 0.0f, 1920.0f, 1080.0f), m_nightBrush);

    // 2. 모닥불 불빛은 시간대와 무관하게, 켜져있는 애만 그림
    bool hasLitFire = false;
    for (CampFire* fire : m_campFires)
    {
        if (fire != nullptr && fire->IsLit())
        {
            hasLitFire = true;
            break;
        }
    }
    if (!hasLitFire)
        return;

    CreateFireLight(context);

    float camX = CameraManager::GetInstance().GetX();
    float camY = CameraManager::GetInstance().GetY();

    for (CampFire* fire : m_campFires)
    {
        if (fire == nullptr || !fire->IsLit())
            continue;

        GameObject* obj = fire->GetGameObject();
        if (obj == nullptr) continue;

        Transform* tr = static_cast<Transform*>(obj->GetElement(ElementType::Transform));
        if (tr == nullptr) continue;

        MathEngine::Vector2 worldTile = tr->GetPostion();
        MathEngine::Vector2 screen = TileManager::GetInstance().TileToScreen(worldTile);

        float screenX = screen.x - camX;
        float screenY = screen.y - camY;

        m_fireLightBrush->SetCenter(D2D1::Point2F(screenX, screenY));
        context->FillEllipse(
            D2D1::Ellipse(D2D1::Point2F(screenX, screenY), m_fireLightRadius, m_fireLightRadius),
            m_fireLightBrush
        );
    }

}

void EnvironmentManager::RegisterCampFire(CampFire* fire)
{
    m_campFires.push_back(fire);
}

void EnvironmentManager::UnregisterCampFire(CampFire* fire)
{
    for (auto it = m_campFires.begin(); it != m_campFires.end(); ++it)
    {
        if (*it == fire)
        {
            m_campFires.erase(it);
            break;
        }
    }
}

void EnvironmentManager::CreateFireLight(ID2D1DeviceContext* context)
{
    if (m_fireLightBrush)
        return;

    ID2D1GradientStopCollection* gradientStops = nullptr;

    D2D1_GRADIENT_STOP stops[] =
    {
        { 0.0f, D2D1::ColorF(1.0f, 0.7f, 0.2f, 0.45f) },
        { 0.4f, D2D1::ColorF(1.0f, 0.4f, 0.1f, 0.2f) },
        { 1.0f, D2D1::ColorF(1.0f, 0.2f, 0.0f, 0.0f) }
    };

    context->CreateGradientStopCollection(
        stops,
        ARRAYSIZE(stops),
        D2D1_GAMMA_2_2,
        D2D1_EXTEND_MODE_CLAMP,
        &gradientStops
    );

    D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES props =
    {
        D2D1::Point2F(0.0f, 0.0f),
        D2D1::Point2F(0.0f, 0.0f),
        m_fireLightRadius,
        m_fireLightRadius
    };

    context->CreateRadialGradientBrush(
        props,
        gradientStops,
        &m_fireLightBrush
    );

    gradientStops->Release();
}

float EnvironmentManager::GetNightAlpha()
{
    TimePhase phase = TimeManager::GetInstance().GetTimePhase();

    switch (phase)
    {
    case TimePhase::Morning:
        return 0.0f;

    case TimePhase::Day:
        return 0.0f;

    case TimePhase::Evening:
        return 0.3f;

    case TimePhase::Night:
        return 0.7f;
    }

    return 0.0f;

}
