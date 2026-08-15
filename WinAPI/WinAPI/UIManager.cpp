#include "UIManager.h"
#include "TimeManager.h"
#include "Graphics.h"
#include "DataManager.h"
#include "Inventory.h"
#include "CraftingManager.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "Player.h"
#include "Weapon.h"

wstring UTF8ToWString(const string& str);

void UIManager::Update(float deltaTime)
{
	if (m_messageTimer > 0.0f)
	{
		m_messageTimer -= deltaTime;
		if (m_messageTimer <= 0.0f)
		{
			m_message.clear();
		}
	}
	if (m_isCraftingOpen) // 실제 플래그 이름은 프로젝트에 맞게
	{
		int wheel = InputManager::GetInstance().GetWheelPower();
		if (wheel != 0)
		{
			ScrollCraftingRecipe(wheel > 0 ? -1 : 1);
		}
	}
}

void UIManager::Render(ID2D1DeviceContext* context)
{
	context->SetTransform(D2D1::Matrix3x2F::Identity());
	RenderTime(context);
	RenderInventory(context);
	RenderInteractionHint(context);
	RenderMessage(context);
	RenderHP(context);

	if (m_isCraftingOpen)
	{
		RenderCrafting(context);
	}

	if (m_isInventoryOpen)
	{
		RenderInventoryWindow(context);
	}

}

void UIManager::ShowMessage(const wstring& message, float duration)
{
	m_message = message;
	m_messageTimer = duration;
}

void UIManager::MoveSelection(int delta, int maxCount)
{
	if (maxCount <= 0) return;
	m_selectedRecipeIndex += delta;
	if (m_selectedRecipeIndex < 0) m_selectedRecipeIndex = 0;
	if (m_selectedRecipeIndex >= maxCount) m_selectedRecipeIndex = maxCount - 1;
	if (m_selectedRecipeIndex < m_craftRecipeScrollOffset)
	{
		m_craftRecipeScrollOffset = m_selectedRecipeIndex;
	}
	else if (m_selectedRecipeIndex >= m_craftRecipeScrollOffset + m_craftRecipeVisibleCount)
	{
		m_craftRecipeScrollOffset = m_selectedRecipeIndex - m_craftRecipeVisibleCount + 1;
	}
}

void UIManager::ToggleInventoryWindow()
{
	m_isInventoryOpen = !m_isInventoryOpen;
}

void UIManager::HandleCraftingInventoryClick(float mouseX, float mouseY)
{
	OutputDebugStringA(("클릭 좌표: " + std::to_string(mouseX) + ", " + std::to_string(mouseY) + "\n").c_str());
	if (m_inventory == nullptr) return;
	
	vector<pair<string, int>> itemList(m_inventory->GetAllItems().begin(), m_inventory->GetAllItems().end());

	for (int i = 0; i < (int)itemList.size(); i++)
	{
		int row = i / m_slotsPerRow;
		int col = i % m_slotsPerRow;

		float slotX = m_craftInvStartX + col * (m_slotSize + m_slotPadding);
		float slotY = m_craftInvStartY + row * (m_slotSize + m_slotPadding);

		if (mouseX >= slotX && mouseX <= slotX + m_slotSize &&
			mouseY >= slotY && mouseY <= slotY + m_slotSize)
		{
			OutputDebugStringA(("슬롯 " + std::to_string(i) + " 클릭 감지! 아이템: " + itemList[i].first + "\n").c_str());

			// 클릭한 아이템이 재료로 쓰이는 첫 번째 레시피를 찾아서 자동 선택
			string station;
			switch (m_craftingStation)
			{
			case InteractType::CampFire:  station = "CampFire"; break;
			case InteractType::WorkTable: station = "WorkTable"; break;
			}

			vector<RecipeData> recipes = DataManager::GetInstance().GetRecipesByStation(station);
			for (int r = 0; r < (int)recipes.size(); r++)
			{
				for (const Ingredient& ing : recipes[r].ingredients)
				{
					if (ing.ingredientId == itemList[i].first)
					{
						m_selectedRecipeIndex = r;
						return;
					}
				}
			}
			return;
		}
	}
}

void UIManager::HandleInventoryClick(float mouseX, float mouseY)
{
	if (m_inventory == nullptr)
		return;

	vector<pair<string, int>> itemList(
		m_inventory->GetAllItems().begin(),
		m_inventory->GetAllItems().end()
	);

	for (int i = 0; i < (int)itemList.size(); i++)
	{
		int row = i / m_slotsPerRow;
		int col = i % m_slotsPerRow;

		float slotX = m_slotStartX + col * (m_slotSize + m_slotPadding);
		float slotY = m_slotStartY + row * (m_slotSize + m_slotPadding);


		if (mouseX >= slotX && mouseX <= slotX + m_slotSize &&
			mouseY >= slotY && mouseY <= slotY + m_slotSize)
		{
			string itemId = itemList[i].first;

			const ItemData* item =
				DataManager::GetInstance().FindItem(itemId);

			if (item->type == "Weapon")
			{
				EquipItem(itemId);
			}
			else if (item->type == "Shield")
			{
				EquipShield(itemId);
			}
			else if (item->type == "Food")
			{
				if (m_onItemUse)
					m_onItemUse(itemId);
			}

			return;
		}
	}

	// 2. 장비 슬롯(무기) 클릭 -> 해제
	if (mouseX >= m_equipSlotX && mouseX <= m_equipSlotX + m_slotSize &&
		mouseY >= m_equipWeaponSlotY && mouseY <= m_equipWeaponSlotY + m_slotSize)
	{
		if (m_onWeaponUnequip)
			m_onWeaponUnequip();
		return;
	}

	if (mouseX >= m_equipSlotX && mouseX <= m_equipSlotX + m_slotSize &&
		mouseY >= m_equipShieldSlotY && mouseY <= m_equipShieldSlotY + m_slotSize)
	{
		if (m_onShieldUnequip)
			m_onShieldUnequip();
		return;
	}
}

bool UIManager::HandleCraftingRecipeClick(float mouseX, float mouseY)
{
	string station;

	switch (m_craftingStation)
	{
	case InteractType::CampFire:
		station = "CampFire";
		break;

	case InteractType::WorkTable:
		station = "WorkTable";
		break;
	}

	vector<RecipeData> recipes =
		DataManager::GetInstance().GetRecipesByStation(station);

	if (recipes.empty())
		return false;

	float y = m_craftRecipeStartY;

	int endIndex = min(
		(int)recipes.size(),
		m_craftRecipeScrollOffset + m_craftRecipeVisibleCount
	);

	for (int i = m_craftRecipeScrollOffset; i < endIndex; i++)
	{
		// 레시피 한 칸 영역
		if (mouseX >= m_craftRecipeStartX &&
			mouseX <= m_craftRecipeStartX + 400 &&
			mouseY >= y &&
			mouseY <= y + 70)
		{
			m_selectedRecipeIndex = i;

			OutputDebugStringA(
				("선택 레시피 : " + recipes[i].id + "\n").c_str()
			);

			return true;
		}

		y += 75;
	}

	return false;
}

void UIManager::EquipItem(const string& itemId)
{
	if (m_player == nullptr)
		return;
	Inventory* inventory = m_player->GetInventory();

	if (inventory->HasEnough(itemId, 1))
	{
		// 인벤토리에서 제거
		//inventory->RemoveItem(itemId, 1);

		if (m_onWeaponEquip)
			m_onWeaponEquip(itemId); // 실제 GameObject 생성/장착은 MainScene에 위임

		ShowMessage(L"장착했습니다");
	}
	else
	{
		ShowMessage(L"아이템이 없습니다");
	}
}

void UIManager::EquipShield(const string& itemId)
{
	if (m_player == nullptr)
		return;
	Inventory* inventory = m_player->GetInventory();

	if (inventory->HasEnough(itemId, 1))
	{
		if (m_onShieldEquip)
			m_onShieldEquip(itemId);

		ShowMessage(L"방패를 장착했습니다");
	}
	else
	{
		ShowMessage(L"아이템이 없습니다");
	}
}

void UIManager::ScrollCraftingRecipe(int direction)
{
	/*m_craftRecipeScrollOffset += direction;
	if (m_craftRecipeScrollOffset < 0)
		m_craftRecipeScrollOffset = 0;*/
	int step = (direction > 0) ? 1 : (direction < 0) ? -1 : 0;

	m_craftRecipeScrollOffset += step;

	// 현재 스테이션의 실제 레시피 개수를 조회해서 상한까지 clamp
	string station;
	switch (m_craftingStation)
	{
	case InteractType::CampFire:  station = "CampFire"; break;
	case InteractType::WorkTable: station = "WorkTable"; break;
	}

	int recipeCount = (int)DataManager::GetInstance().GetRecipesByStation(station).size();
	int maxOffset = max(0, recipeCount - m_craftRecipeVisibleCount);

	if (m_craftRecipeScrollOffset < 0)
		m_craftRecipeScrollOffset = 0;
	if (m_craftRecipeScrollOffset > maxOffset)
		m_craftRecipeScrollOffset = maxOffset;
}

void UIManager::RenderTime(ID2D1DeviceContext* context)
{
	float x = 1050.0f;
	float y = 10.0f;

	float uiWidth = 200.0f;
	float uiHeight = 100.0f;

	// 현재 시간대
	const wchar_t* phase = TimeManager::GetInstance().GetPhaseString();

	ID2D1Bitmap* timeUI = nullptr;

	if (wcscmp(phase, L"저녁") == 0 || wcscmp(phase, L"밤") == 0)
	{
		timeUI =m_resourceManager->GetImage("Night");
	}
	else
	{
		timeUI = m_resourceManager->GetImage("Day");
	}

	if (timeUI != nullptr)
	{
		GRAPHICS.DrawBitmapUI(
			timeUI,
			x,
			y - 30.f,
			uiWidth,
			uiHeight
		);
	}
	int day = TimeManager::GetInstance().GetDay();
	int hour = TimeManager::GetInstance().GetHour();
	int minute = TimeManager::GetInstance().GetMinute();

	wchar_t dayText[32];
	swprintf_s(dayText,L"DAY %d",day);
	float dayFontSize = 30.0f;
	float dayTextWidth =GRAPHICS.MeasureTextWidth(dayText,dayFontSize);
	float dayX =x + (uiWidth - dayTextWidth) * 0.5f;
	float dayY =y + 5.0f;

	GRAPHICS.DrawString(
		dayText,
		dayX +15.f,
		dayY,
		D2D1::ColorF(D2D1::ColorF::Black),
		dayFontSize
	);
	wchar_t timeText[32];

	swprintf_s(timeText,L"%02d : %02d",hour,minute);
	float timeFontSize = 30.0f;
	float timeTextWidth =GRAPHICS.MeasureTextWidth(timeText,timeFontSize);
	float timeX = x + (uiWidth - timeTextWidth) * 0.5f;
	float timeY =y + 48.0f;

	GRAPHICS.DrawString(
		timeText,
		timeX,
		timeY,
		D2D1::ColorF(D2D1::ColorF::Black),
		timeFontSize
	);
}

void UIManager::RenderInventory(ID2D1DeviceContext* context)
{
	if (m_inventory == nullptr)
		return;

	float startX = 40.0f;
	float startY = 90.0f;

	float invWidth = 80.0f;
	float invHeight = 240.0f;

	GRAPHICS.FillRoundedRect(
		startX,
		startY,
		invWidth,
		invHeight,
		10.0f,
		D2D1::ColorF(
			0.92f,
			0.87f,
			0.75f,
			1.0f
		)
	);

	// 전체 인벤토리 검정 테두리
	GRAPHICS.DrawRoundedRect(
		startX,
		startY,
		invWidth,
		invHeight,
		10.0f,
		D2D1::ColorF::Black,
		2.0f
	);

	const float slotSize = 60.0f;

	const float slotX =startX + 10.0f;

	const float slotY[3] =
	{
		startY + 10.0f,
		startY + 90.0f,
		startY + 170.0f
	};

	// ==================================================
	// 인벤토리 아이템
	// ==================================================

	vector<pair<string, int>> items(m_inventory->GetAllItems().begin(),	m_inventory->GetAllItems().end());

	for (int i = 0; i < 3; i++)
	{
		GRAPHICS.FillRoundedRect(
			slotX,
			slotY[i],
			slotSize,
			slotSize,
			6.0f,
			D2D1::ColorF(0.97f,0.94f,0.85f,1.0f));

		GRAPHICS.DrawRoundedRect(
			slotX,
			slotY[i],
			slotSize,
			slotSize,
			6.0f,
			D2D1::ColorF::Black,
			2.0f
		);

		if (i >= static_cast<int>(items.size()))
			continue;
		const ItemData* item =
			DataManager::GetInstance()
			.FindItem(items[i].first);

		if (item == nullptr)
			continue;
		ID2D1Bitmap* icon =m_resourceManager->GetImage(item->image);

		if (icon != nullptr)
		{
			GRAPHICS.DrawBitmapUI(
				icon,
				slotX,
				slotY[i],
				slotSize,
				slotSize
			);
		}

		wchar_t count[10];

		swprintf_s(count,L"%d",items[i].second);

		GRAPHICS.DrawString(
			count,
			slotX + 34.0f,
			slotY[i] + 29.0f,
			D2D1::ColorF::Black,
			15.0f
		);
	}
}

void UIManager::RenderInteractionHint(ID2D1DeviceContext* context)
{
	if (m_isCraftingOpen)
		return;

	if (m_interactionHint.empty())
		return;

	ID2D1Bitmap* bubble =m_resourceManager->GetImage("Text");

	if (bubble == nullptr)
		return;

	float centerX = 650.0f;
	float y = 150.0f;

	float fontSize = 22.0f;

	// 1. 텍스트 크기 측정
	float textWidth =GRAPHICS.MeasureTextWidth(m_interactionHint.c_str(),fontSize);

	float paddingLeft = 20.0f;
	float paddingRight = 25.0f;
	// 2. 원본 이미지 크기
	D2D1_SIZE_F imageSize = bubble->GetSize();

	float srcWidth = imageSize.width;
	float srcHeight = imageSize.height;

	// 3. 원본 이미지 영역
	float leftSrcWidth = 360.0f;
	float rightSrcWidth = 180.0f;

	float centerSrcStart = leftSrcWidth;
	float centerSrcEnd = srcWidth - rightSrcWidth;

	// 4. 화면 크기
	float height = 80.0f;
	float scaleY =height / srcHeight;
	float leftWidth = leftSrcWidth * scaleY;
	float rightWidth =rightSrcWidth * scaleY;
	float centerWidth =textWidth +paddingLeft +paddingRight;
	float width =leftWidth +centerWidth +rightWidth;
	// 5. 가운데 기준 위치

	float x =centerX -width * 0.5f;

	// 6. 겹침

	const float overlap = 1.5f;

	// 7. LEFT

	D2D1_RECT_F srcLeft =
		D2D1::RectF(
			0.0f,
			0.0f,
			leftSrcWidth,
			srcHeight
		);

	D2D1_RECT_F dstLeft =
		D2D1::RectF(
			x,
			y,
			x + leftWidth + overlap,
			y + height
		);

	context->DrawBitmap(
		bubble,
		dstLeft,
		1.0f,
		D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		srcLeft
	);
	// 8. CENTER
	D2D1_RECT_F srcCenter =
		D2D1::RectF(
			centerSrcStart,
			0.0f,
			centerSrcEnd,
			srcHeight
		);

	D2D1_RECT_F dstCenter =
		D2D1::RectF(
			x + leftWidth - overlap,
			y,
			x + leftWidth + centerWidth + overlap,
			y + height
		);

	context->DrawBitmap(
		bubble,
		dstCenter,
		1.0f,
		D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		srcCenter
	);

	// 9. RIGHT
	D2D1_RECT_F srcRight =
		D2D1::RectF(
			centerSrcEnd,
			0.0f,
			srcWidth,
			srcHeight
		);

	D2D1_RECT_F dstRight =
		D2D1::RectF(
			x + leftWidth + centerWidth - overlap,
			y,
			x + width,
			y + height
		);

	context->DrawBitmap(
		bubble,
		dstRight,
		1.0f,
		D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		srcRight
	);

	// 10. 텍스트
	float textX =x +leftWidth +paddingLeft;

	GRAPHICS.DrawString(
		m_interactionHint.c_str(),
		textX,
		y + 25.0f,
		D2D1::ColorF(D2D1::ColorF::Black),
		fontSize
	);
}

void UIManager::RenderCrafting(ID2D1DeviceContext* context)
{
	if (m_inventory == nullptr || m_resourceManager == nullptr)
		return;
	float panelX = 80.0f;
	float panelY = 50.0f;

	float panelWidth = 1100.0f;
	float panelHeight = 550.0f;

	// 아이보리 / 베이지 배경
	GRAPHICS.FillRoundedRect(
		panelX,
		panelY,
		panelWidth,
		panelHeight,
		18.0f,
		D2D1::ColorF(
			0.92f,
			0.87f,
			0.75f,
			0.85f
		)
	);

	// 검정 외곽선
	GRAPHICS.DrawRoundedRect(
		panelX,
		panelY,
		panelWidth,
		panelHeight,
		18.0f,
		D2D1::ColorF::Black,
		3.0f
	);

	wstring title =(m_craftingStation == InteractType::CampFire)? L"요리": L"제작";

	GRAPHICS.DrawString(
		title.c_str(),
		panelX + 30.0f,
		panelY + 20.0f,
		D2D1::ColorF::Black,
		28.0f
	);

	RenderCraftingInventorySlots(context);
	RenderCraftingRecipeList(context);
	RenderCraftingIngredients(context);

	GRAPHICS.DrawString(
		L"만들고 싶은 레시피를 클릭하세요!",
		m_craftRecipeStartX,
		panelY + panelHeight - 45.0f,
		D2D1::ColorF::Black,
		16.0f
	);
}

void UIManager::RenderMessage(ID2D1DeviceContext* context)
{
	if (m_message.empty())
		return;

	ID2D1Bitmap* bubble =m_resourceManager->GetImage("bubbleText");

	if (bubble == nullptr)
		return;
	// 기본 위치

	float centerX = 650.0f;
	float y = 230.0f;

	float fontSize = 20.0f;
	// 1. 텍스트 크기 측정

	float textWidth =GRAPHICS.MeasureTextWidth(m_message.c_str(),fontSize);
	float paddingLeft = 15.0f;
	float paddingRight = 25.0f;

	// 2. 원본 이미지 크기
	D2D1_SIZE_F imageSize = bubble->GetSize();

	float srcWidth = imageSize.width;
	float srcHeight = imageSize.height;


	// 3. 원본 이미지 영역
	float leftSrcWidth = 360.0f;
	float rightSrcWidth = 180.0f;

	float centerSrcStart = leftSrcWidth;
	float centerSrcEnd = srcWidth - rightSrcWidth;

	// 4. 화면 크기
	float height = 80.0f;
	float scaleY =height / srcHeight;
	float leftWidth =leftSrcWidth * scaleY;
	float rightWidth =rightSrcWidth * scaleY;
	float centerWidth =textWidth +paddingLeft +paddingRight;
	float width =leftWidth +centerWidth +rightWidth;

	// 5. 가운데 기준 위치

	float x =centerX -width * 0.5f;

	// 6. 겹침 크기
	const float overlap = 1.5f;

	// 7. LEFT
	D2D1_RECT_F srcLeft =
		D2D1::RectF(
			0.0f,
			0.0f,
			leftSrcWidth,
			srcHeight
		);

	D2D1_RECT_F dstLeft =
		D2D1::RectF(
			x,
			y,
			x + leftWidth + overlap,
			y + height
		);

	context->DrawBitmap(
		bubble,
		dstLeft,
		1.0f,
		D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		srcLeft
	);

	// 8. CENTER
	D2D1_RECT_F srcCenter =
		D2D1::RectF(
			centerSrcStart,
			0.0f,
			centerSrcEnd,
			srcHeight
		);

	D2D1_RECT_F dstCenter =
		D2D1::RectF(
			x + leftWidth - overlap,
			y,
			x + leftWidth + centerWidth + overlap,
			y + height
		);

	context->DrawBitmap(
		bubble,
		dstCenter,
		1.0f,
		D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		srcCenter
	);
	// 9. RIGHT
	D2D1_RECT_F srcRight =
		D2D1::RectF(
			centerSrcEnd,
			0.0f,
			srcWidth,
			srcHeight
		);

	D2D1_RECT_F dstRight =
		D2D1::RectF(
			x + leftWidth + centerWidth - overlap,
			y,
			x + width,
			y + height
		);

	context->DrawBitmap(
		bubble,
		dstRight,
		1.0f,
		D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		srcRight
	);
	// 10. 텍스트
	float textX =x +leftWidth +paddingLeft;

	GRAPHICS.DrawString(
		m_message.c_str(),
		textX,
		y + 25.0f,
		D2D1::ColorF(D2D1::ColorF::Black),
		fontSize
	);
}

void UIManager::RenderInventoryWindow(ID2D1DeviceContext* context)
{
	if (m_inventory == nullptr || m_resourceManager == nullptr)
		return;

	float panelX = m_slotStartX - 30.0f;
	float panelY = m_slotStartY - 70.0f;

	float panelWidth = 800.0f;
	float panelHeight = 420.0f;

	// 아이보리 / 베이지 배경
	GRAPHICS.FillRoundedRect(
		panelX,
		panelY,
		panelWidth,
		panelHeight,
		16.0f,
		D2D1::ColorF(
			0.92f,
			0.87f,
			0.75f,
			0.8f
		)
	);

	// 검정 외곽선
	GRAPHICS.DrawRoundedRect(
		panelX,
		panelY,
		panelWidth,
		panelHeight,
		16.0f,
		D2D1::ColorF::Black,
		2.0f
	);

	GRAPHICS.DrawString(
		L"===인벤토리===",
		m_slotStartX,
		m_slotStartY - 55.0f,
		D2D1::ColorF::Black,
		24.0f
	);

	vector<pair<string, int>> itemList(m_inventory->GetAllItems().begin(),m_inventory->GetAllItems().end());


	for (int i = 0; i < (int)itemList.size(); i++)
	{
		int row = i / m_slotsPerRow;
		int col = i % m_slotsPerRow;

		float slotX =m_slotStartX +col * (m_slotSize + m_slotPadding);
		float slotY =m_slotStartY +row * (m_slotSize + m_slotPadding);

		GRAPHICS.FillRoundedRect(
			slotX,
			slotY,
			m_slotSize,
			m_slotSize,
			8.0f,
			D2D1::ColorF(
				0.97f,
				0.94f,
				0.85f,
				1.0f
			)
		);

		GRAPHICS.DrawRoundedRect(
			slotX,
			slotY,
			m_slotSize,
			m_slotSize,
			8.0f,
			D2D1::ColorF::Black,
			2.0f
		);

		const ItemData* itemData =DataManager::GetInstance().FindItem(itemList[i].first);

		if (itemData != nullptr)
		{
			ID2D1Bitmap* bitmap =m_resourceManager->GetImage(itemData->image);

			if (bitmap != nullptr)
			{
				float iconPadding = 5.0f;

				GRAPHICS.DrawBitmapUI(
					bitmap,
					slotX + iconPadding,
					slotY + iconPadding,
					m_slotSize - iconPadding * 2.0f,
					m_slotSize - iconPadding * 2.0f
				);
			}
		}
		wchar_t countText[10];

		swprintf_s(countText,L"%d",itemList[i].second);

		GRAPHICS.DrawString(
			countText,
			slotX + m_slotSize - 25.0f,
			slotY + m_slotSize - 28.0f,
			D2D1::ColorF::Black,
			15.0f
		);
	}

	int totalSlotsToShow =max((int)itemList.size(),m_slotsPerRow * 2);

	for (int i = (int)itemList.size();i < totalSlotsToShow;i++)
	{
		int row = i / m_slotsPerRow;
		int col = i % m_slotsPerRow;

		float slotX = m_slotStartX + col * (m_slotSize + m_slotPadding);
		float slotY =m_slotStartY +row * (m_slotSize + m_slotPadding);

		// 빈 슬롯 배경
		GRAPHICS.FillRoundedRect(
			slotX,
			slotY,
			m_slotSize,
			m_slotSize,
			8.0f,
			D2D1::ColorF(
				0.97f,
				0.94f,
				0.85f,
				1.0f
			)
		);

		// 빈 슬롯 테두리
		GRAPHICS.DrawRoundedRect(
			slotX,
			slotY,
			m_slotSize,
			m_slotSize,
			8.0f,
			D2D1::ColorF::Black,
			2.0f
		);
	}

	GRAPHICS.DrawString(
		L"===장비===",
		m_equipSlotX,
		m_slotStartY - 55.0f,
		D2D1::ColorF::Black,
		24.0f
	);

	float weaponSlotY =m_equipWeaponSlotY + 20.0f;

	GRAPHICS.FillRoundedRect(
		m_equipSlotX,
		weaponSlotY,
		m_slotSize,
		m_slotSize,
		8.0f,
		D2D1::ColorF(
			0.97f,
			0.94f,
			0.85f,
			1.0f
		)
	);

	GRAPHICS.DrawRoundedRect(
		m_equipSlotX,
		weaponSlotY,
		m_slotSize,
		m_slotSize,
		8.0f,
		D2D1::ColorF::Black,
		2.0f
	);

	// 무기 이름
	GRAPHICS.DrawString(
		L"=무기=",
		m_equipSlotX,
		m_equipWeaponSlotY - 10.0f,
		D2D1::ColorF::Black,
		14.0f
	);

	Weapon* equippedWeapon =(m_player != nullptr)? m_player->GetWeapon(): nullptr;

	if (equippedWeapon != nullptr)
	{
		const ItemData* itemData =DataManager::GetInstance().FindItem(equippedWeapon->GetWeaponId());

		if (itemData != nullptr)
		{
			ID2D1Bitmap* bitmap =m_resourceManager->GetImage(itemData->image);

			if (bitmap != nullptr)
			{
				float pad = 5.0f;

				GRAPHICS.DrawBitmapUI(
					bitmap,
					m_equipSlotX + pad,
					weaponSlotY + pad,
					m_slotSize - pad * 2.0f,
					m_slotSize - pad * 2.0f
				);
			}
		}
	}

	float shieldSlotY =m_equipShieldSlotY + 20.0f;

	GRAPHICS.FillRoundedRect(
		m_equipShieldSlotX,
		shieldSlotY,
		m_slotSize,
		m_slotSize,
		8.0f,
		D2D1::ColorF(
			0.97f,
			0.94f,
			0.85f,
			1.0f
		)
	);

	GRAPHICS.DrawRoundedRect(
		m_equipShieldSlotX,
		shieldSlotY,
		m_slotSize,
		m_slotSize,
		8.0f,
		D2D1::ColorF::Black,
		2.0f
	);

	// 방어구 이름
	GRAPHICS.DrawString(
		L"=방어구=",
		m_equipShieldSlotX,
		m_equipShieldSlotY - 10.0f,
		D2D1::ColorF::Black,
		14.0f
	);

	string equippedShieldId =(m_player != nullptr)? m_player->GetEquippedShieldId(): "";

	if (!equippedShieldId.empty())
	{
		const ItemData* itemData =DataManager::GetInstance().FindItem(equippedShieldId);

		if (itemData != nullptr)
		{
			ID2D1Bitmap* bitmap =m_resourceManager->GetImage(itemData->image);

			if (bitmap != nullptr)
			{
				float pad = 5.0f;

				GRAPHICS.DrawBitmapUI(
					bitmap,
					m_equipShieldSlotX + pad,
					shieldSlotY + pad,
					m_slotSize - pad * 2.0f,
					m_slotSize - pad * 2.0f
				);
			}
		}
	}

	GRAPHICS.DrawString(
		L"I : 닫기",
		m_slotStartX,
		panelY + panelHeight - 35.0f,
		D2D1::ColorF::Black,
		16.0f
	);
}

void UIManager::RenderCraftingInventorySlots(ID2D1DeviceContext* context)
{
	if (m_inventory == nullptr || m_resourceManager == nullptr)
		return;
	GRAPHICS.DrawString(
		L"[내 인벤토리]",
		m_craftInvStartX,
		m_craftInvStartY + 35.0f,
		D2D1::ColorF::Black,
		20.0f
	);

	vector<pair<string, int>> itemList(m_inventory->GetAllItems().begin(),m_inventory->GetAllItems().end());

	for (int i = 0; i < (int)itemList.size(); i++)
	{
		int row = i / m_slotsPerRow;
		int col = i % m_slotsPerRow;

		float slotX =m_craftInvStartX +col * (m_slotSize + m_slotPadding);
		float slotY =m_craftInvStartY +60.f+row * (m_slotSize + m_slotPadding);

		GRAPHICS.FillRoundedRect(
			slotX,
			slotY,
			m_slotSize,
			m_slotSize,
			8.0f,
			D2D1::ColorF(
				0.97f,
				0.94f,
				0.85f,
				1.0f
			)
		);


		GRAPHICS.DrawRoundedRect(
			slotX,
			slotY,
			m_slotSize,
			m_slotSize,
			8.0f,
			D2D1::ColorF::Black,
			2.0f
		);

		const ItemData* itemData =DataManager::GetInstance().FindItem(itemList[i].first);

		if (itemData != nullptr)
		{
			ID2D1Bitmap* bitmap =m_resourceManager->GetImage(itemData->image);

			if (bitmap != nullptr)
			{
				float pad = 5.0f;

				GRAPHICS.DrawBitmapUI(
					bitmap,
					slotX + pad,
					slotY + pad,
					m_slotSize - pad * 2.0f,
					m_slotSize - pad * 2.0f
				);
			}
		}


		wchar_t countText[10];

		swprintf_s(countText,L"%d",itemList[i].second);

		GRAPHICS.DrawString(
			countText,
			slotX + m_slotSize - 25.0f,
			slotY + m_slotSize - 25.0f,
			D2D1::ColorF::Black,
			15.0f
		);
	}
}

void UIManager::RenderCraftingRecipeList(ID2D1DeviceContext* context)
{
	string station;

	switch (m_craftingStation)
	{
	case InteractType::CampFire:
		station = "CampFire";
		break;

	case InteractType::WorkTable:
		station = "WorkTable";
		break;

	default:
		return;
	}

	vector<RecipeData> recipes =DataManager::GetInstance().GetRecipesByStation(station);

	const float recipeOffsetY = 30.0f;

	GRAPHICS.DrawString(
		L"레시피",
		m_craftRecipeStartX,
		m_craftRecipeStartY - 40.0f + recipeOffsetY,
		D2D1::ColorF::Black,
		20.0f
	);

	int maxOffset =max(0,(int)recipes.size() -m_craftRecipeVisibleCount);

	m_craftRecipeScrollOffset =clamp(m_craftRecipeScrollOffset,0,maxOffset);

	D2D1_RECT_F clipRect =
		D2D1::RectF(
			m_craftRecipeStartX,
			m_craftRecipeStartY + recipeOffsetY,
			m_craftRecipeStartX + 400.0f,
			m_craftRecipeStartY +
			m_craftRecipeAreaHeight
		);

	context->PushAxisAlignedClip(clipRect,D2D1_ANTIALIAS_MODE_ALIASED);

	float y = m_craftRecipeStartY + recipeOffsetY;

	int endIndex =min((int)recipes.size(),m_craftRecipeScrollOffset +m_craftRecipeVisibleCount);

	for (int i = m_craftRecipeScrollOffset;i < endIndex;i++)
	{
		const RecipeData& recipe = recipes[i];
		const ItemData* resultItem =DataManager::GetInstance().FindItem(recipe.resultId);

		wstring resultName =(resultItem != nullptr)? UTF8ToWString(resultItem->name): UTF8ToWString(recipe.resultId);

		bool canCraft =CraftingManager::CanCraft(recipe.id,m_inventory,m_player->GetWeapon());

		wstring ingredientText;

		for (int j = 0;j < (int)recipe.ingredients.size();j++)
		{
			const Ingredient& ing = recipe.ingredients[j];
			const ItemData* ingItem =DataManager::GetInstance().FindItem(ing.ingredientId);

			wstring ingName =(ingItem != nullptr)? UTF8ToWString(ingItem->name): UTF8ToWString(ing.ingredientId);

			wchar_t buf[50];

			swprintf_s(
				buf,
				L"%s x%d",
				ingName.c_str(),
				ing.count
			);

			if (j > 0)
				ingredientText += L", ";

			ingredientText += buf;
		}


		float cardX = m_craftRecipeStartX;

		float cardWidth = 380.0f;
		float cardHeight = 70.0f;

		float radius = 10.0f;

		bool selected =(i == m_selectedRecipeIndex);

		if (selected)
		{
			// 선택된 레시피
			GRAPHICS.FillRoundedRect(
				cardX,
				y,
				cardWidth,
				cardHeight,
				radius,
				D2D1::ColorF(
					0.82f,
					0.74f,
					0.58f,
					1.0f
				)
			);
		}
		else
		{
			// 일반 레시피
			GRAPHICS.FillRoundedRect(
				cardX,
				y,
				cardWidth,
				cardHeight,
				radius,
				D2D1::ColorF(
					0.97f,
					0.94f,
					0.85f,
					1.0f
				)
			);
		}

		GRAPHICS.DrawRoundedRect(
			cardX,
			y,
			cardWidth,
			cardHeight,
			radius,
			D2D1::ColorF::Black,
			selected ? 3.0f : 2.0f
		);

		wstring resultText;

		if (selected)
			resultText += L"> ";

		resultText += resultName;

		if (canCraft)
			resultText += L"  (가능)";
		else
			resultText += L"  (부족)";

		GRAPHICS.DrawString(
			resultText.c_str(),
			cardX + 15.0f,
			y + 7.0f,
			D2D1::ColorF::Black,
			18.0f
		);

		wstring ingredientLine =
			L"필요: " + ingredientText;

		GRAPHICS.DrawString(
			ingredientLine.c_str(),
			cardX + 15.0f,
			y + 37.0f,
			D2D1::ColorF::Black,
			14.0f
		);

		// 다음 카드
		y += cardHeight + 10.0f;
	}

	context->PopAxisAlignedClip();

	RenderRecipeScrollbar(
		context,
		(int)recipes.size(),
		m_craftRecipeVisibleCount,
		m_craftRecipeScrollOffset
	);
}

void UIManager::RenderCraftingIngredients(ID2D1DeviceContext* context)
{
	if (m_inventory == nullptr ||
		m_resourceManager == nullptr)
		return;

	string station;

	switch (m_craftingStation)
	{
	case InteractType::CampFire:
		station = "CampFire";
		break;

	case InteractType::WorkTable:
		station = "WorkTable";
		break;

	default:
		return;
	}

	vector<RecipeData> recipes =DataManager::GetInstance().GetRecipesByStation(station);

	if (recipes.empty())
		return;

	if (m_selectedRecipeIndex < 0 ||m_selectedRecipeIndex >= (int)recipes.size())
		return;

	const RecipeData& recipe =recipes[m_selectedRecipeIndex];

	Weapon* equipped =(m_player != nullptr)? m_player->GetWeapon(): nullptr;

	GRAPHICS.DrawString(
		L"필요 재료",
		m_craftIngredientStartX,
		m_craftIngredientStartY - 10.0f,
		D2D1::ColorF::Black,
		20.0f
	);

	for (int i = 0;i < (int)recipe.ingredients.size();i++)
	{
		const Ingredient& ing =recipe.ingredients[i];

		float slotX =m_craftIngredientStartX +i * (m_slotSize + m_slotPadding);

		const float ingredientOffsetY = 30.0f;

		float slotY =m_craftIngredientStartY +ingredientOffsetY;
		bool isEquippedWeapon =(equipped != nullptr && ing.ingredientId == equipped->GetWeaponId());

		int haveCount =isEquippedWeapon? ing.count: m_inventory->GetItemCount(ing.ingredientId);

		bool enough =isEquippedWeapon? true: (haveCount >= ing.count);

		GRAPHICS.FillRoundedRect(
			slotX,
			slotY,
			m_slotSize,
			m_slotSize,
			8.0f,
			D2D1::ColorF(
				0.97f,
				0.94f,
				0.85f,
				1.0f
			)
		);


		if (enough)
		{
			GRAPHICS.DrawRoundedRect(
				slotX,
				slotY,
				m_slotSize,
				m_slotSize,
				8.0f,
				D2D1::ColorF::Black,
				2.0f
			);
		}
		else
		{
			GRAPHICS.DrawRoundedRect(
				slotX,
				slotY,
				m_slotSize,
				m_slotSize,
				8.0f,
				D2D1::ColorF(
					0.75f,
					0.15f,
					0.15f,
					1.0f
				),
				3.0f
			);
		}

		const ItemData* itemData =DataManager::GetInstance().FindItem(ing.ingredientId);

		if (itemData != nullptr)
		{
			ID2D1Bitmap* bitmap =m_resourceManager->GetImage(itemData->image);

			if (bitmap != nullptr)
			{
				float pad = 7.0f;

				GRAPHICS.DrawBitmapUI(
					bitmap,
					slotX + pad,
					slotY + pad,
					m_slotSize - pad * 2.0f,
					m_slotSize - pad * 2.0f
				);
			}
		}

		wchar_t countText[20];

		if (isEquippedWeapon)
		{
			swprintf_s(countText,L"장착");
		}
		else
		{
			swprintf_s(countText,L"%d/%d",haveCount,ing.count);
		}

		float countWidth = 48.0f;
		float countHeight = 22.0f;

		GRAPHICS.FillRoundedRect(
			slotX + m_slotSize - countWidth - 2.0f,
			slotY + m_slotSize - countHeight - 2.0f,
			countWidth,
			countHeight,
			5.0f,
			D2D1::ColorF(
				0.05f,
				0.05f,
				0.05f,
				0.85f
			)
		);

		GRAPHICS.DrawString(
			countText,
			slotX + m_slotSize - countWidth + 2.0f,
			slotY + m_slotSize - countHeight - 1.0f,
			D2D1::ColorF::White,
			13.0f
		);
	}
}

void UIManager::RenderRecipeScrollbar(ID2D1DeviceContext* context, int totalCount, int visibleCount, int scrollOffset)
{
	if (totalCount <= visibleCount) return; // 스크롤 필요 없으면 안 그림

	float trackX = m_craftRecipeStartX + 410.f; // 리스트 폭(400) 오른쪽에 붙임
	float trackY = m_craftRecipeStartY;
	float trackWidth = 6.f;
	float trackHeight = m_craftRecipeAreaHeight;

	// 트랙(배경 막대)
	GRAPHICS.FillRect(trackX, trackY, trackWidth, trackHeight, D2D1::ColorF(0.3f, 0.3f, 0.3f, 0.5f));

	// 썸 크기 = 보이는 비율만큼
	float visibleRatio = (float)visibleCount / (float)totalCount;
	float thumbHeight = max(trackHeight * visibleRatio, 20.f); // 최소 높이 보장

	// 썸 위치 = 스크롤 진행률에 비례
	float maxOffset = (float)(totalCount - visibleCount);
	float scrollRatio = (maxOffset > 0) ? (float)scrollOffset / maxOffset : 0.f;
	float thumbY = trackY + scrollRatio * (trackHeight - thumbHeight);

	// 썸(실제 스크롤 위치 표시)
	GRAPHICS.FillRect(trackX, thumbY, trackWidth, thumbHeight, D2D1::ColorF(0.8f, 0.8f, 0.8f, 0.9f));
}

void UIManager::RenderHP(ID2D1DeviceContext* context)
{

	if (m_player == nullptr)
		return;

	int currentHP = m_player->GetHealth();
	int maxHP = m_player->GetMaxHealth();

	if (maxHP <= 0)
		return;

	float hpRatio =static_cast<float>(currentHP) /static_cast<float>(maxHP);

	hpRatio = max(0.0f, min(1.0f, hpRatio));

	// -----------------------------------------
	// HP 위치 / 크기
	// -----------------------------------------

	float x = 25.0f;
	float y = 10.0f;

	float barWidth = 220.0f;
	float barHeight = 100.0f;


	ID2D1Bitmap* hpImage = m_resourceManager->GetImage("HPUI");

	if (hpImage != nullptr)
	{
		GRAPHICS.DrawBitmapUI(
			hpImage,
			x-10,
			y -20,
			barWidth +20,
			barHeight
		);
	}

	float hpX = x + 35.0f;
	float hpY = y + 19.0f;

	float hpWidth = 175.0f;
	float hpHeight = 20.0f;

	float fillWidth = hpWidth * hpRatio;

	if (fillWidth > 0.0f)
	{
		GRAPHICS.FillRoundedRect(
			hpX,
			hpY,
			fillWidth,
			hpHeight,
			6.0f,     // 둥근 정도
			D2D1::ColorF(
				0.9f,
				0.03f,
				0.06f,
				1.0f
			)
		);
	}
	wchar_t hpText[64];

	swprintf_s(
		hpText,
		L"HP %d / %d",
		currentHP,
		maxHP
	);

	GRAPHICS.DrawString(
		hpText,
		x + barWidth +20.f,
		y + 1.5f,
		D2D1::ColorF(D2D1::ColorF::Black), 30.f);
}
