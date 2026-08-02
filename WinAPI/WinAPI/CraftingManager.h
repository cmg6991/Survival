#pragma once
#include "pch.h"

class Inventory;

class CraftingManager
{
public:
	static bool CanCraft(const string& recipeId, Inventory* inventory);
	static bool Craft(const string& recipeId, Inventory* inventory);
};

