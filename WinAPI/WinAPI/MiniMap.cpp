#include "MiniMap.h"

#include "Player.h"
#include "Transform.h"
#include "TileManager.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Interactable.h"
#include "ItemPickUp.h"
#include "Monster.h"

MiniMap::MiniMap() 
	:m_player(nullptr), m_resourceManager(nullptr), m_centerX(1100.0f),m_centerY(550.0f), m_width(360.f),m_height(220.0f),m_tileWidth(12.0f), m_tileHeight(6.0f)
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
    MathEngine::Vector2 playerPos = m_player->GetTransform()->GetPostion();

    // 플레이어 기준 상대 좌표
    float relativeX = tileX - playerPos.x;
    float relativeY = tileY - playerPos.y;

    // 평면(직교) 투영: 그냥 타일 크기만큼 스케일
    float mapX = relativeX * m_tileWidth;
    float mapY = relativeY * m_tileHeight;

    return
    {
        m_centerX + mapX,
        m_centerY + mapY
    };
}

void MiniMap::CreateDiamondGeometry(ID2D1Factory* factory, ID2D1PathGeometry** geometry)
{
    if (factory == nullptr)
        return;
    if (geometry == nullptr)
        return;
    factory->CreatePathGeometry(geometry);
    if (*geometry == nullptr)
        return;
    ID2D1GeometrySink* sink = nullptr;
    (*geometry)->Open(&sink);

    if (sink == nullptr)
        return;

    float halfWidth = m_width * 0.5f;
    float halfHeight = m_height * 0.5f;

    // 위
    sink->BeginFigure(D2D1::Point2F(m_centerX,m_centerY - halfHeight), D2D1_FIGURE_BEGIN_FILLED);
    //오른쪽
    sink->AddLine( D2D1::Point2F( m_centerX + halfWidth,m_centerY));
    // 아래
    sink->AddLine(D2D1::Point2F(m_centerX, m_centerY + halfHeight));
    // 왼쪽
    sink->AddLine( D2D1::Point2F(m_centerX - halfWidth,m_centerY));
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    sink->Release();
}

void MiniMap::RenderObjects(ID2D1DeviceContext* context, const std::vector<GameObject*>& objects)
{
	if (context == nullptr)
		return;

	// 미니맵 오브젝트는 픽셀 단위로 선명하게 표시
	context->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

	for (GameObject* obj : objects)
	{
		if (obj == nullptr)
			continue;

		if (!obj->GetActive())
			continue;

		Transform* tr = static_cast<Transform*>(obj->GetElement(ElementType::Transform));
		if (tr == nullptr)
			continue;

		D2D1_COLOR_F color;
		float radius = 3.0f;
		bool shouldDraw = true;

		if (obj->GetElement(ElementType::Wall) != nullptr)
		{
			color = D2D1::ColorF(D2D1::ColorF::Gray);
			radius = m_tileWidth * 0.5f; // 벽은 타일 하나 크기만큼
		}
		else if (obj->GetElement(ElementType::Monster) != nullptr)
		{
			Monster* monster = static_cast<Monster*>(obj->GetElement(ElementType::Monster));
			if (monster != nullptr && monster->IsDead())
			{
				shouldDraw = false; // 죽은 몬스터는 표시 안 함
			}
			color = D2D1::ColorF(D2D1::ColorF::Orange);
			radius = 6.0f;
		}
		else if (obj->GetElement(ElementType::Interactable) != nullptr)
		{
			color = D2D1::ColorF(D2D1::ColorF::Yellow);
			radius = 6.0;
		}
		else if (obj->GetElement(ElementType::ItemPickUp) != nullptr)
		{
			color = D2D1::ColorF(D2D1::ColorF::Cyan);
			radius = 4.5f;
		}
		else
		{
			// 표시할 필요 없는 오브젝트(총알, 이펙트 등)는 건너뜀
			continue;
		}

		if (!shouldDraw)
			continue;

		MathEngine::Vector2 pos = TileToMiniMap(tr->GetPostion().x, tr->GetPostion().y);

		ID2D1SolidColorBrush* brush = nullptr;
		context->CreateSolidColorBrush(color, &brush);

		if (brush != nullptr)
		{
			if (obj->GetElement(ElementType::Wall) != nullptr)
			{
				// 벽은 사각형으로
				D2D1_RECT_F rect = D2D1::RectF(
					pos.x - radius, pos.y - radius,
					pos.x + radius, pos.y + radius);
				context->FillRectangle(rect, brush);
			}
			else
			{
				// 나머지는 점으로
				D2D1_ELLIPSE ellipse = D2D1::Ellipse(
					D2D1::Point2F(pos.x, pos.y), radius, radius);
				context->FillEllipse(ellipse, brush);
			}

			brush->Release();
		}
		context->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	}
}

void MiniMap::Render(ID2D1DeviceContext* context, const std::vector<GameObject*>& objects)
{
	if (context == nullptr)
		return;

	if (m_player == nullptr)
		return;

	if (m_resourceManager == nullptr)
		return;

	ID2D1Factory* factory = nullptr;
	context->GetFactory(&factory);

	if (factory == nullptr)
		return;

	ID2D1PathGeometry* diamond = nullptr;
	CreateDiamondGeometry(factory, &diamond);

	if (diamond == nullptr)
	{
		factory->Release();
		return;
	}

	// 배경
	ID2D1SolidColorBrush* backgroundBrush = nullptr;
	context->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black, 0.75f),
		&backgroundBrush);

	if (backgroundBrush)
	{
		context->FillGeometry(diamond, backgroundBrush);
		backgroundBrush->Release();
	}

	// 마름모 마스크로 클리핑
	ID2D1Layer* layer = nullptr;
	context->CreateLayer(nullptr, &layer);

	if (layer == nullptr)
	{
		diamond->Release();
		factory->Release();
		return;
	}

	D2D1_LAYER_PARAMETERS layerParameters = D2D1::LayerParameters();
	layerParameters.contentBounds = D2D1::InfiniteRect();
	layerParameters.geometricMask = diamond;
	layerParameters.maskAntialiasMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
	layerParameters.opacity = 1.0f;

	context->PushLayer(layerParameters, layer);

	// ---- 여기서부터 마름모 안쪽에 그려질 내용 ----

	RenderObjects(context, objects);

	MathEngine::Vector2 playerPos = m_player->GetTransform()->GetPostion();

	// 미니맵에 표시할 범위(타일 단위)를 미니맵 크기 기준으로 계산
	int rangeX = (int)((m_width * 0.5f) / m_tileWidth) + 1;
	int rangeY = (int)((m_height * 0.5f) / m_tileHeight) + 1;

	int playerTileX = (int)playerPos.x;
	int playerTileY = (int)playerPos.y;

	int startX = playerTileX - rangeX;
	int endX = playerTileX + rangeX;
	int startY = playerTileY - rangeY;
	int endY = playerTileY + rangeY;

	// 지형(바닥) 색상 - 단색 사각형으로 표시 (아이소 텍스처 깨짐 방지)
	ID2D1SolidColorBrush* groundBrush = nullptr;
	context->CreateSolidColorBrush(
		D2D1::ColorF(0x1A1A1A), // 원하는 색으로 조정 가능
		&groundBrush);

	if (groundBrush)
	{
		for (int y = startY; y <= endY; y++)
		{
			for (int x = startX; x <= endX; x++)
			{
				MathEngine::Vector2 pos = TileToMiniMap((float)x, (float)y);

				D2D1_RECT_F destRect = D2D1::RectF(
					pos.x - m_tileWidth * 0.5f,
					pos.y - m_tileHeight * 0.5f,
					pos.x + m_tileWidth * 0.5f,
					pos.y + m_tileHeight * 0.5f);

				context->FillRectangle(destRect, groundBrush);
			}
		}

		groundBrush->Release();
	}

	// 플레이어 위치 (항상 미니맵 중앙)
	MathEngine::Vector2 playerMiniPos = TileToMiniMap(playerPos.x, playerPos.y);

	ID2D1SolidColorBrush* playerBrush = nullptr;
	context->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Red),
		&playerBrush);

	if (playerBrush)
	{
		D2D1_ELLIPSE playerCircle = D2D1::Ellipse(
			D2D1::Point2F(playerMiniPos.x, playerMiniPos.y),
			4.0f, 4.0f);

		context->FillEllipse(playerCircle, playerBrush);
		playerBrush->Release();
	}

	// ---- 마스크 해제 ----
	context->PopLayer();

	// 테두리
	ID2D1SolidColorBrush* borderBrush = nullptr;
	context->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::White),
		&borderBrush);

	if (borderBrush)
	{
		context->DrawGeometry(diamond, borderBrush, 2.0f);
		borderBrush->Release();
	}

	layer->Release();
	diamond->Release();
	factory->Release();
}