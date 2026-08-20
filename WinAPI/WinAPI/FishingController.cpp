#include "FishingController.h"
#include "pch.h"
#include "TileMap.h"
#include "Player.h"
#include "UIManager.h"
#include "DataManager.h"
#include "Animator.h"
#include "SoundManager.h"

void FishingController::Init(TileMap* tileMap, Player* player)
{
    m_tileMap = tileMap;
    m_player = player;
}

bool FishingController::TryStartFishing(const MathEngine::Vector2& playerPos)
{
    SoundManager::GetInstance().PlaySFX("Fishing");
    if (m_state != FishingState::Idle) return false;   // 이미 낚시 중

    if (!IsNearWater(playerPos))
    {
        UIManager::GetInstance().ShowMessage(L"근처에 물이 없습니다");
        return false;
    }

    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> waitDist(2.0f, 4.5f);

    m_waitTimer = waitDist(rng);
    m_state = FishingState::Waiting;

    if (m_player != nullptr)
    {
        Animator* animator =
            m_player->GetAnimator();


        if (animator != nullptr)
        {
            animator->PlayAndPauseAt(
                2,      // row
                6,      // 총 프레임
                0.15f,  // 프레임 시간
                5       // ★ 3번 프레임에서 정지
            );
        }
    }

    UIManager::GetInstance().ShowMessage(L"낚싯대를 던졌습니다...");
    return true;
}

bool FishingController::IsNearWater(const MathEngine::Vector2& pos) const
{
    if (m_tileMap == nullptr) return false;

    int px = (int)round(pos.x);
    int py = (int)round(pos.y);

    for (int dy = -1; dy <= 1; dy++)
        for (int dx = -1; dx <= 1; dx++)
            if (m_tileMap->IsWater(px + dx, py + dy))
                return true;

    return false;
}
void FishingController::Update(float deltaTime)
{
    if (m_state != FishingState::Waiting) return;

    m_waitTimer -= deltaTime;
    if (m_waitTimer <= 0.0f)
    {
        m_state = FishingState::Ready;

        static std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> chance(0.0f, 1.0f);

        if (chance(rng) < 0.75f) 
        {
            const string fishId = "Item_Fish";  
            int count = 1;

            if (m_player != nullptr)
            {
                SoundManager::GetInstance().PlaySFX("Fishing");
                int added = m_player->GetInventory()->AddItem(fishId, count);
                if (added > 0)
                {
                    UIManager::GetInstance().ShowMessage(L"물고기를 낚았습니다!");
                }
                else
                {
                    UIManager::GetInstance().ShowMessage(L"인벤토리가 가득 찼습니다");
                }
            }
        }
        else
        {
            SoundManager::GetInstance().PlaySFX("Fishing");
            UIManager::GetInstance().ShowMessage(L"놓쳤습니다...");
        }

        if (m_player != nullptr)
        {
            Animator* animator =
                m_player->GetAnimator();


            if (animator != nullptr)
            {
                animator->ResumeAnimation();
            }
        }

        m_state = FishingState::Idle;  
    }
}