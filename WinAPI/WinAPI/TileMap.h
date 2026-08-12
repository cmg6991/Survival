#pragma once
#include <windows.h>
#include "Graphics.h"
#include "pch.h"

class ResourceManager;

enum class TileType
{
    FLOOR,
    ROAD,
    GRASS,
    END
};


class TileMap
{
public:
    void Init();

    void Render(ID2D1DeviceContext* context, ResourceManager* resourceManager);
};