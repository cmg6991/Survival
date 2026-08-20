// WinAPI.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#include <windows.h>
#include "framework.h"
#include "WinAPI.h"
#include <crtdbg.h>
#include "GameProcess.h"
#include <objbase.h>

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
   // _CrtSetBreakAlloc(8664);
    // TODO: 여기에 코드를 입력합니다.
//#ifdef _DEBUG
//    _CrtSetDbgFlag(
//        _CRTDBG_ALLOC_MEM_DF |
//        _CRTDBG_LEAK_CHECK_DF
//    );
//
//    _CrtSetBreakAlloc(9718);
//    _CrtSetBreakAlloc(9720);
//#endif
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    CoInitialize(nullptr);

    GameProcess* gameProcess = new GameProcess;

    gameProcess->Init(hInstance);
    gameProcess->Loop();
    gameProcess->Release();

    delete gameProcess;
    CoUninitialize();
}

