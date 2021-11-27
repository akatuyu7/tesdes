#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "ProjectileManager.h"
#include "Effect.h"

class Player : public Character
{
public:
    Player();
    ~Player() override;

    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* dc, Shader* shader);
    void DrawDebugGUI();
    void DrawDebugPrimitive();
    void InputJump();
    void CollisionProjectilesVsEnemies();

protected:
    void OnLanding() override;

private:
    DirectX::XMFLOAT3 GetMoveVec() const;
    void InputMove(float elapsedTime);
    void CollisionPlayerVsEnemies();
    void InputProjectile();
private:
    Model* model = nullptr;
    float   moveSpeed = 5.0f;
    float   turnSpeed = DirectX::XMConvertToRadians(720);
    float   jumpSpeed = 20.0f;
    int     jumpCount = 0;
    int     jumpLimit = 2;
    ProjectileManager projectileManager;
    Effect* hitEffect = nullptr;
};