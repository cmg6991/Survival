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
	m_craftRecipeScrollOffset += direction;
	if (m_craftRecipeScrollOffset < 0)
		m_craftRecipeScrollOffset = 0;
}

void UIManager::RenderTime(ID2D1DeviceContext* context)
{
	int day = TimeManager::GetInstance().GetDay();
	int hour = TimeManager::GetInstance().GetHour();
	int minute = TimeManager::GetInstance().GetMinute();

	wchar_t text[100];
	swprintf_s(
		text,
		L"Day %d  %02d:%02d   [%s]",
		day,
		hour,
		minute,
		TimeManager::GetInstance().GetPhaseString()
	);
	GRAPHICS.DrawString(text, 20, 20);
}

void UIManager::RenderInventory(ID2D1DeviceContext* context)
{
	ID2D1Bitmap* invUI = m_resourceManager->GetImage("SideBarUI");
	///*float invY = 100;
	//for (auto& pair : m_inventory->GetAllItems())
	//{
	//	const ItemData* item = DataManager::GetInstance().FindItem(pair.first);

	//	wstring name;
	//	if (item != nullptr)
	//		name = UTF8ToWString(item->name);
	//	else
	//		name = UTF8ToWString(pair.first);

	//	wchar_t line[100];
	//	swprintf_s(line, L"%s x%d", name.c_str(), pair.second);

	//	GRAPHICS.DrawString(line, 20, invY);
	//	invY += 30;
	//}*/
	if (invUI == nullptr)
		return;

	float startX = 20;
	float startY = 70;

	// 인벤토리 배경
	GRAPHICS.DrawBitmapUI(invUI, startX, startY, 64, 192);

	const float slotSize = 48.0f;
	const float slotX = startX + 8;
	const float slotY[3] =
	{
		startY + 8,
		startY + 72,
		startY + 136
	};
	vector<pair<string, int>> items(
		m_inventory->GetAllItems().begin(),
		m_inventory->GetAllItems().end());

	for (int i = 0; i < min((int)items.size(), 3); i++)
	{
		const ItemData* item = DataManager::GetInstance().FindItem(items[i].first);
		if (item == nullptr)
			continue;

		ID2D1Bitmap* icon = m_resourceManager->GetImage(item->image);
		if (icon != nullptr)
		{
			GRAPHICS.DrawBitmapUI(
				icon,
				slotX,
				slotY[i],
				slotSize,
				slotSize);
		}

		wchar_t count[10];
		swprintf_s(count, L"%d", items[i].second);

		GRAPHICS.DrawString(
			count,
			slotX + 35,
			slotY[i] + 30,
			15.f);
	}
}

void UIManager::RenderInteractionHint(ID2D1DeviceContext* context)
{
	if (m_isCraftingOpen) return;
	if (m_interactionHint.empty()) return;

	GRAPHICS.DrawString(m_interactionHint.c_str(), 700, 450);
}

void UIManager::RenderCrafting(ID2D1DeviceContext* context)
{
	if (m_inventory == nullptr || m_resourceManager == nullptr) return;

	// 배경 패널 (전체 크래프팅 창)
	GRAPHICS.FillRect(80, 50, 1100, 550, D2D1::ColorF(0.08f, 0.08f, 0.08f, 0.9f));

	wstring title = (m_craftingStation == InteractType::CampFire) ? L"=== 요리 ===" : L"=== 제작 ===";
	GRAPHICS.DrawString(title.c_str(), m_craftInvStartX, 200);

	RenderCraftingInventorySlots(context);
	RenderCraftingRecipeList(context);
	RenderCraftingIngredients(context);

	GRAPHICS.DrawString(L"만들고 싶은 레시피를 클릭하세요!", m_craftRecipeStartX, 550);
}

void UIManager::RenderMessage(ID2D1DeviceContext* context)
{
	if (m_message.empty()) return;
	GRAPHICS.DrawString(m_message.c_str(), 700, 500);

}

void UIManager::RenderInventoryWindow(ID2D1DeviceContext* context)
{
	if (m_inventory == nullptr || m_resourceManager == nullptr) return;

	// 배경 패널
	float panelWidth = m_slotsPerRow * (m_slotSize + m_slotPadding) + m_slotPadding;
	GRAPHICS.FillRect(m_slotStartX - 20, m_slotStartY - 60, panelWidth + 400, 400, D2D1::ColorF(0.1f, 0.1f, 0.1f, 0.85f));

	GRAPHICS.DrawString(L"=== 인벤토리 ===", m_slotStartX, m_slotStartY - 50);

	// unordered_map을 순서 고정된 리스트로 스냅샷
	vector<pair<string, int>> itemList(m_inventory->GetAllItems().begin(), m_inventory->GetAllItems().end());

	for (int i = 0; i < (int)itemList.size(); i++)
	{
		int row = i / m_slotsPerRow;
		int col = i % m_slotsPerRow;

		float slotX = m_slotStartX + col * (m_slotSize + m_slotPadding);
		float slotY = m_slotStartY + row * (m_slotSize + m_slotPadding);

		// 슬롯 배경 + 테두리
		GRAPHICS.FillRect(slotX, slotY, m_slotSize, m_slotSize, D2D1::ColorF(0.2f, 0.2f, 0.2f, 1.0f));
		GRAPHICS.DrawRect(slotX, slotY, m_slotSize, m_slotSize, D2D1::ColorF::White, 2.0f);

		// 아이템 아이콘
		const ItemData* itemData = DataManager::GetInstance().FindItem(itemList[i].first);
		if (itemData != nullptr)
		{
			ID2D1Bitmap* bitmap = m_resourceManager->GetImage(itemData->image);
			if (bitmap != nullptr)
			{
				float iconPadding = 8.0f;
				GRAPHICS.DrawBitmapUI(
					bitmap,
					slotX + iconPadding,
					slotY + iconPadding,
					m_slotSize - iconPadding * 2,
					m_slotSize - iconPadding * 2
				);
			}
		}

		// 개수 표시 (슬롯 우측하단)
		wchar_t countText[10];
		swprintf_s(countText, L"%d", itemList[i].second);
		GRAPHICS.DrawString(countText, slotX + m_slotSize - 25, slotY + m_slotSize - 30);
	}

	// 빈 슬롯도 최소 개수만큼 테두리만 그려서 격자 느낌 유지 (선택 사항)
	int totalSlotsToShow = max((int)itemList.size(), m_slotsPerRow * 2); // 최소 2줄 정도는 항상 보이게
	for (int i = (int)itemList.size(); i < totalSlotsToShow; i++)
	{
		int row = i / m_slotsPerRow;
		int col = i % m_slotsPerRow;

		float slotX = m_slotStartX + col * (m_slotSize + m_slotPadding);
		float slotY = m_slotStartY + row * (m_slotSize + m_slotPadding);

		GRAPHICS.DrawRect(slotX, slotY, m_slotSize, m_slotSize, D2D1::ColorF(0.4f, 0.4f, 0.4f, 1.0f), 1.0f);
	}

	GRAPHICS.DrawString(L"I: 닫기", m_slotStartX, m_slotStartY + 300);

	GRAPHICS.DrawString(L"===장비===", m_equipSlotX, m_slotStartY - 50);

	GRAPHICS.FillRect(m_equipSlotX, m_equipWeaponSlotY+20, m_slotSize, m_slotSize, D2D1::ColorF(0.25f, 0.2f, 0.1f, 1.0f));
	GRAPHICS.DrawRect(m_equipSlotX, m_equipWeaponSlotY+20, m_slotSize, m_slotSize, D2D1::ColorF::Gold, 2.0f);
	GRAPHICS.DrawString(L"무기", m_equipSlotX, m_equipWeaponSlotY - 10, 14.f);

	Weapon* equippedWeapon = (m_player != nullptr) ? m_player->GetWeapon() : nullptr;
	if (equippedWeapon != nullptr)
	{
		const ItemData* itemData = DataManager::GetInstance().FindItem(equippedWeapon->GetWeaponId());
		if (itemData != nullptr)
		{
			ID2D1Bitmap* bitmap = m_resourceManager->GetImage(itemData->image);
			if (bitmap != nullptr)
			{
				float pad = 5.0f;
				GRAPHICS.DrawBitmapUI(bitmap, m_equipSlotX + pad, m_equipWeaponSlotY + pad+20, m_slotSize - pad * 2, m_slotSize - pad * 2);
			}
		}
	}

	GRAPHICS.DrawString(L"===방어구===", m_equipShieldSlotX, m_equipShieldSlotY - 50);

	GRAPHICS.FillRect(m_equipShieldSlotX, m_equipShieldSlotY + 20, m_slotSize, m_slotSize, D2D1::ColorF(0.1f, 0.15f, 0.25f, 1.0f));
	GRAPHICS.DrawRect(m_equipShieldSlotX, m_equipShieldSlotY + 20, m_slotSize, m_slotSize, D2D1::ColorF::SteelBlue, 2.0f);
	GRAPHICS.DrawString(L"방패", m_equipShieldSlotX, m_equipShieldSlotY - 10, 14.f);

	string equippedShieldId = (m_player != nullptr) ? m_player->GetEquippedShieldId() : "";
	if (!equippedShieldId.empty())
	{
		const ItemData* itemData = DataManager::GetInstance().FindItem(equippedShieldId);
		if (itemData != nullptr)
		{
			ID2D1Bitmap* bitmap = m_resourceManager->GetImage(itemData->image);
			if (bitmap != nullptr)
			{
				float pad = 5.0f;
				GRAPHICS.DrawBitmapUI(bitmap, m_equipShieldSlotX + pad, m_equipShieldSlotY + pad+20 , m_slotSize - pad * 2, m_slotSize - pad * 2);
			}
		}
	}
}

void UIManager::RenderCraftingInventorySlots(ID2D1DeviceContext* context)
{
	GRAPHICS.DrawString(L"[내 인벤토리]", m_craftInvStartX, m_craftInvStartY - 35);

	vector<pair<string, int>> itemList(m_inventory->GetAllItems().begin(), m_inventory->GetAllItems().end());

	for (int i = 0; i < (int)itemList.size(); i++)
	{
		int row = i / m_slotsPerRow;
		int col = i % m_slotsPerRow;

		float slotX = m_craftInvStartX + col * (m_slotSize + m_slotPadding);
		float slotY = m_craftInvStartY + row * (m_slotSize + m_slotPadding);

		GRAPHICS.FillRect(slotX, slotY, m_slotSize, m_slotSize, D2D1::ColorF(0.2f, 0.2f, 0.2f, 1.0f));
		GRAPHICS.DrawRect(slotX, slotY, m_slotSize, m_slotSize, D2D1::ColorF::White, 2.0f);

		const ItemData* itemData = DataManager::GetInstance().FindItem(itemList[i].first);
		if (itemData != nullptr)
		{
			ID2D1Bitmap* bitmap = m_resourceManager->GetImage(itemData->image);
			if (bitmap != nullptr)
			{
				float pad = 8.0f;
				GRAPHICS.DrawBitmapUI(bitmap, slotX + pad, slotY + pad, m_slotSize - pad * 2, m_slotSize - pad * 2);
			}
		}

		wchar_t countText[10];
		swprintf_s(countText, L"%d", itemList[i].second);
		GRAPHICS.DrawString(countText, slotX + m_slotSize - 25, slotY + m_slotSize - 22, 15.f);
	}
}

void UIManager::RenderCraftingRecipeList(ID2D1DeviceContext* context)
{
	string station;
	switch (m_craftingStation)
	{
	case InteractType::CampFire:  station = "CampFire"; break;
	case InteractType::WorkTable: station = "WorkTable"; break;
	}

	vector<RecipeData> recipes = DataManager::GetInstance().GetRecipesByStation(station);

	GRAPHICS.DrawString(L"[레시피]", m_craftRecipeStartX, m_craftRecipeStartY - 30);


	// 스크롤 범위 보정 (레시피 개수가 바뀌었을 수도 있으니 매번 clamp)
	int maxOffset = max(0, (int)recipes.size() - m_craftRecipeVisibleCount);
	m_craftRecipeScrollOffset = clamp(m_craftRecipeScrollOffset, 0, maxOffset);

	// 리스트 영역만큼 클리핑 -> 이 영역 밖으로는 절대 안 그려짐
	D2D1_RECT_F clipRect = D2D1::RectF(
		m_craftRecipeStartX,
		m_craftRecipeStartY,
		m_craftRecipeStartX + 400.f, // 리스트 폭에 맞게 조절
		m_craftRecipeStartY + m_craftRecipeAreaHeight
	);
	context->PushAxisAlignedClip(clipRect, D2D1_ANTIALIAS_MODE_ALIASED);

	float y = m_craftRecipeStartY;
	int endIndex = min((int)recipes.size(), m_craftRecipeScrollOffset + m_craftRecipeVisibleCount);

	for (int i = m_craftRecipeScrollOffset; i < endIndex; i++)
	{
		const RecipeData& recipe = recipes[i];
		const ItemData* resultItem = DataManager::GetInstance().FindItem(recipe.resultId);

		wstring resultName = (resultItem != nullptr) ? UTF8ToWString(resultItem->name) : UTF8ToWString(recipe.resultId);
		bool canCraft = CraftingManager::CanCraft(recipe.id, m_inventory,m_player->GetWeapon() );

		// 필요한 재료 이름들을 "재료명xN, 재료명xN" 형태로 조합
		wstring ingredientText;
		for (int j = 0; j < (int)recipe.ingredients.size(); j++)
		{
			const Ingredient& ing = recipe.ingredients[j];
			const ItemData* ingItem = DataManager::GetInstance().FindItem(ing.ingredientId);
			wstring ingName = (ingItem != nullptr) ? UTF8ToWString(ingItem->name) : UTF8ToWString(ing.ingredientId);

			wchar_t buf[50];
			swprintf_s(buf, L"%s x%d", ingName.c_str(), ing.count);

			if (j > 0) ingredientText += L", ";
			ingredientText += buf;
		}

		wchar_t line[100];
		swprintf_s(line, L"%s%s %s",
			(i == m_selectedRecipeIndex) ? L"> " : L"   ",
			resultName.c_str(),
			canCraft ? L"(가능)" : L"(부족)");

		GRAPHICS.DrawString(line, m_craftRecipeStartX, y);
		y += 25;

		// 재료 목록을 결과물 이름 바로 아래, 살짝 들여써서 표시
		wstring ingredientLine = L"    필요: " + ingredientText;
		GRAPHICS.DrawString(ingredientLine.c_str(), m_craftRecipeStartX, y);
		y += 50; // 다음 레시피와 간격
	}
	context->PopAxisAlignedClip();
	RenderRecipeScrollbar(context, (int)recipes.size(), m_craftRecipeVisibleCount, m_craftRecipeScrollOffset);
}

void UIManager::RenderCraftingIngredients(ID2D1DeviceContext* context)
{
	string station;
	switch (m_craftingStation)
	{
	case InteractType::CampFire:  station = "CampFire"; break;
	case InteractType::WorkTable: station = "WorkTable"; break;
	}

	vector<RecipeData> recipes = DataManager::GetInstance().GetRecipesByStation(station);
	if (recipes.empty() || m_selectedRecipeIndex >= (int)recipes.size()) return;

	const RecipeData& recipe = recipes[m_selectedRecipeIndex];
	Weapon* equipped = (m_player != nullptr) ? m_player->GetWeapon() : nullptr;

	GRAPHICS.DrawString(L"[필요 재료]", m_craftIngredientStartX, m_craftIngredientStartY - 30);

	for (int i = 0; i < (int)recipe.ingredients.size(); i++)
	{
		const Ingredient& ing = recipe.ingredients[i];

		float slotX = m_craftIngredientStartX + i * (m_slotSize + m_slotPadding);
		float slotY = m_craftIngredientStartY;

		bool isEquippedWeapon = (equipped != nullptr && ing.ingredientId == equipped->GetWeaponId());

		int haveCount = isEquippedWeapon ? ing.count : m_inventory->GetItemCount(ing.ingredientId);
		bool enough = isEquippedWeapon ? true : (haveCount >= ing.count);

		GRAPHICS.FillRect(slotX, slotY, m_slotSize, m_slotSize, D2D1::ColorF(0.2f, 0.2f, 0.2f, 1.0f));
		GRAPHICS.DrawRect(slotX, slotY, m_slotSize, m_slotSize,
			enough ? D2D1::ColorF::LightGreen : D2D1::ColorF::Red, 2.0f);

		const ItemData* itemData = DataManager::GetInstance().FindItem(ing.ingredientId);
		if (itemData != nullptr)
		{
			ID2D1Bitmap* bitmap = m_resourceManager->GetImage(itemData->image);
			if (bitmap != nullptr)
			{
				float pad = 8.0f;
				GRAPHICS.DrawBitmapUI(bitmap, slotX + pad, slotY + pad, m_slotSize - pad * 2, m_slotSize - pad * 2);
			}
		}

		wchar_t countText[20]; 
		if (isEquippedWeapon)
			swprintf_s(countText, L"장착중");
		else
			swprintf_s(countText, L"%d/%d", haveCount, ing.count);

		GRAPHICS.DrawString(countText, slotX, slotY + m_slotSize + 5);
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

	float hpRatio =
		static_cast<float>(currentHP) /
		static_cast<float>(maxHP);

	hpRatio = max(0.0f, min(1.0f, hpRatio));

	// -----------------------------------------
	// HP 위치 / 크기
	// -----------------------------------------

	float x = 25.0f;
	float y = 55.0f;

	float barWidth = 220.0f;
	float barHeight = 22.0f;

	float border = 2.0f;

	float innerWidth = barWidth - border * 2.0f;
	float innerHeight = barHeight - border * 2.0f;

	// -----------------------------------------
	// 1. 검은색 전체 배경
	// -----------------------------------------

	GRAPHICS.DrawRect(
		x,
		y,
		barWidth,
		barHeight,
		D2D1::ColorF(
			0.02f,
			0.02f,
			0.02f,
			1.0f
		)
	);

	// -----------------------------------------
	// 2. 글씨를 먼저 그린다
	// -----------------------------------------

	wchar_t hpText[64];

	swprintf_s(
		hpText,
		L"HP %d / %d",
		currentHP,
		maxHP
	);

	GRAPHICS.DrawString(
		hpText,
		x + barWidth +10.f,
		y + 1.0f
	);

	// -----------------------------------------
	// 3. 빨간색 HP 영역
	//
	// HP 100% -> 전체를 덮음
	// HP 50%  -> 절반만 덮음
	// HP 0%   -> 아무것도 덮지 않음
	// -----------------------------------------

	float fillWidth = innerWidth * hpRatio;

	if (fillWidth > 0.0f)
	{
		GRAPHICS.FillRect(
			x + border,
			y + border,
			fillWidth,
			innerHeight,
			D2D1::ColorF(
				0.9f,
				0.03f,
				0.06f,
				1.0f
			)
		);
	}

	// -----------------------------------------
	// 4. 테두리
	// -----------------------------------------

	// 위
	GRAPHICS.DrawRect(
		x,
		y,
		barWidth,
		border,
		D2D1::ColorF::White
	);

	// 아래
	GRAPHICS.DrawRect(
		x,
		y + barHeight - border,
		barWidth,
		border,
		D2D1::ColorF::White
	);

	// 왼쪽
	GRAPHICS.DrawRect(
		x,
		y,
		border,
		barHeight,
		D2D1::ColorF::White
	);

	// 오른쪽
	GRAPHICS.DrawRect(
		x + barWidth - border,
		y,
		border,
		barHeight,
		D2D1::ColorF::White
	);
}
