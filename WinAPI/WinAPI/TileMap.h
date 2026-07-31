#pragma once
#include <windows.h>
#include "Graphics.h"

class ResourceManager;

class TileMap
{
public:
    void Init();

    void Render(ID2D1DeviceContext* context ,ResourceManager* resourceManager);

private:
    static const int WIDTH = 100;
    static const int HEIGHT = 100;
};

