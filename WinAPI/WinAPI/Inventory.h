#pragma once
#include "pch.h"

class Inventory
{
public:
	Inventory(int maxSlotCount = 10, int maxStackPerItem = 50);
	~Inventory();

	int AddItem(const string& itemId, int count = 1);
	bool RemoveItem(const string& itemId, int count = 1);
	int GetItemCount(const string& itemId) const;
	bool HasEnough(const string& itemId, int count) const;

	bool IsFull() const;
	int GetSlotCount() const { return (int)m_items.size(); }
	int GetMaxSlotCount() const { return m_maxSlotCount; }

	void DropItem(const string& itemId);

	const unordered_map<string, int>& GetAllItems() const { return m_items; }
	void SetAllItems(const unordered_map<string, int>& items) { m_items = items; }

private:
	unordered_map<string, int> m_items;
	int m_maxSlotCount;
	int m_maxStackPerItem;
};

