#include "Tree.h"

#include "ResourceManager.h"
#include "DrawUtil.h"
#include "CameraManager.h"

//Tree::Tree() : m_tileX(10), m_tileY(10)
//{
//}
//
//void Tree::Render(HDC hdc,ResourceManager* resourceManager)
//{
//    HBITMAP bitmap = resourceManager->GetImage("Tree");
//
//    float isoX =
//        (m_tileX - m_tileY) * 64;
//
//    float isoY =
//        (m_tileX + m_tileY) * 32;
//
//    int screenX =
//        (int)isoX -
//        (int)CameraManager::GetInstance().GetX();
//
//    int screenY =
//        (int)isoY -
//        (int)CameraManager::GetInstance().GetY();
//
//    DrawBitmap(
//        hdc,
//        bitmap,
//        screenX,
//        screenY - 128); // юс╫ц
//}
