#include "MiniMap.h"

#include "Player.h"
#include "Transform.h"
#include "TileManager.h"
#include "ResourceManager.h"

MiniMap::MiniMap() 
	:m_player(nullptr), m_resourceManager(nullptr), m_centerX(1110.0f),m_centerY(600.0f), m_width(240.f),m_height(140.0f),m_tileWidth(12.0f), m_tileHeight(6.0f)
{
}

MiniMap::~MiniMap()
{
	
}

void MiniMap::Init(Player* player, ResourceManager* resourceManager)
{
	m_player = player;
	m_resourceManager = resourceManager;
}

MathEngine::Vector2 MiniMap::TileToMiniMap(float tileX, float tileY) const
{
	float isoX =
		(tileX - tileY) *
		(m_tileWidth * 0.5f);

	float isoY =
		(tileX + tileY) *
		(m_tileHeight * 0.5f);
    float mapCenterX =
        (MAP_WIDTH - 1) * 0.5f;

    float mapCenterY =
        (MAP_HEIGHT - 1) * 0.5f;


    float centerIsoX =
        (mapCenterX - mapCenterY) *
        (m_tileWidth * 0.5f);

    float centerIsoY =
        (mapCenterX + mapCenterY) *
        (m_tileHeight * 0.5f);


    isoX -= centerIsoX;
    isoY -= centerIsoY;


    return
    {
        m_centerX + isoX,
        m_centerY + isoY
    };
}

void MiniMap::CreateDiamondGeometry(ID2D1Factory* factory, ID2D1PathGeometry** geometry)
{
    if (factory == nullptr)
        return;

    if (geometry == nullptr)
        return;


    factory->CreatePathGeometry(
        geometry);


    if (*geometry == nullptr)
        return;


    ID2D1GeometrySink* sink = nullptr;

    (*geometry)->Open(
        &sink);


    if (sink == nullptr)
        return;


    float halfWidth =
        m_width * 0.5f;

    float halfHeight =
        m_height * 0.5f;


    // 위
    sink->BeginFigure(
        D2D1::Point2F(
            m_centerX,
            m_centerY - halfHeight),
        D2D1_FIGURE_BEGIN_FILLED);


    // 오른쪽
    sink->AddLine(
        D2D1::Point2F(
            m_centerX + halfWidth,
            m_centerY));


    // 아래
    sink->AddLine(
        D2D1::Point2F(
            m_centerX,
            m_centerY + halfHeight));


    // 왼쪽
    sink->AddLine(
        D2D1::Point2F(
            m_centerX - halfWidth,
            m_centerY));


    sink->EndFigure(
        D2D1_FIGURE_END_CLOSED);


    sink->Close();

    sink->Release();
}

void MiniMap::Render(
    ID2D1DeviceContext* context)
{
    if (context == nullptr)
        return;

    if (m_player == nullptr)
        return;

    if (m_resourceManager == nullptr)
        return;
    ID2D1Factory* factory = nullptr;

    context->GetFactory(
        &factory);

    if (factory == nullptr)
        return;

    ID2D1PathGeometry* diamond = nullptr;

    CreateDiamondGeometry(
        factory,
        &diamond);

    if (diamond == nullptr)
    {
        factory->Release();
        return;
    }

    ID2D1SolidColorBrush* backgroundBrush = nullptr;

    context->CreateSolidColorBrush(
        D2D1::ColorF(
            D2D1::ColorF::Black,
            0.75f),
        &backgroundBrush);


    if (backgroundBrush)
    {
        context->FillGeometry(
            diamond,
            backgroundBrush);

        backgroundBrush->Release();
    }

    ID2D1Layer* layer = nullptr;

    context->CreateLayer(
        nullptr,
        &layer);


    if (layer == nullptr)
    {
        diamond->Release();
        factory->Release();
        return;
    }

    D2D1_LAYER_PARAMETERS layerParameters =
        D2D1::LayerParameters();

    layerParameters.contentBounds =
        D2D1::InfiniteRect();

    layerParameters.geometricMask =
        diamond;

    layerParameters.maskAntialiasMode =
        D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;

    layerParameters.opacity =
        1.0f;


    context->PushLayer(
        layerParameters,
        layer);

    ID2D1Bitmap* tileBitmap =
        m_resourceManager->GetImage(
            "Tile_W");


    if (tileBitmap != nullptr)
    {
        D2D1_SIZE_F bitmapSize =
            tileBitmap->GetSize();

        D2D1_RECT_F sourceRect =
            D2D1::RectF(
                0.0f,
                0.0f,
                bitmapSize.width,
                bitmapSize.height);
        for (int y = 0;
            y < MAP_HEIGHT;
            y++)
        {
            for (int x = 0;
                x < MAP_WIDTH;
                x++)
            {
                MathEngine::Vector2 pos =
                    TileToMiniMap(
                        (float)x,
                        (float)y);


                D2D1_RECT_F destRect =
                    D2D1::RectF(
                        pos.x -
                        m_tileWidth * 0.5f,

                        pos.y,

                        pos.x +
                        m_tileWidth * 0.5f,

                        pos.y +
                        m_tileHeight);


                DrawBitmap(
                    context,
                    tileBitmap,
                    destRect,
                    sourceRect,
                    false);
            }
        }
        MathEngine::Vector2 playerPos =
            m_player
            ->GetTransform()
            ->GetPostion();


        MathEngine::Vector2 playerMiniPos =
            TileToMiniMap(
                playerPos.x,
                playerPos.y);
        ID2D1SolidColorBrush* playerBrush = nullptr;

        context->CreateSolidColorBrush(
            D2D1::ColorF(
                D2D1::ColorF::Red),
            &playerBrush);


        if (playerBrush)
        {
            D2D1_ELLIPSE playerCircle =
                D2D1::Ellipse(
                    D2D1::Point2F(
                        playerMiniPos.x,
                        playerMiniPos.y +
                        m_tileHeight * 0.5f),

                    4.0f,
                    4.0f);


            context->FillEllipse(
                playerCircle,
                playerBrush);


            playerBrush->Release();
        }
        context->PopLayer();
        ID2D1SolidColorBrush* borderBrush = nullptr;

        context->CreateSolidColorBrush(
            D2D1::ColorF(
                D2D1::ColorF::White),
            &borderBrush);


        if (borderBrush)
        {
            context->DrawGeometry(
                diamond,
                borderBrush,
                2.0f);

            borderBrush->Release();
        }
        layer->Release();

        diamond->Release();

        factory->Release();
    }
}