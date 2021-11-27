#pragma once

#include <DirectXMath.h>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

class EffectManager
{
private:
    EffectManager() {}
    ~EffectManager() {}

public:
    static EffectManager& Instance()
    {
        static EffectManager instance;
        return instance;
    }

    void Initialize();

    void Fainalize();

    void Update(float elapsedTime);

    void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

    Effekseer::Manager* GetEffekseerManager() { return effekseerManager; }

private:
    Effekseer::Manager*             effekseerManager = nullptr;
    EffekseerRenderer::Renderer*    effekseerRenderer = nullptr;
};