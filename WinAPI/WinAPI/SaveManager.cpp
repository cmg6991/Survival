#include "SaveManager.h"
#include "pch.h"

bool SaveManager::Save(const SaveData& data, const string& filePath)
{
	json j;

	// 플레이어
	j["playerX"] = data.playerX;
	j["playerY"] = data.playerY;

	// 시간
	j["day"] = data.day;
	j["hour"] = data.hour;
	j["minute"] = data.minute;

	j["playTimeSeconds"] = data.playTimeSeconds;

	// 웨이브
	j["currentWave"] = data.currentWave;
	j["hasGlowRing"] = data.hasGlowRing;
	j["equippedWeaponId"] = data.equippedWeaponId;   // 추가
	j["equippedShieldId"] = data.equippedShieldId;
	// 인벤토리
	json inventoryJson = json::object();
	for (auto& pair : data.inventory)
	{
		inventoryJson[pair.first] = pair.second;
	}
	j["inventory"] = inventoryJson;

	json collectedJson = json::array();
	for (const string& id : data.collectedItemsIds)
	{
		collectedJson.push_back(id);
	}
	j["collectedItemsIds"] = collectedJson;

	// 저장 폴더가 없으면 생성
	filesystem::path path(filePath);
	if (path.has_parent_path())
	{
		filesystem::create_directories(path.parent_path());
	}

	wchar_t buffer[512];

	swprintf_s(
		buffer,
		L"[SAVE] filePath = %S\n",
		filePath.c_str()
	);

	OutputDebugStringW(buffer);

	std::ofstream file(filePath);
	if (!file.is_open())
	{
		OutputDebugStringA(("SaveManager::Save 실패 - 파일을 열 수 없음: " + filePath + "\n").c_str());
		return false;
	}

	file << j.dump(4);
	file.close();

	return true;
}

bool SaveManager::Load(SaveData& outData, const string& filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		OutputDebugStringA(("SaveManager::Load 실패 - 파일 없음: " + filePath + "\n").c_str());
		return false;
	}

	json j;

	try
	{
		file >> j;
	}
	catch (const json::parse_error& e)
	{
		OutputDebugStringA(("SaveManager::Load 실패 - JSON 파싱 오류: " + string(e.what()) + "\n").c_str());
		return false;
	}

	// 각 필드는 없으면 기본값으로 안전하게 처리 (세이브 포맷이 바뀌어도 크래시 방지)
	outData.playerX = j.value("playerX", 0.0f);
	outData.playerY = j.value("playerY", 0.0f);

	outData.day = j.value("day", 1);
	outData.hour = j.value("hour", 6);
	outData.minute = j.value("minute", 0);

	outData.playTimeSeconds = j.value("playTimeSeconds", 0);

	outData.currentWave = j.value("currentWave", 1);
	outData.hasGlowRing = j.value("hasGlowRing", false);
	outData.equippedWeaponId = j.value("equippedWeaponId", string(""));   // 추가
	outData.equippedShieldId = j.value("equippedShieldId", string(""));

	outData.inventory.clear();
	if (j.contains("inventory") && j["inventory"].is_object())
	{
		for (auto& [key, value] : j["inventory"].items())
		{
			outData.inventory[key] = value.get<int>();
		}
	}

	outData.collectedItemsIds.clear();
	if (j.contains("collectedItemsIds"))
	{
		for (auto& id : j["collectedItemsIds"])
		{
			outData.collectedItemsIds.push_back(id.get<string>());
		}
	}

	return true;
}

bool SaveManager::HasSaveFile(const string& filePath)
{
	return std::filesystem::exists(filePath);
}

bool SaveManager::DeleteSaveFile(const string& filePath)
{
	if (!std::filesystem::exists(filePath))
		return false;

	return std::filesystem::remove(filePath);
}

string SaveManager::GetSavePath(int slot)
{
	char buffer[64];
	sprintf_s(buffer,"Save/Save_%02d.json",slot);
	return string(buffer);
}

bool SaveManager::SaveSlot(const SaveData& data, int slot)
{
	if (slot < 1 || slot > 3)
		return false;
	return Save(data,GetSavePath(slot));
}

bool SaveManager::LoadSlot(SaveData& outData, int slot)
{
	if (slot < 1 || slot > 3)
		return false;
	return Load(outData,GetSavePath(slot));
}

bool SaveManager::HasSaveSlot(int slot)
{
	if (slot < 1 || slot > 3)
		return false;
	return HasSaveFile(GetSavePath(slot));
}

bool SaveManager::DeleteSaveSlot(int slot)
{
	if (slot < 1 || slot > 3)
		return false;
	return DeleteSaveFile(GetSavePath(slot));
}
