#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

class SceneLoading : public Scene
{
public:
    SceneLoading(Scene* nextScene) : nextScene(nextScene) {}
    ~SceneLoading() override {}

    void Initialize() override;

    void Finalize() override;

    void Update(float elapsedTime) override;

    void Render() override;

private:
    static void LoadingThread(SceneLoading* scene);

private:
    Sprite* sprite = nullptr;
    float angle = 0.0f;
    Scene* nextScene = nullptr;
};