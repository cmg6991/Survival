#pragma once
#include "pch.h"
#include "SaveData.h"

class SaveManager
{
public:
	static bool Save(const SaveData& data, const string& filePath = "Save/save.json");
	static bool Load(SaveData& outData, const string& filePath = "Save/save.json");
	static bool HasSaveFile(const string& filePath = "Save/save.json");
	static bool DeleteSaveFile(const string& filePath = "Save/save.json");

};

