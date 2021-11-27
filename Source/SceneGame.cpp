#include "Graphics/Graphics.h"
#include "SceneGame.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "EnemySlime.h"
#include "EffectManager.h"

// 初期化
void SceneGame::Initialize()
{
	stage = new Stage();
	player = new Player();
	cameraController = new CameraController();

	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)
	);
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f
	);

	EnemyManager& enemyManager = EnemyManager::Instance();

	for (int i = 0; i < 6; ++i)
	{
		EnemySlime* slime = new EnemySlime();
		slime->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 5));
		enemyManager.Register(slime);
	}

	gauge = new Sprite();
}

// 終了化
void SceneGame::Finalize()
{
	if (gauge != nullptr)
	{
		delete gauge;
		gauge = nullptr;
	}

	if (stage != nullptr)
	{
		delete stage;
		stage = nullptr;
	}

	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}

	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}

	EnemyManager::Instance().Clear();
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);

	stage->Update(elapsedTime);
	player->Update(elapsedTime);
	EnemyManager::Instance().Update(elapsedTime);
	EffectManager::Instance().Update(elapsedTime);

	SpawnEnemy();
}

// 描画処理
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// 描画処理
	RenderContext rc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };	// ライト方向（下方向）

	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.Getprojection();

	// 3Dモデル描画
	{
		Shader* shader = graphics.GetShader();
		shader->Begin(dc, rc);

		stage->Render(dc, shader);
		player->Render(dc, shader);
		EnemyManager::Instance().Render(dc, shader);
		shader->End(dc);
	}

	// 3Dエフェクト描画
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// 3Dデバッグ描画
	{
		player->DrawDebugPrimitive();

		EnemyManager::Instance().DrawDebugPrimitive();

		// ラインレンダラ描画実行
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// デバッグレンダラ描画実行
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

	// 2Dスプライト描画
	{
		RenderEnemyGauge(dc, rc.view, rc.projection);
	}

	// 2DデバッグGUI描画
	{
		ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("Status", nullptr, ImGuiWindowFlags_None))
		{
			player->DrawDebugGUI();
			cameraController->DrawDebugGUI();
			EnemyManager::Instance().DrawDebugGUI();
		}
		ImGui::End();
	}
}

void SceneGame::RenderEnemyGauge(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	D3D11_VIEWPORT viewport;
	UINT numViwewports = 1;
	dc->RSGetViewports(&numViwewports, &viewport);

	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();

	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		DirectX::XMFLOAT3 enemyPos = enemy->GetPosition();
		enemyPos.y += enemy->GetHeight();

		DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project(
			DirectX::XMLoadFloat3(&enemyPos),
			viewport.TopLeftX, viewport.TopLeftY,
			viewport.Width,	viewport.Height,
			0.0f, 1.0f,
			Projection,	View, World
		);

		DirectX::XMFLOAT3 screenPosition;
		DirectX::XMStoreFloat3(&screenPosition, ScreenPosition);

		this->gauge->Render(dc,
			screenPosition.x - 25, screenPosition.y - 10, 50 * (enemy->GetHealth() / enemy->GetMaxHealth()), 5,
			0, 0, 1, 1,
			0,
			1, 0, 0, 1);
	}
}

void SceneGame::SpawnEnemy()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();

	RenderContext rc;
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.Getprojection();


	D3D11_VIEWPORT viewport;
	UINT numViwewports = 1;
	dc->RSGetViewports(&numViwewports, &viewport);

	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&rc.view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&rc.projection);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();

	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		DirectX::XMFLOAT3 enemyPos = enemy->GetPosition();
		enemyPos.y += enemy->GetHeight();

		DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project(
			DirectX::XMLoadFloat3(&enemyPos),
			viewport.TopLeftX, viewport.TopLeftY,
			viewport.Width, viewport.Height,
			0.0f, 1.0f,
			Projection, View, World
		);

		DirectX::XMFLOAT3 screenPosition;
		DirectX::XMStoreFloat3(&screenPosition, ScreenPosition);

		this->gauge->Render(dc,
			screenPosition.x - 25, screenPosition.y - 10, 50 * (enemy->GetHealth() / enemy->GetMaxHealth()), 5,
			0, 0, 1, 1,
			0,
			1, 0, 0, 1);
	}
}
