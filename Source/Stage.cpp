#include "Stage.h"

static Stage* instance = nullptr;

Stage::Stage()
{
    instance = this;

    model = new Model("Data/Model/ExampleStage/ExampleStage.mdl");
}

Stage::~Stage()
{
    delete model;
}

void Stage::Update(float elapsedTime)
{
    //‰É
}

void Stage::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
}

bool Stage::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::IntersectRayVsModel(start, end, model, hit);
}

Stage& Stage::Instence()
{
    return *instance;
}
