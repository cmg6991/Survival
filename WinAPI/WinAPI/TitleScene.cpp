#include "TitleScene.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "Graphics.h"
#include "SceneManager.h"
#include "DataManager.h"
#include "SoundManager.h"
#include "SaveManager.h"

TitleScene::TitleScene(ResourceManager* resourceManager,SceneManager* sceneManager)
	:Scene("TitleScene"), m_resourceManager(nullptr),m_startHover(nullptr), m_exitHover(false),m_loadHover(false),
    m_sceneManager(nullptr),m_backHover(false)
{
    m_resourceManager = resourceManager;
    m_sceneManager = sceneManager;
}

TitleScene::~TitleScene()
{
}

void TitleScene::Init()
{

    m_startButton = D2D1::RectF(
        490.0f,
        400.0f,
        790.0f,
        470.0f
    );

    m_loadButton = D2D1::RectF(
        490.0f,
        490.0f,
        790.0f,
        560.0f
    );

    m_exitButton = D2D1::RectF(
        490.0f,
        580.0f,
        790.0f,
        650.0f
    );

    m_slotPanel = D2D1::RectF(
        390.0f,
        70.0f,
        890.0f,
        630.0f
    );

    m_slotButtons[0] = D2D1::RectF(
        430.0f,
        150.0f,
        850.0f,
        250.0f
    );

    m_slotButtons[1] = D2D1::RectF(
        430.0f,
        270.0f,
        850.0f,
        370.0f
    );

    m_slotButtons[2] = D2D1::RectF(
        430.0f,
        390.0f,
        850.0f,
        490.0f
    );

    m_backButton = D2D1::RectF(
        540.0f,
        520.0f,
        740.0f,
        580.0f
    );

    m_startHover = false;
    m_loadHover = false;
    m_exitHover = false;

    for (int i = 0; i < 3; i++)
    {
        m_slotHover[i] = false;
    }

    m_backHover = false;

    m_showConfirm = false;
    m_selectedSlot = 0;


    for (int i = 0; i < 3; i++)
    {
        m_slotExists[i] = false;
        m_slotData[i] = SaveData();

        int slot = i + 1;

        if (SaveManager::HasSaveSlot(slot))
        {
            if (SaveManager::LoadSlot(
                m_slotData[i],
                slot))
            {
                m_slotExists[i] = true;
            }
        }
    }
}

void TitleScene::FixedUpdate()
{
}

void TitleScene::Update(float deltaTime)
{
    MathEngine::Vector2 mouse =InputManager::GetInstance().GetMousePosition();
    if (m_menuState == TitleMenuState::Main)
    {
        m_startHover =
            mouse.x >= m_startButton.left &&
            mouse.x <= m_startButton.right &&
            mouse.y >= m_startButton.top &&
            mouse.y <= m_startButton.bottom;

        m_loadHover =
            mouse.x >= m_loadButton.left &&
            mouse.x <= m_loadButton.right &&
            mouse.y >= m_loadButton.top &&
            mouse.y <= m_loadButton.bottom;

        m_exitHover =
            mouse.x >= m_exitButton.left &&
            mouse.x <= m_exitButton.right &&
            mouse.y >= m_exitButton.top &&
            mouse.y <= m_exitButton.bottom;


        if (InputManager::GetInstance().IsGetKeyDown(VK_LBUTTON))
        {

            if (m_startHover)
            {
                SoundManager::GetInstance().PlaySFX("Button",1.f);
                m_menuState = TitleMenuState::NewGameSlot;
                return;
            }

            if (m_loadHover)
            {
                SoundManager::GetInstance().PlaySFX("Button",1.f);
                m_menuState =TitleMenuState::LoadGameSlot;
                return;
            }

            if (m_exitHover)
            {
                SoundManager::GetInstance().PlaySFX("Button",1.f);
                PostQuitMessage(0);
                return;
            }
        }

        return;
    }

    for (int i = 0; i < 3; i++)
    {
        m_slotHover[i] =
            mouse.x >= m_slotButtons[i].left &&
            mouse.x <= m_slotButtons[i].right &&
            mouse.y >= m_slotButtons[i].top &&
            mouse.y <= m_slotButtons[i].bottom;
    }

    m_backHover =
        mouse.x >= m_backButton.left &&
        mouse.x <= m_backButton.right &&
        mouse.y >= m_backButton.top &&
        mouse.y <= m_backButton.bottom;


    if (InputManager::GetInstance().IsGetKeyDown(VK_LBUTTON))
    {

        for (int i = 0; i < 3; i++)
        {
            if (m_slotHover[i])
            {
                SelectSlot(i + 1);
                return;
            }
        }

        if (m_backHover)
        {
            SoundManager::GetInstance().PlaySFX(
                "Button",
                1.f
            );

            m_menuState =
                TitleMenuState::Main;

            return;
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
            0.0f,
            600.0f,
            400.0f
        );
    }

    //RenderButton(m_startButton,L"GAME START",m_startHover);
    //RenderButton(m_loadButton, L"LOAD GAME", m_loadHover);
    //RenderButton(m_exitButton,L"EXIT",m_exitHover);

    if (m_menuState == TitleMenuState::Main)
    {
        RenderButton(m_startButton,L"NEW GAME",m_startHover);
        RenderButton(m_loadButton, L"LOAD GAME", m_loadHover);
        RenderButton(m_exitButton,L"EXIT",m_exitHover);
    }
    else
    {
        RenderSlotPanel();

        // 제목
        RenderSlotTitle();

        // 슬롯 3개
        for (int i = 0; i < 3; i++)
        {
            RenderSlotButton(
                m_slotButtons[i],
                i + 1,
                m_slotHover[i]
            );
        }

        // 뒤로가기
        RenderButton(
            m_backButton,
            L"BACK",
            m_backHover
        );
        RenderButton(m_backButton,L"BACK",m_backHover);
    }

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
                0.92f,
                0.87f,
                0.75f,
                1.0f
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

    /*D2D1::ColorF textColor =
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
        );*/
    D2D1::ColorF textColor = D2D1::ColorF(0.05f,
        0.05f,
        0.05f,
        1.0f);

    GRAPHICS.DrawString(
        text,
        textX,
        textY,
        textColor,
        fontSize
    );
}

void TitleScene::SelectSlot(int slot)
{
    SoundManager::GetInstance().PlaySFX("Button",1.f);


    if (m_menuState == TitleMenuState::NewGameSlot)
    {
        // 기존 세이브가 있으면 확인창
        if (SaveManager::HasSaveSlot(slot))
        {
            m_selectedSlot = slot;
            m_showConfirm = true;
            return;
        }
        // 기존 세이브가 없으면 바로 시작
        m_sceneManager->SetGameStartType(GameStartType::NewGame);
        m_sceneManager->SetSaveSlot(slot);
        m_sceneManager->LoadScene("Main");
    }

    if (m_menuState == TitleMenuState::LoadGameSlot)
    {
        if (!SaveManager::HasSaveSlot(slot))
        {
            OutputDebugStringW(L"선택한 슬롯에 세이브 파일이 없습니다.\n");
            return;
        }
        m_sceneManager->SetGameStartType(GameStartType::LoadGame);
        m_sceneManager->SetSaveSlot(slot);
        m_sceneManager->LoadScene("Main");
        return;
    }
}

void TitleScene::RenderSlotButton(const D2D1_RECT_F& rect, int slot, bool hover)
{
    float width =
        rect.right - rect.left;

    float height =
        rect.bottom - rect.top;

    D2D1::ColorF background =
        hover
        ? D2D1::ColorF(
            0.92f,
            0.87f,
            0.75f,
            1.0f
        )
        : D2D1::ColorF(
            0.08f,
            0.08f,
            0.08f,
            0.9f
        );

    GRAPHICS.FillRoundedRect(
        rect.left,
        rect.top,
        width,
        height,
        12.0f,
        background
    );

    GRAPHICS.DrawRoundedRect(
        rect.left,
        rect.top,
        width,
        height,
        12.0f,
        hover
        ? D2D1::ColorF(
            1.0f,
            1.0f,
            1.0f,
            0.8f
        )
        : D2D1::ColorF(
            0.92f,
            0.87f,
            0.75f,
            1.0f
        ),
        2.0f
    );

    int index = slot - 1;

    float textX =
        rect.left + 20.0f;

    float textY =
        rect.top + 12.0f;

    D2D1::ColorF textColor =
        hover
        ? D2D1::ColorF(
            0.05f,
            0.05f,
            0.05f
        )
        : D2D1::ColorF(
            1.0f,
            1.0f,
            1.0f
        );

    wchar_t slotText[64];

    swprintf_s(
        slotText,
        L"SLOT %d",
        slot
    );

    GRAPHICS.DrawString(
        slotText,
        textX,
        textY,
        textColor,
        22.0f
    );

    // 비어있는 슬롯
    if (!m_slotExists[index])
    {
        GRAPHICS.DrawString(
            L"EMPTY",
            textX,
            textY + 35.0f,
            hover
            ? D2D1::ColorF(
                0.3f,
                0.3f,
                0.3f
            )
            : D2D1::ColorF(
                0.65f,
                0.65f,
                0.65f
            ),
            16.0f
        );

        return;
    }

    // 플레이 시간
    int totalSeconds =m_slotData[index].playTimeSeconds;
    int hours =totalSeconds / 3600;
    int minutes =(totalSeconds % 3600) / 60;
    int seconds =totalSeconds % 60;

    wchar_t info[128];

    swprintf_s(
        info,
        L"PLAY TIME  %02d:%02d:%02d",
        hours,
        minutes,
        seconds
    );

    GRAPHICS.DrawString(
        info,
        textX,
        textY + 35.0f,
        textColor,
        16.0f
    );
}

void TitleScene::RenderSlotPanel()
{
    float width =m_slotPanel.right - m_slotPanel.left;
    float height =m_slotPanel.bottom - m_slotPanel.top;

    GRAPHICS.FillRoundedRect(
        m_slotPanel.left,
        m_slotPanel.top,
        width,
        height,
        20.0f,
        D2D1::ColorF(
            0.03f,
            0.03f,
            0.03f,
            0.85f
        )
    );

    GRAPHICS.DrawRoundedRect(
        m_slotPanel.left,
        m_slotPanel.top,
        width,
        height,
        20.0f,
        D2D1::ColorF(
            1.0f,
            1.0f,
            1.0f,
            0.85f
        ),
        2.0f
    );
}

void TitleScene::RenderSlotTitle()
{
    const wchar_t* title =
        m_menuState == TitleMenuState::NewGameSlot
        ? L"NEW GAME"
        : L"LOAD GAME";

    float fontSize = 32.0f;

    float textWidth =
        GRAPHICS.MeasureTextWidth(
            title,
            fontSize
        );

    float panelCenter =
        (m_slotPanel.left + m_slotPanel.right) * 0.5f;

    float textX =
        panelCenter - textWidth * 0.5f;

    GRAPHICS.DrawString(
        title,
        textX,
        100.0f,
        D2D1::ColorF(
            1.0f,
            1.0f,
            1.0f
        ),
        fontSize
    );
}

void TitleScene::RenderConfirmWindow()
{
}

bool TitleScene::IsSlotOccupied(int slot)
{
    return false;
}
