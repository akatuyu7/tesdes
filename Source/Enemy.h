#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Graphics.h"
#include "Character.h"

class Enemy : public Character
{
public:
    Enemy() {}
    ~Enemy() override {}

    virtual void Update(float elapsedTime) = 0;
    virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;
    virtual void DrawDebugPrimitive();
    virtual void DrawDebugGUI(int i);
    void Destroy();
};