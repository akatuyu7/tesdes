#include <EnemyManager.h>
#include <imgui.h>
#include "Collision.h"

void EnemyManager::Update(float elapsedTime)
{
    for (Enemy* enemy : enemies)
    {
        enemy->Update(elapsedTime);
    }

    for (Enemy* enemy : removes)
    {
        std::vector<Enemy*>::iterator it = std::find(enemies.begin(), enemies.end(), enemy);

        if (it != enemies.end())
        {
            enemies.erase(it);
        }

        delete enemy;
    }
    removes.clear();

    CollisionEnemyVsEnemies();
}

void EnemyManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    for (Enemy* enemy : enemies)
    {
        enemy->Render(dc, shader);
    }
}

void EnemyManager::Register(Enemy* enemy)
{
    enemies.emplace_back(enemy);
}

void EnemyManager::Clear()
{
    for (Enemy* enemy : enemies)
    {
        delete enemy;
    }
}

void EnemyManager::Remove(Enemy* enemy)
{
    for (Enemy* e : removes)
    {
        if(e == enemy) return;
    }
    removes.emplace_back(enemy);
}

void EnemyManager::DrawDebugGUI()
{
    if (ImGui::CollapsingHeader("Enemies", ImGuiTreeNodeFlags_DefaultOpen))
    {
        int enemyCount = GetEnemyCount();
        for (int i = 0; i < enemyCount; ++i)
        {
            GetEnemy(i)->DrawDebugGUI(i);
        }
    }
}

void EnemyManager::DrawDebugPrimitive()
{
    for (Enemy* enemy : enemies)
    {
        enemy->DrawDebugPrimitive();
    }
}

void EnemyManager::CollisionEnemyVsEnemies()
{
    int enemyCount = GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemy1 = GetEnemy(i);
        for (int j = i + 1; j < enemyCount; ++j)
        {
            Enemy* enemy2 = GetEnemy(j);

            DirectX::XMFLOAT3 outPosition;
            if (Collision::IntersectCylinderVsCylinder(
                enemy1->GetPosition(),
                enemy1->GetRadius(),
                enemy1->GetHeight(),
                enemy2->GetPosition(),
                enemy2->GetRadius(),
                enemy2->GetHeight(),
                outPosition))
            {
                enemy2->SetPosition(outPosition);
            }
        }
    }
}