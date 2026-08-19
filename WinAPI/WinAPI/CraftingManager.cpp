#include "CraftingManager.h"
#include "DataManager.h"
#include "Inventory.h"
#include "Weapon.h"
#include <random>

bool CraftingManager::CanCraft(const string& recipeId, Inventory* inventory, Weapon* weapon)
{
	for (const RecipeData& recipe : DataManager::GetInstance().GetRecipeList())
	{
		if (recipe.id != recipeId) continue;

		for (const Ingredient& ing : recipe.ingredients)
		{
			if (weapon != nullptr && ing.ingredientId == weapon->GetWeaponId())
				continue;
			if (!inventory->HasEnough(ing.ingredientId, ing.count))
				return false;
		}
		return true;
	}
	return false;
}

CraftResult CraftingManager::Craft(const string& recipeId, Inventory* inventory, Weapon* weapon)
{
	if (!CanCraft(recipeId, inventory,weapon))
		return CraftResult::None;

	for (const RecipeData& recipe : DataManager::GetInstance().GetRecipeList())
	{
		if (recipe.id != recipeId) continue;

		for (const Ingredient& ing : recipe.ingredients)
		{
			if (weapon != nullptr && ing.ingredientId == weapon->GetWeaponId())
				continue;
			inventory->RemoveItem(ing.ingredientId, ing.count);
		}

		static std::mt19937 rng(std::random_device{}());
		std::uniform_real_distribution<float> dist(0.0f, 1.0f);
		bool success = dist(rng) <= recipe.successRate;

		if (!success)
			return CraftResult::Failed; // 재료는 이미 소모됐지만 결과물은 안 나옴

		if (recipe.resultId != "Item_GlowRing")
		{
			inventory->AddItem(
				recipe.resultId,
				recipe.resultCount
			);
		}

		return CraftResult::Success;
	}
	return CraftResult::None;
}
