#include "ProjectileHoming.h"

ProjectileHoming::ProjectileHoming(ProjectileManager* manager)
    : Projectile(manager)
{
    model = new Model("Data/Model/Sword/Sword.mdl");

    scale.x = scale.y = scale.z = 3.0f;
}

ProjectileHoming::~ProjectileHoming()
{
    delete model;
}

void ProjectileHoming::Update(float elapsedTime)
{
    lifeTimer -= elapsedTime;
    if (lifeTimer <= 0)
    {
        Destroy();
    }

    float moveSpeed = this->moveSpeed * elapsedTime;
    position.x += direction.x * moveSpeed;
    position.y += direction.y * moveSpeed;
    position.z += direction.z * moveSpeed;

    if (chaker)
    {
        float turnSpeed = this->turnSpeed * elapsedTime;
        DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);
        DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&target);
        DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Target, Position);

        DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
        float lengthSq;
        DirectX::XMStoreFloat(&lengthSq, LengthSq);

        if (lengthSq < 5.0f)
        {
            this->chaker = false;
        }

        if (lengthSq > 0.00001f)
        {
            Vec = DirectX::XMPlaneNormalize(Vec);

            DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);
            Direction = DirectX::XMPlaneNormalize(Direction);

            DirectX::XMVECTOR Dot = DirectX::XMPlaneDot(Vec, Direction);

            float dot;
            DirectX::XMStoreFloat(&dot, Dot);

            float rot = 1.0f - dot;
            if (rot > turnSpeed)
            {
                rot = turnSpeed;
            }

            if (fabsf(rot) > 0.00001f)
            {
                DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Direction, Vec);

                DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(Axis, rot);

                DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
                Transform = DirectX::XMMatrixMultiply(Transform, Rotation);

                Direction = DirectX::XMPlaneNormalize(Transform.r[2]);
                DirectX::XMStoreFloat3(&direction, Direction);
            }
        }
    }


    UpdateTransform();

    model->UpdateTransform(transform);
}

void ProjectileHoming::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
}

void ProjectileHoming::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& target)
{
    this->direction = direction;
    this->position = position;
    this->target = target;

    UpdateTransform();
}
