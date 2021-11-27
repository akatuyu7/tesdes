#include "EnemySlime.h"

EnemySlime::EnemySlime()
{
    model = new Model("Data/Model/Slime/Slime.mdl");

    scale.x = scale.y = scale.z = 0.01f;

    radius = 0.5f;
    height = 1.0f;
}

EnemySlime::~EnemySlime()
{
    delete model;
}

void EnemySlime::Update(float elapsedTime)
{
    UpdateTransform();

    UpdateVelocity(elapsedTime);

    UpdateinvincibleTimer(elapsedTime);

    model->UpdateTransform(transform);
}

void EnemySlime::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
}

void EnemySlime::DrawDebugGUI(int i)
{
    ImGui::PushID(("EnemySlime%d", i));
    if (ImGui::CollapsingHeader("EnemySlime", ImGuiTreeNodeFlags_DefaultOpen))
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

void EnemySlime::OnDead()
{
    Destroy();
}
