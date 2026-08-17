#include "TitleScene.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "Graphics.h"
#include "SceneManager.h"
#include "DataManager.h"

TitleScene::TitleScene(ResourceManager* resourceManager,SceneManager* sceneManager)
	:Scene("TitleScene"), m_resourceManager(nullptr),m_startHover(nullptr), m_exitHover(false),
    m_sceneManager(nullptr)
{
    m_resourceManager = resourceManager;
    m_sceneManager = sceneManager;
}

TitleScene::~TitleScene()
{
}

void TitleScene::Init()
{
    m_resourceManager->Init();
    for (const ImageData& img : DataManager::GetInstance().GetImageList())
    {
        m_resourceManager->AddImage(img.keyString, img.path);
    }


    m_startButton = D2D1::RectF(
        490.0f,
        400.0f,
        790.0f,
        470.0f
    );

    m_exitButton = D2D1::RectF(
        490.0f,
        490.0f,
        790.0f,
        560.0f
    );

    m_startHover = false;
    m_exitHover = false;
}

void TitleScene::FixedUpdate()
{
}

void TitleScene::Update(float deltaTime)
{
    MathEngine::Vector2 mouse =
        InputManager::GetInstance().GetMousePosition();

    m_startHover =
        mouse.x >= m_startButton.left &&
        mouse.x <= m_startButton.right &&
        mouse.y >= m_startButton.top &&
        mouse.y <= m_startButton.bottom;

    m_exitHover =
        mouse.x >= m_exitButton.left &&
        mouse.x <= m_exitButton.right &&
        mouse.y >= m_exitButton.top &&
        mouse.y <= m_exitButton.bottom;


    if (InputManager::GetInstance().IsGetKeyDown(VK_LBUTTON))
    {
        if (m_startHover)
        {
            OutputDebugStringW(L"GAME START CLICKED\n");

            m_sceneManager->LoadScene("Main");
        }

        if (m_exitHover)
        {
            PostQuitMessage(0);
        }
    }
}

void TitleScene::LateUpdate()
{
}

void TitleScene::PreRender()
{
}

void TitleScene::Render(ID2D1DeviceContext* context)
{
    ID2D1Bitmap* background =m_resourceManager->GetImage("TitleBackGround");

    if (background != nullptr)
    {
        GRAPHICS.DrawBitmapUI(
            background,
            0.0f,
            0.0f,
            1280.0f,
            720.0f
        );
    }


    ID2D1Bitmap* logo =m_resourceManager->GetImage("TitleLogo");

    if (logo != nullptr)
    {
        GRAPHICS.DrawBitmapUI(
            logo,
            340.0f,
            80.0f,
            600.0f,
            200.0f
        );
    }

    RenderButton(m_startButton,L"GAME START",m_startHover);
    RenderButton(m_exitButton,L"EXIT",m_exitHover);

}

void TitleScene::PostRender(ID2D1DeviceContext* context)
{
}

void TitleScene::Release()
{
    m_resourceManager = nullptr;
}

void TitleScene::RenderButton(const D2D1_RECT_F& rect, const wchar_t* text, bool hover)
{
    if (text == nullptr)
        return;

    float width = rect.right - rect.left;
    float height =rect.bottom - rect.top;

    if (hover)
    {
        GRAPHICS.FillRoundedRect(
            rect.left,
            rect.top,
            width,
            height,
            12.0f,
            D2D1::ColorF(
                1.0f,
                1.0f,
                1.0f,
                0.95f
            )
        );

        GRAPHICS.DrawRoundedRect(
            rect.left,
            rect.top,
            width,
            height,
            12.0f,
            D2D1::ColorF(
                1.0f,
                1.0f,
                1.0f,
                1.0f
            ),
            2.0f
        );
    }
    else
    {
        GRAPHICS.FillRoundedRect(
            rect.left,
            rect.top,
            width,
            height,
            12.0f,
            D2D1::ColorF(
                0.05f,
                0.05f,
                0.05f,
                0.85f
            )
        );

        GRAPHICS.DrawRoundedRect(
            rect.left,
            rect.top,
            width,
            height,
            12.0f,
            D2D1::ColorF(
                1.0f,
                1.0f,
                1.0f,
                0.5f
            ),
            2.0f
        );
    }

    float fontSize = 24.0f;
    float textWidth =GRAPHICS.MeasureTextWidth(text,fontSize);

    float textX =rect.left +(width - textWidth) * 0.5f;
    float textY =rect.top +(height - fontSize) * 0.5f;

    D2D1::ColorF textColor =
        hover
        ? D2D1::ColorF(
            0.05f,
            0.05f,
            0.05f,
            1.0f
        )
        : D2D1::ColorF(
            1.0f,
            1.0f,
            1.0f,
            1.0f
        );

    GRAPHICS.DrawString(
        text,
        textX,
        textY,
        textColor,
        fontSize
    );
}
