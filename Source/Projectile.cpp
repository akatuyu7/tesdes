#include "Projectile.h"
#include "Graphics/Graphics.h"
#include "ProjectileManager.h"

Projectile::Projectile(ProjectileManager* manager)
    : manager(manager)
{
    manager->Register(this);
}

void Projectile::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));
}

void Projectile::UpdateTransform()
{
    //transform._11 = scale.x;
    //transform._12 = 0.0f;
    //transform._13 = 0.0f;
    //transform._14 = 0.0f;
    //transform._21 = 0.0f;
    //transform._22 = scale.y;
    //transform._23 = 0.0f;
    //transform._24 = 0.0f;
    //transform._31 = 0.0f;
    //transform._32 = 0.0f;
    //transform._33 = scale.z;
    //transform._34 = 0.0f;
    //transform._41 = position.x;
    //transform._42 = position.y;
    //transform._43 = position.z;
    //transform._44 = 1.0f;

    DirectX::XMVECTOR Front, Up, Right;
    
    Front = DirectX::XMLoadFloat3(&direction);
    Front = DirectX::XMPlaneNormalize(Front);

    DirectX::XMFLOAT3 kUp(0.0f, scale.y, 0.0f);
    Up = DirectX::XMLoadFloat3(&kUp);
    Up = DirectX::XMPlaneNormalize(Up);

    Right = DirectX::XMVector3Cross(Front, Up);

    Up = DirectX::XMVector3Cross(Front, Right);
    
    DirectX::XMFLOAT3 right, up, front;
    DirectX::XMStoreFloat3(&right, Right);
    DirectX::XMStoreFloat3(&up, Up);
    DirectX::XMStoreFloat3(&front, Front);

    transform._11 = right.x * scale.x;
    transform._12 = right.y * scale.x;
    transform._13 = right.z * scale.x;
    transform._14 = 0.0f;
    transform._21 = up.x * scale.y;
    transform._22 = up.y * scale.y;
    transform._23 = up.z * scale.y;
    transform._24 = 0.0f;
    transform._31 = front.x * scale.z;
    transform._32 = front.y * scale.z;
    transform._33 = front.z * scale.z;
    transform._34 = 0.0f;
    transform._41 = position.x;
    transform._42 = position.y;
    transform._43 = position.z;
    transform._44 = 1.0f;

    this->direction = front;
}

void Projectile::DrawDebugGUI()
{
    ImGui::PushID(("Projectile%d"));
    if (ImGui::CollapsingHeader("Projectile", ImGuiTreeNodeFlags_DefaultOpen))
    {

        ImGui::InputFloat3("Position", &position.x);
        ImGui::InputFloat3("Direction", &direction.x);
        ImGui::InputFloat3("Scale", &scale.x);
    }
    ImGui::PopID();
}

void Projectile::Destroy()
{
    manager->Remove(this);
}
