#include "EnvironmentManager.h"
#include "Graphics.h"
#include "TimeManager.h"
#include "CampFire.h"
#include "TileManager.h"
#include "CameraManager.h"
#include "GameObject.h"
#include "Transform.h"
#include "Player.h"
#include "TileManager.h"

void EnvironmentManager::Init(Player* player)
{
	m_nightAlpha = 0.0f;
	m_nightBrush = nullptr;
    m_fireLightBrush = nullptr;
    m_ringLightBrush = nullptr;
    m_campFires.clear();
    m_player = player;
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
    if (context == nullptr)
        return;

    // =========================================================
    // 1. ¹ã ¿À¹ö·¹ÀÌ
    // =========================================================
    if (!m_nightBrush)
    {
        context->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Black),
            &m_nightBrush
        );
    }

    if (m_nightBrush)
    {
        m_nightBrush->SetOpacity(m_nightAlpha);

        context->FillRectangle(
            D2D1::RectF(
                0.0f,
                0.0f,
                1920.0f,
                1080.0f
            ),
            m_nightBrush
        );
    }


    // =========================================================
    // 2. ¸ð´ÚºÒ ºû
    // =========================================================

    bool hasLitFire = false;

    for (CampFire* fire : m_campFires)
    {
        if (fire != nullptr && fire->IsLit())
        {
            hasLitFire = true;
            break;
        }
    }

    // ¸ð´ÚºÒÀÌ ÀÖÀ» ¶§¸¸ ¸ð´ÚºÒ ºû ·»´õ¸µ
    if (hasLitFire)
    {
        CreateFireLight(context);

        float camX =
            CameraManager::GetInstance().GetX();

        float camY =
            CameraManager::GetInstance().GetY();

        for (CampFire* fire : m_campFires)
        {
            if (fire == nullptr || !fire->IsLit())
                continue;

            GameObject* obj = fire->GetGameObject();

            if (obj == nullptr)
                continue;

            Transform* tr =
                static_cast<Transform*>(
                    obj->GetElement(ElementType::Transform)
                    );

            if (tr == nullptr)
                continue;

            MathEngine::Vector2 worldTile =
                tr->GetPostion();

            MathEngine::Vector2 screen =
                TileManager::GetInstance().TileToScreen(worldTile);

            float screenX = screen.x - camX;
            float screenY = screen.y - camY;

            m_fireLightBrush->SetCenter(
                D2D1::Point2F(screenX, screenY)
            );

            context->FillEllipse(
                D2D1::Ellipse(
                    D2D1::Point2F(screenX, screenY),
                    m_fireLightRadius,
                    m_fireLightRadius
                ),
                m_fireLightBrush
            );
        }
    }


    // =========================================================
    // 3. ¹ß±¤¼® ¹ÝÁö ºû
    // =========================================================
    // ¸ð´ÚºÒÀÌ ¾ø¾îµµ ¹Ýµå½Ã ½ÇÇàµÇ¾î¾ß ÇÔ
    RenderRingLight(context);
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

void EnvironmentManager::CreateRingLight(ID2D1DeviceContext* context)
{
    if (m_ringLightBrush != nullptr)
        return;

    ID2D1GradientStopCollection* gradientStops = nullptr;

    D2D1_GRADIENT_STOP stops[] =
    {
        // Áß½É
        {
            0.0f,
            D2D1::ColorF(
                1.0f,
                1.0f,
                1.0f,
                0.55f
            )
        },

        // Áß°£
        {
            0.30f,
            D2D1::ColorF(
                0.95f,
                0.98f,
                1.0f,
                0.30f
            )
        },

        // ¿Ü°û
        {
            0.65f,
            D2D1::ColorF(
                0.85f,
                0.92f,
                1.0f,
                0.10f
            )
        },

        // ¿ÏÀü Åõ¸í
        {
            1.0f,
            D2D1::ColorF(
                0.8f,
                0.9f,
                1.0f,
                0.0f
            )
        }
    };

    HRESULT hr =
        context->CreateGradientStopCollection(
            stops,
            ARRAYSIZE(stops),
            D2D1_GAMMA_2_2,
            D2D1_EXTEND_MODE_CLAMP,
            &gradientStops
        );

    if (FAILED(hr) || gradientStops == nullptr)
        return;

    D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES props =
    {
        D2D1::Point2F(0.0f, 0.0f),
        D2D1::Point2F(0.0f, 0.0f),

        m_ringLightRadius,
        m_ringLightRadius
    };

    hr =
        context->CreateRadialGradientBrush(
            props,
            gradientStops,
            &m_ringLightBrush
        );

    gradientStops->Release();

    if (FAILED(hr))
    {
        m_ringLightBrush = nullptr;
    }
}

void EnvironmentManager::RenderRingLight(ID2D1DeviceContext* context)
{
    if (context == nullptr)
        return;

    if (m_player == nullptr)
        return;

    // ¹ß±¤¼® ¹ÝÁö¸¦ ÀåÂøÇÏÁö ¾Ê¾Ò´Ù¸é ºû ¾øÀ½
    if (!m_player->HasGlowRing())
        return;

    if (m_ringLightBrush == nullptr)
    {
        CreateRingLight(context);
    }

    if (m_ringLightBrush == nullptr)
        return;

    GameObject* playerObject =
        m_player->GetGameObject();

    if (playerObject == nullptr)
        return;

    Transform* transform =
        static_cast<Transform*>(
            playerObject->GetElement(
                ElementType::Transform
            )
            );

    if (transform == nullptr)
        return;

    MathEngine::Vector2 playerPos =
        transform->GetPostion();

    // ¿ùµå ÁÂÇ¥ ¡æ È­¸é ÁÂÇ¥
    MathEngine::Vector2 screenPos =
        TileManager::GetInstance().TileToScreen(playerPos);


    // CameraManager Àû¿ë
    screenPos.x -= CameraManager::GetInstance().GetX();
    screenPos.y -= CameraManager::GetInstance().GetY();

    m_ringLightBrush->SetCenter(
        D2D1::Point2F(
            screenPos.x,
            screenPos.y
        )
    );

    context->FillEllipse(
        D2D1::Ellipse(
            D2D1::Point2F(
                screenPos.x,
                screenPos.y
            ),
            m_ringLightRadius,
            m_ringLightRadius
        ),
        m_ringLightBrush
    );
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
