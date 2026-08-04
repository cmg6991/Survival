#pragma once
#include "pch.h"

struct ItemData
{
	string id;
	string name;
	string image;
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
	vector<Ingredient> ingredients;
};