#pragma once
#include "pch.h"

struct SaveData
{
	float playerX = 0;
	float playerY = 0;

	int day = 1;
	int hour = 6;
	int minute = 0;

	int currentWave = 1;

	unordered_map<string, int> inventory;

	vector<string> collectedItemsIds;

	int playTimeSeconds = 0;

	bool hasGlowRing = false;

	string equippedWeaponId = "";
	string equippedShieldId = "";
};