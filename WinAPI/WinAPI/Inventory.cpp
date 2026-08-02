#include "Inventory.h"

Inventory::Inventory(int maxSlotCount, int maxStackPerItem) 
	:m_maxSlotCount(maxSlotCount),m_maxStackPerItem(maxStackPerItem)
{
}

Inventory::~Inventory()
{
}

int Inventory::AddItem(const string& itemId, int count)
{
	auto it = m_items.find(itemId);

	if (it == m_items.end())
	{
		// 새로운 아이템 종류인데 슬롯이 꽉 찼으면 못 넣음
		if (IsFull())
			return 0;

		int addAmount = min(count, m_maxStackPerItem);
		m_items[itemId] = addAmount;
		return addAmount;
	}
	else
	{
		// 이미 있는 아이템은 최대치까지만 채움
		int spaceLeft = m_maxStackPerItem - it->second;
		int addAmount = min(count, spaceLeft);
		addAmount = max(addAmount, 0);

		it->second += addAmount;
		return addAmount;
	}
}

bool Inventory::RemoveItem(const string& itemId, int count)
{
	auto it = m_items.find(itemId);
	if (it == m_items.end() || it->second < count)
		return false;

	it->second -= count;
	if (it->second <= 0)
		m_items.erase(it);

	return true;
}

int Inventory::GetItemCount(const string& itemId) const
{
	auto it = m_items.find(itemId);
	if (it == m_items.end()) return 0;
	return it->second;
}

bool Inventory::HasEnough(const string& itemId, int count) const
{
	return GetItemCount(itemId) >= count;
}

bool Inventory::IsFull() const
{
	return (int)m_items.size() >= m_maxSlotCount;
}
