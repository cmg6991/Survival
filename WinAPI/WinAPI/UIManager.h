#pragma once
#include "pch.h"
#include "Singleton.h"
#include "InteractType.h"

class Inventory;
class CampFire;
class ResourceManager;
class Player;

class UIManager : public Singleton<UIManager>
{
public:
	void Update(float deltaTime);
	void Render(ID2D1DeviceContext* context);

	void ShowMessage(const wstring& message, float duration = 2.0f);
	void SetInventory(Inventory* inventory) { m_inventory = inventory; }
	void SetResourceManager(ResourceManager* resourceManager) { m_resourceManager = resourceManager; }

	void SetInteractionHint(const wstring& hint) { m_interactionHint = hint; }
	void ClearInteractionHint() { m_interactionHint.clear(); }
	
	void SetPlayer(Player* player) { m_player = player; }

	void OpenCrafting(InteractType station) { m_isCraftingOpen = true; m_craftingStation = station; m_selectedRecipeIndex = 0; }
	void CloseCrafting() { m_isCraftingOpen = false; }
	bool IsCraftingOpen() const { return m_isCraftingOpen; }
	InteractType GetCraftingStation() const { return m_craftingStation; }
	int GetSelectedRecipeIndex() const { return m_selectedRecipeIndex; }
	void MoveSelection(int delta, int maxCount);
	int GetSelectedIndexClamped() const { return m_selectedRecipeIndex; }

	void ToggleInventoryWindow();
	bool IsInventoryWindowOpen() const { return m_isInventoryOpen; }

	void HandleCraftingInventoryClick(float mouseX, float mouseY);
	void HandleInventoryClick(float mouseX, float mouseY);
	bool HandleCraftingRecipeClick(float mouseX, float mouseY);
	void EquipItem(const string& itemId);

	void SetOnWeaponEquip(function<void(const string&)> callback) { m_onWeaponEquip = callback; }
	void SetOnWeaponUnequip(function<void()> callback) { m_onWeaponUnequip = callback; }

	void ScrollCraftingRecipe(int direction);

private:
	void RenderTime(ID2D1DeviceContext* context);
	void RenderInventory(ID2D1DeviceContext* context);
	void RenderInteractionHint(ID2D1DeviceContext* context);
	void RenderCrafting(ID2D1DeviceContext* context);
	void RenderMessage(ID2D1DeviceContext* context);
	void RenderInventoryWindow(ID2D1DeviceContext* context);

	void RenderCraftingInventorySlots(ID2D1DeviceContext* context); // 좌측 인벤토리 슬롯
	void RenderCraftingRecipeList(ID2D1DeviceContext* context);     // 우측 레시피 목록
	void RenderCraftingIngredients(ID2D1DeviceContext* context);
	void RenderRecipeScrollbar(ID2D1DeviceContext* context, int totalCount, int visibleCount, int scrollOffset);
private:
	Inventory* m_inventory = nullptr;
	ResourceManager* m_resourceManager = nullptr;
	Player* m_player = nullptr;

	wstring m_interactionHint;

	bool m_isCraftingOpen = false;
	InteractType m_craftingStation = InteractType::CampFire;
	int m_selectedRecipeIndex = 0;

	wstring m_message;
	float m_messageTimer = 0.0f;

	function<void(const string&)> m_onWeaponEquip;
	function<void()> m_onWeaponUnequip;

	// 인벤토리 창 상태
	bool m_isInventoryOpen = false;

	// 슬롯 레이아웃
	const float m_slotStartX = 100.0f;
	const float m_slotStartY = 120.0f;
	const float m_slotSize = 80.0f;
	const float m_slotPadding = 10.0f;
	const int m_slotsPerRow = 5;

	// 크래프팅 창 전용 슬롯 레이아웃 (인벤토리 창과 별개 좌표)
	const float m_craftInvStartX = 100.0f;
	const float m_craftInvStartY = 100.0f;

	const float m_craftRecipeStartX = 700.0f;
	const float m_craftRecipeStartY = 80.0f;

	const float m_craftIngredientStartX = 700.0f;
	const float m_craftIngredientStartY = 420.0f;

	// 장비 슬롯 좌표 (인벤토리 옆)
	float m_equipSlotX = 700.f;
	float m_equipWeaponSlotY = 120.f;

	int m_craftRecipeScrollOffset = 0;   // 몇 번째 레시피부터 보여줄지
	int m_craftRecipeVisibleCount = 4;   // 한 화면에 보여줄 레시피 개수 (아이템당 75px면 영역 높이/75로 계산해도 됨)
	float m_craftRecipeAreaHeight = 330.f; // 재료 슬롯 침범하지 않는 리스트 영역 높이

private:
	UIManager() {}
	~UIManager() {}

	friend class Singleton<UIManager>;
};

