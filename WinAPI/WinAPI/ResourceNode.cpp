#include "ResourceNode.h"

ResourceNode::ResourceNode(const string& itemId, int minCount, int maxCount)
	: m_itemId(itemId), m_minCount(minCount), m_maxCount(maxCount)
{
}

ResourceNode::~ResourceNode()
{
}
