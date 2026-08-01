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
	//int GetSlotCount() const { return (int)m_items.size(); }
};

