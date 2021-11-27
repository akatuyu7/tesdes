#pragma once

#include "Graphics/Model.h"
#include "Projectile.h"

class ProjectileStraite : public Projectile
{
public:
    ProjectileStraite(ProjectileManager* manager);
    ~ProjectileStraite() override;

    void Update(float elapsedTime) override;

    void Render(ID3D11DeviceContext* dc, Shader* shader) override;

    void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position);

private:
    Model*  model = nullptr;
    float   speed = 10.0f;
    float   lifeTimer = 3.0f;
};