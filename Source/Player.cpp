#include <imgui.h>
#include "Player.h"
#include "Camera.h"
#include "Input/Input.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "projectileStraite.h"
#include "ProjectileHoming.h"

Player::Player()
{
    model = new Model("Data/Model/Jummo/Jummo.mdl");

    scale.x = scale.y = scale.z = 0.01f;

    hitEffect = new Effect("Data/Effect/Stone.efkefc");
}

Player::~Player()
{
    delete hitEffect;

    delete model;
}

void Player::Update(float elapsedTime)
{
    InputMove(elapsedTime);

    InputJump();

    InputProjectile();

    UpdateVelocity(elapsedTime);

    projectileManager.Update(elapsedTime);

    CollisionPlayerVsEnemies();

    CollisionProjectilesVsEnemies();

    UpdateTransform();

    model->UpdateTransform(transform);
}

void Player::InputMove(float elapsedTime)
{
    DirectX::XMFLOAT3 moveVec = GetMoveVec();

    Move(moveVec.x, moveVec.z, moveSpeed);
    Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
}

void Player::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);

    projectileManager.Render(dc, shader);
}

void Player::DrawDebugGUI()
{
    if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushID("Player");

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

        ImGui::InputFloat3("Velocity", &velocity.x);

        ImGui::InputFloat4("x", &transform._11);
        ImGui::InputFloat4("y", &transform._21);
        ImGui::InputFloat4("z", &transform._31);
        ImGui::InputFloat4("a", &transform._41);

        ImGui::PopID();
    }

    projectileManager.DrawDebugGUI();
}

DirectX::XMFLOAT3 Player::GetMoveVec() const
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    Camera& camera = Camera::Instance();
    const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
    const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

    float cameraRightX = cameraRight.x;
    float cameraRightZ = cameraRight.z;
    float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
    if (cameraRightLength > 0.0f)
    {
        cameraRightX /= cameraRightLength;
        cameraRightZ /= cameraRightLength;
    }

    float cameraFrontX = cameraFront.x;
    float cameraFrontZ = cameraFront.z;
    float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
    if (cameraFrontLength > 0.0f)
    {
        cameraFrontX /= cameraFrontLength;
        cameraFrontZ /= cameraFrontLength;
    }

    DirectX::XMFLOAT3 vec;
    vec.x = cameraRightX * ax + cameraFrontX * ay;
    vec.z = cameraRightZ * ax + cameraFrontZ * ay;
    vec.y = 0.0;

    return vec;
}

void Player::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));

    projectileManager.DrawDebugPrimitive();
}

void Player::CollisionPlayerVsEnemies()
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemy = enemyManager.GetEnemy(i);

        DirectX::XMFLOAT3 outPosition;
        if (Collision::IntersectCylinderVsCylinder(
            this->position,
            this->radius,
            this->height,
            enemy->GetPosition(),
            enemy->GetRadius(),
            enemy->GetHeight(),
            outPosition))
        {
            DirectX::XMVECTOR playerV = DirectX::XMLoadFloat3(&this->position);
            DirectX::XMVECTOR enemyV = DirectX::XMLoadFloat3(&enemy->GetPosition());
            DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(playerV, enemyV);
            vec = DirectX::XMPlaneNormalize(vec);
            DirectX::XMFLOAT3 p;
            DirectX::XMStoreFloat3(&p, vec);
            if (p.y > 0.8)
            {
                Jump(jumpSpeed * 0.7f);
                enemy->ApplyDamage(1, 0);
            }
            else
            {
                enemy->SetPosition(outPosition);
            }
        }
    }
}

void Player::InputJump()
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (gamePad.GetButtonDown() & GamePad::BTN_A)
    {
        if (jumpCount < jumpLimit)
        {
            Jump(jumpSpeed);
            ++jumpCount;
        }
    }
}

void Player::CollisionProjectilesVsEnemies()
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    int projectileCount = projectileManager.GetProjectileCount();
    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < projectileCount; ++i)
    {
        Projectile* projectile = projectileManager.GetProjectile(i);
        for (int j = 0; j < enemyCount; ++j)
        {
            Enemy* enemy = enemyManager.GetEnemy(j);

            DirectX::XMFLOAT3 outPosition;
            if (Collision::IntersectSphereVsCylinder(
                projectile->GetPosition(),
                projectile->GetRadius(),
                enemy->GetPosition(),
                enemy->GetRadius(),
                enemy->GetHeight(),
                outPosition))
            {
                if (enemy->ApplyDamage(1, 0.5f))
                {
                    //‚«”ò‚Î‚·
                    {
                        const float power = 10.0f;
                        DirectX::XMFLOAT3 impulse;
                        const DirectX::XMFLOAT3 pPos = projectile->GetPosition();
                        const DirectX::XMFLOAT3 ePos = enemy->GetPosition();
                        float vX = ePos.x - pPos.x;
                        float vZ = ePos.z - pPos.z;
                        float lebgth = sqrtf(vX * vX + vZ * vZ);
                        vX /= lebgth;
                        vZ /= lebgth;

                        impulse.x = vX * power;
                        impulse.z = vZ * power;
                        impulse.y = 0.5f * power;


                        enemy->AddImpulse(impulse);
                    }

                    {
                        DirectX::XMFLOAT3 e = enemy->GetPosition();
                        e.y += enemy->GetHeight() * 0.5f;
                        hitEffect->Play(e);
                    }
                    projectile->Destroy();
                }
            }
        }
    }
}

void Player::OnLanding()
{
    jumpCount = 0;
}

void Player::InputProjectile()
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    if (gamePad.GetButtonDown() & GamePad::BTN_X)
    {
        DirectX::XMFLOAT3 dir;

        //DirectX::XMFLOAT3 a;
        //a.y = DirectX::XMConvertToDegrees(angle.y);
        //dir.x = fabsf(a.y) < 90 ? a.y / 90.0f : a.y > 0 ? 2 - (a.y / 90.0f) : -2 - (a.y / 90.0f);
        //dir.y = 0.0f;
        //dir.z = a.y > 0 ? 1 - (a.y / 90.0f) : 1 + (a.y / 90.0f);

        dir.x = transform._31;
        dir.y = transform._32;
        dir.z = transform._33;

        DirectX::XMVECTOR Dir = DirectX::XMLoadFloat3(&dir);
        Dir = DirectX::XMVector3Normalize(Dir);
        DirectX::XMStoreFloat3(&dir, Dir);

        DirectX::XMFLOAT3 pos;
        pos.x = position.x;
        pos.y = position.y + height * 0.5f;
        pos.z = position.z;

        ProjectileStraite* projectile = new ProjectileStraite(&projectileManager);
        projectile->Launch(dir, pos);
    }

    if (gamePad.GetButtonDown() & GamePad::BTN_Y)
    {
        DirectX::XMFLOAT3 dir;

        //DirectX::XMFLOAT3 a;
        //a.y = DirectX::XMConvertToDegrees(angle.y);
        //dir.x = fabsf(a.y) < 90 ? a.y / 90.0f : a.y > 0 ? 2 - (a.y / 90.0f) : -2 - (a.y / 90.0f);
        //dir.y = 0.0f;
        //dir.z = a.y > 0 ? 1 - (a.y / 90.0f) : 1 + (a.y / 90.0f);

        dir.x = transform._31;
        dir.y = transform._32;
        dir.z = transform._33;

        DirectX::XMVECTOR Dir = DirectX::XMLoadFloat3(&dir);
        Dir = DirectX::XMVector3Normalize(Dir);
        DirectX::XMStoreFloat3(&dir, Dir);

        DirectX::XMFLOAT3 pos;
        pos.x = position.x;
        pos.y = position.y + height * 0.5f;
        pos.z = position.z;

        DirectX::XMFLOAT3 target;
        target.x = pos.x + dir.x * moveSpeed;
        target.y = pos.y + dir.y * moveSpeed;
        target.z = pos.z + dir.z * moveSpeed;

        float dist = FLT_MAX;
        EnemyManager& enemyManager = EnemyManager::Instance();
        DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&target);
        int enemyCount = enemyManager.GetEnemyCount();

        for (int i = 0; i < enemyCount; ++i)
        {
            Enemy* enemy = enemyManager.GetEnemy(i);
            DirectX::XMVECTOR EnemyPos = DirectX::XMLoadFloat3(&enemy->GetPosition());
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Position, EnemyPos);
            DirectX::XMVECTOR Length = DirectX::XMVector3LengthSq(Vec);
            float length;
            DirectX::XMStoreFloat(&length, Length);
        
            if (dist > length)
            {
                dist = length;
                target = enemy->GetPosition();
                target.y += enemy->GetHeight() * 0.5f;
            }
        }

        ProjectileHoming* projectile = new ProjectileHoming(&projectileManager);
        projectile->Launch(dir, pos, target);
    }
}