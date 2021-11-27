#include "Enemy.h"
#include "EnemyManager.h"
//#include "Graphics/Graphics.h"

void Enemy::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

void Enemy::DrawDebugGUI(int i)
{
    ImGui::PushID(("Enemy%d", i));
    if (ImGui::CollapsingHeader("Enemy", ImGuiTreeNodeFlags_DefaultOpen))
    {

        ImGui::InputFloat3("Position", &position.x);
        DirectX::XMFLOAT3 a;
        a.x = DirectX::XMConvertToDegrees(angle.x);
        a.y = DirectX::XMConvertToDegrees(angle.y);
        a.z = DirectX::XMConvertToDegrees(angle.z);
        ImGui::InputFloat3("Angle", &a.x);
        angle.x = DirectX::XMConvertToRadians(a.x);
        angle.y = DirectX::XMConvertToRadians(a.y);
        angle.z = DirectX::XMConvertToRadians(a.z);

        ImGui::InputFloat3("Scale", &scale.x);

        ImGui::InputInt("Health", &health);
    }
    ImGui::PopID();
}

void Enemy::Destroy()
{
    EnemyManager::Instance().Remove(this);
}
