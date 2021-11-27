#include "Collision.h"

bool Collision::IntersectSphereVsSphere(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    DirectX::XMFLOAT3& outPositionB)
{
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec       = DirectX::XMVectorSubtract(PositionA, PositionB);
    DirectX::XMVECTOR LengthSq  = DirectX::XMVector3Length(Vec);
    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, LengthSq);

    float range = radiusA + radiusB;
    if(lengthSq > range)
    {
        return false;
    }
    
    Vec = DirectX::XMPlaneNormalize(Vec);
    Vec = DirectX::XMVectorScale(Vec, range - lengthSq);
    PositionB = DirectX::XMVectorSubtract(PositionB, Vec);

    DirectX::XMStoreFloat3(&outPositionB, PositionB);

    return true;
}

bool Collision::IntersectCylinderVsCylinder(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    float heightA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    float heightB,
    DirectX::XMFLOAT3& outPositionB)
{
    if (positionA.y > positionB.y + heightB)
    {
        return false;
    }

    if (positionB.y > positionA.y + heightA)
    {
        return false;
    }

    DirectX::XMFLOAT3 vec = { positionA.x - positionB.x, positionA.y - positionB.y, positionA.z - positionB.z };
    float length = sqrtf(vec.x * vec.x + vec.z * vec.z);

    float range = radiusA + radiusB;
    if (length > range)
    {
        return false;
    }

    vec.x /= length;
    vec.y /= length;
    vec.z /= length;

    outPositionB.x = positionB.x - vec.x * (range - length);
    outPositionB.y = positionB.y - vec.y * (range - length);
    outPositionB.z = positionB.z - vec.z * (range - length);

    return true;
}

bool Collision::IntersectSphereVsCylinder(const DirectX::XMFLOAT3& spherePosition, float sphereRadius, const DirectX::XMFLOAT3& cylinderPosition, float cylinderRadius, float cylinderHeight, DirectX::XMFLOAT3& outCylinderPosition)
{
    if (spherePosition.y - sphereRadius > cylinderPosition.y + cylinderHeight)
    {
        return false;
    }

    if (spherePosition.y + sphereRadius < cylinderPosition.y)
    {
        return false;
    }

    DirectX::XMFLOAT3 vec = { spherePosition.x - cylinderPosition.x, spherePosition.y - cylinderPosition.y, spherePosition.z - cylinderPosition.z };
    float length = sqrtf(vec.x * vec.x + vec.z * vec.z);

    float range = sphereRadius + cylinderRadius;
    if (length > range)
    {
        return false;
    }

    vec.x /= length;
    vec.y /= length;
    vec.z /= length;

    outCylinderPosition.x = cylinderPosition.x - vec.x * (range - length);
    outCylinderPosition.y = cylinderPosition.y - vec.y * (range - length);
    outCylinderPosition.z = cylinderPosition.z - vec.z * (range - length);


    return true;
}

bool Collision::IntersectRayVsModel(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const Model* model, HitResult& result)
{
    DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
    DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
    DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
    DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

    DirectX::XMStoreFloat(&result.distance, WorldRayLength);

    bool hit = false;
    const ModelResource* resource = model->GetResource();
    for (const ModelResource::Mesh& mesh : resource->GetMeshes())
    {
        const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

        DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
        DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

        DirectX::XMVECTOR Start = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
        DirectX::XMVECTOR End = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
        DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);
        DirectX::XMVECTOR Dir = DirectX::XMVector3Normalize(Vec);
        DirectX::XMVECTOR Length = DirectX::XMVector3Length(Vec);

        float neart;
        DirectX::XMStoreFloat(&neart, Length);

        const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
        const std::vector<UINT> indices = mesh.indices;

        int materialIndex = -1;
        DirectX::XMVECTOR HitPosition;
        DirectX::XMVECTOR HitNormal;
        for (const ModelResource::Subset& subset : mesh.subsets)
        {
            for (UINT i = 0; i < subset.indexCount; i += 3)
            {
                UINT index = subset.startIndex + i;

                const ModelResource::Vertex& a = vertices.at(indices.at(index));
                const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
                const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

                DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
                DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
                DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

                DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
                DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
                DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

                DirectX::XMVECTOR Normal = DirectX::XMVector3Cross(AB, BC);

                DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Dir, Normal);
                float dot;
                DirectX::XMStoreFloat(&dot, Dot);
                if (dot >= 0) continue;

                DirectX::XMVECTOR V = DirectX::XMVectorSubtract(A, Start);
                DirectX::XMVECTOR T = DirectX::XMVectorDivide(DirectX::XMVector3Dot(V, Normal), Dot);
                float t;
                DirectX::XMStoreFloat(&t, T);
                if (t < 0.0f || t > neart) continue;

                DirectX::XMVECTOR Position = DirectX::XMVectorAdd(Start, DirectX::XMVectorMultiply(Dir, T));

                DirectX::XMVECTOR V1 = DirectX::XMVectorSubtract(Position, B);
                DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(AB, V1);
                DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Normal, Cross1);
                float dot1;
                DirectX::XMStoreFloat(&dot1, Dot1);
                if (dot1 < 0) continue;

                DirectX::XMVECTOR V2 = DirectX::XMVectorSubtract(Position, C);
                DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(BC, V2);
                DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Normal, Cross2);
                float dot2;
                DirectX::XMStoreFloat(&dot2, Dot2);
                if (dot2 < 0) continue;

                DirectX::XMVECTOR V3 = DirectX::XMVectorSubtract(Position, A);
                DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(CA, V3);
                DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Normal, Cross3);
                float dot3;
                DirectX::XMStoreFloat(&dot3, Dot3);
                if (dot3 < 0) continue;

                neart = t;

                HitPosition = Position;
                HitNormal = Normal;
                materialIndex = subset.materialIndex;
            }
        }
        if (materialIndex >= 0)
        {
            DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
            DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
            DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
            float distance;
            DirectX::XMStoreFloat(&distance, WorldCrossLength);

            if (result.distance > distance)
            {
                DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(HitNormal, WorldTransform);

                result.distance = distance;
                result.materialIndex = materialIndex;
                DirectX::XMStoreFloat3(&result.position, WorldPosition);
                DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));
                hit = true;
            }
        }
    }
    return hit;

    //if (end.y < 0.0f)
    //{
    //    result.position.x = end.x;
    //    result.position.y = 0.0f;
    //    result.position.z = end.z;
    //    result.normal.x = 0.0f;
    //    result.normal.y = 1.0f;
    //    result.normal.z = 0.0f;
    //    return true;
    //}
    //return false;
}
