#pragma once
#include "pch.h"
#include "SaveData.h"

class SaveManager
{
public:
	static bool Save(const SaveData& data, const string& filePath);
	static bool Load(SaveData& outData, const string& filePath);
	static bool HasSaveFile(const string& filePath);
	static bool DeleteSaveFile(const string& filePath);

	static string GetSavePath(int slot);
	// 특정 슬롯에 저장
	static bool SaveSlot(const SaveData& data, int slot);

	// 특정 슬롯 불러오기
	static bool LoadSlot(SaveData& outData,int slot);

	// 특정 슬롯에 세이브 파일이 있는지 확인
	static bool HasSaveSlot(int slot);
	// 특정 슬롯 삭제
	static bool DeleteSaveSlot(int slot);
};

