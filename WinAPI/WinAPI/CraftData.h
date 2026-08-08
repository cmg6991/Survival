#pragma once
#include "pch.h"

struct ItemData
{
	string id;
	string name;
	string image;
	string type;
	string weaponType;
};

struct Ingredient
{
	string ingredientId;
	int count;
};


struct RecipeData
{
	string id;
	string resultId;
	int resultCount = 1;
	string station;
	bool isWeaponUpgrade = false;
	float successRate = 1.f;
	vector<Ingredient> ingredients;
};