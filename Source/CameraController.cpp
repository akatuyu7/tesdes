#include <imgui.h>
#include "CameraController.h"
#include "Camera.h"
#include "Input/Input.h"

void CameraController::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisRX();
    float ay = gamePad.GetAxisRY();
    float speed = rollSpeed * elapsedTime;

    angle.x -= ay * speed;
    angle.y += ax * speed;

    if (angle.x > maxAngleX)
        angle.x = maxAngleX;

    if (angle.x < minAngleX)
        angle.x = minAngleX;

    if (angle.y < -DirectX::XM_PI) 
        angle.y += DirectX::XM_2PI;

    if (angle.y > DirectX::XM_PI)
        angle.y -= DirectX::XM_2PI;

    DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

    DirectX::XMVECTOR Front = Transform.r[2];
    DirectX::XMFLOAT3 front;
    DirectX::XMStoreFloat3(&front, Front);

    DirectX::XMFLOAT3 eye;
    eye.x = target.x - range * front.x;
    eye.y = target.y - range * front.y;
    eye.z = target.z - range * front.z;

    Camera::Instance().SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));
}

void CameraController::DrawDebugGUI()
{
    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushID("Camera");

        DirectX::XMFLOAT3 a;
        a.x = DirectX::XMConvertToDegrees(angle.x);
        a.y = DirectX::XMConvertToDegrees(angle.y);
        a.z = DirectX::XMConvertToDegrees(angle.z);
        ImGui::InputFloat3("Angle", &a.x);
        angle.x = DirectX::XMConvertToRadians(a.x);
        angle.y = DirectX::XMConvertToRadians(a.y);
        angle.z = DirectX::XMConvertToRadians(a.z);
        ImGui::InputFloat("Range", &range);

        ImGui::PopID();
    }
}