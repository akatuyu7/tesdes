#include "projectileStraite.h"

ProjectileStraite::ProjectileStraite(ProjectileManager* manager)
    : Projectile(manager)
{
    model = new Model("Data/Model/Sword/Sword.mdl");
    
    scale.x = scale.y = scale.z = 3.0f;
}

ProjectileStraite::~ProjectileStraite()
{
    delete model;
}

void ProjectileStraite::Update(float elapsedTime)
{
    lifeTimer -= elapsedTime;
    if (lifeTimer <= 0)
    {
        Destroy();
    }

    float speed = this->speed * elapsedTime;
    position.x += direction.x * speed;
    position.y += direction.y * speed;
    position.z += direction.z * speed;

    UpdateTransform();

    model->UpdateTransform(transform);
}

void ProjectileStraite::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
}

void ProjectileStraite::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position)
{
    this->direction = direction;
    this->position = position;
}