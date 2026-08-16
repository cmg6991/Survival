#pragma once
#include "pch.h"

struct ItemData
{
	string id;
	string name;
	string image;
	string type;
	string weaponType;
	string weaponSpriteKey;
	int defenseValue;

	int damage;
	float range;
	float speed;

	int healAmount;
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

struct MonsterData
{
	string id;
	string name;
	string image;
	int health;
	float moveSpeed;
	int contactDamage;
	int cellWidth;
	int cellHeight;
	int animColumn;
	float colliderRadius;
	float scale;
};

struct ResourceDrop
{
	string itemId;
	float chance;
	int minCount;
	int maxCount;
};

struct ResourceObjectData
{
	string id;
	string name;
	string image;
	string interactType;
	float scale = 1.0f;

	string baseItemId;
	int baseMinCount = 1;
	int baseMaxCount = 1;

	vector<ResourceDrop> bonusDrops;
};