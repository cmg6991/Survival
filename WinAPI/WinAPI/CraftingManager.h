#pragma once
#include "pch.h"

class Weapon;
class Inventory;

enum class CraftResult
{
	Success,
	Failed,
	None
};

class CraftingManager
{
public:
	static bool CanCraft(const string& recipeId, Inventory* inventory,Weapon* weapon = nullptr);
	static CraftResult Craft(const string& recipeId, Inventory* inventory, Weapon* weapon = nullptr);
};

