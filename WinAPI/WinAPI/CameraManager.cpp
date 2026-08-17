#include "CameraManager.h"
#include "InputManager.h"
#include "Transform.h"
#include "TileManager.h"

void CameraManager::Init()
{
    m_x = 0.0f;
    m_y = 0.0f;
}

void CameraManager::Follow(Transform* transform)
{
    MathEngine::Vector2 screen = TileManager::GetInstance().TileToScreen( transform->GetPostion());

    m_x = screen.x - 640;
    m_y = screen.y - 360;
    //float playerOffsetY = 130.0f; // 캐릭터 높이에 맞게 40~80 사이로 조절해보세요.

    //// 3. 카메라 위치 계산 (화면 중앙 960, 540 기준)
    //m_x = screen.x - 700.0f;
    //m_y = (screen.y + playerOffsetY) - 540.0f; // +가 아니라 - 로 변경!
}
