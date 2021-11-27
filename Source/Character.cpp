#include "Character.h"
#include "Stage.h"

void Character::UpdateTransform()
{
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

    DirectX::XMMATRIX W = S * R * T;

    DirectX::XMStoreFloat4x4(&transform, W);
}

bool Character::ApplyDamage(int damage, float invincibleTime)
{
    if (damage <= 0) return false;

    if (health <= 0) return false;

    if (invincibleTimer > 0) return false;

    health -= damage;
    invincibleTimer = invincibleTime;

    if (health <= 0)
    {
        OnDead();
    }
    else
    {
        OnDamaged();
    }

    return true;
}

void Character::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
    velocity.x += impulse.x;
    velocity.y += impulse.y;
    velocity.z += impulse.z;
}

void Character::Move(float vx, float vz, float speed)
{
    moveVecX = vx;
    moveVecZ = vz;

    maxMoveSpeed = speed;
}

void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
    speed *= elapsedTime;

    if (!vx && !vz) return;

    float lange = sqrtf(vx * vx + vz * vz);
    if (lange > 0.0f)
    {
        vx /= lange;
        vz /= lange;
    }

    float frontX = sinf(angle.y);
    float frontZ = cosf(angle.y);

    float dot = (frontX * vx) + (frontZ * vz);
    float rot = 1.0f - dot;

    float cross = (frontX * vz) - (frontZ * vx);

    if (cross < 0.0f)
    {
        angle.y += rot * speed;
    }
    else
    {
        angle.y -= rot * speed;
    }

    DirectX::XMFLOAT3 a;
    a.y = DirectX::XMConvertToDegrees(angle.y);

    if (a.y > 180.0f)
    {
        a.y -= 360.0f;
    }

    if (a.y < -180.0f)
    {
        a.y += 360.0f;
    }

    angle.y = DirectX::XMConvertToRadians(a.y);
}

void Character::Jump(float speed)
{
    velocity.y = speed;
}

void Character::UpdateVelocity(float elapsedTime)
{
    float elapsedFrame = 60.0f * elapsedTime;

    UpdateVeticalVelocity(elapsedFrame);

    UpdateHorizontalVelocity(elapsedFrame);

    UpdateVeticalMove(elapsedTime);

    UpdateHorizontalMove(elapsedTime);
}

void Character::UpdateinvincibleTimer(float elapsedTime)
{
    if (invincibleTimer > 0.0f)
    {
        invincibleTimer -= elapsedTime;
    }
}

void Character::UpdateVeticalVelocity(float elapsedFrame)
{
    velocity.y += gravity * elapsedFrame;
}

void Character::UpdateVeticalMove(float elapsedTime)
{
    float my = velocity.y * elapsedTime;
    slopeRate = 0.0f;

    if (my < 0.0f)
    {
        DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
        DirectX::XMFLOAT3 end = { position.x, position.y + my, position.z };

        HitResult hit;
        if (Stage::Instence().RayCast(start, end, hit))
        {
            position.y = hit.position.y;

            if (!isGround)
            {
                OnLanding();
            }
            isGround = true;
            velocity.y = 0.0f;
        }
        else
        {
            position.y += my;
            isGround = false;
        }
    }
    else if (my > 0.0f)
    {
        position.y += my;
        isGround = false;
    }
}

void Character::UpdateHorizontalVelocity(float elapsedFrame)
{
    float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    if (length > 0.0f)
    {
        float friction = this->friction * elapsedFrame;

        if(!isGround) friction = this->friction * 0.05f * elapsedFrame;

        if (length > friction)
        {
            float vx = velocity.x / length;
            float vz = velocity.z / length;
            
            velocity.x -= vx * friction;
            velocity.z -= vz * friction;
        }
        else
        {
            velocity.x = 0;
            velocity.z = 0;
        }
    }

    if (length <= maxMoveSpeed)
    {
        float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
        if (moveVecLength > 0.0f)
        {
            float acceleration = this->acceleration * elapsedFrame;

            if (!isGround) acceleration = this->airControl * elapsedFrame;

            velocity.x += moveVecX * acceleration;
            velocity.z += moveVecZ * acceleration;

            float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
            if (length > maxMoveSpeed)
            {
                float vx = velocity.x / length;
                float vz = velocity.z / length;

                velocity.x = vx * maxMoveSpeed;
                velocity.z = vz * maxMoveSpeed;
            }
        }
    }
    moveVecX = 0.0f;
    moveVecZ = 0.0f;
}

void Character::UpdateHorizontalMove(float elapsedTime)
{
    position.x += velocity.x * elapsedTime;
    position.z += velocity.z * elapsedTime;
}
