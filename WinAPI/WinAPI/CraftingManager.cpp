#include "CraftingManager.h"
#include "DataManager.h"
#include "Inventory.h"

bool CraftingManager::CanCraft(const string& recipeId, Inventory* inventory)
{
	for (const RecipeData& recipe : DataManager::GetInstance().GetRecipeList())
	{
		if (recipe.id != recipeId) continue;

		for (const Ingredient& ing : recipe.ingredients)
		{
			if (!inventory->HasEnough(ing.ingredientId, ing.count))
				return false;
		}
		return true;
	}
	return false;
}

bool CraftingManager::Craft(const string& recipeId, Inventory* inventory)
{
	if (!CanCraft(recipeId, inventory))
		return false;

	for (const RecipeData& recipe : DataManager::GetInstance().GetRecipeList())
	{
		if (recipe.id != recipeId) continue;

		for (const Ingredient& ing : recipe.ingredients)
		{
			inventory->RemoveItem(ing.ingredientId, ing.count);
		}

		inventory->AddItem(recipe.resultId, recipe.resultCount);
		return true;
	}
	return false;
}
