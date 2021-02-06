#include "MainScene.h"

#include <CherrySoda/CherrySoda.h>

#include "Program.h"
#include "Entities/Player.h"
#include "Graphics/Skybox.h"
#include "Voxel/Chunk.h"
#include "Voxel/World.h"

using namespace cherrysoda;

class SkyboxRenderer : public SingleTagRenderer
{
public:
	typedef SingleTagRenderer base;

	SkyboxRenderer(const BitTag& tag)
	: base(tag)
	{
		SetEffect(GameApp::ms_skyboxShader);
	}

	void BeforeRender(Scene* scene) override
	{
		Graphics::UseRenderPass(RenderPass())->SetViewport(0, 0, Engine::Instance()->GetWidth(), Engine::Instance()->GetHeight());
	}
};

class VoxelRenderer : public SingleTagRenderer
{
public:
	typedef SingleTagRenderer base;

	VoxelRenderer(const BitTag& tag)
	: base(tag)
	{
		SetEffect(GameApp::ms_voxelShader);
	}

	void BeforeRender(Scene* scene) override
	{
		Graphics::UseRenderPass(RenderPass())->SetViewport(0, 0, Engine::Instance()->GetWidth(), Engine::Instance()->GetHeight());
	}
};

void MainScene::Begin()
{
	base::Begin();

	m_uniformMtx = Graphics::CreateUniformMat4("u_mtx");
	
	Graphics::SetRenderPassOrder({ 0, 1, 2 });
	Graphics::UseRenderPass(1)->SetClearDiscard();
	Graphics::UseRenderPass(2)->SetClearDiscard();

	m_skyboxRenderer = new SkyboxRenderer(ms_skyboxTag);
	m_skyboxRenderer->RenderPass(1);
	m_skyboxRenderer->GetCamera()->SetSize(Vec2_One);
	m_skyboxRenderer->GetCamera()->Position(Math::Vec3(0.5f, 0.5f, 0.5f));
	m_skyboxRenderer->GetCamera()->FOV(90.f);

	m_voxelRenderer = new VoxelRenderer(ms_voxelTag);
	m_voxelRenderer->RenderPass(2);

	m_skybox = new Entity;
	m_skybox->Add(new Skybox);
	m_skybox->Tag(ms_skyboxTag);

	m_player = new Player(m_voxelRenderer->GetCamera());
	m_player->Position(Math::Vec3(0.f, 70.f, 0.f));

	Math::Vec3 worldBasePosition(-World::Size() * Chunk::Size() / 2.0f);
	m_voxelWorld = new World(worldBasePosition);
	m_voxelWorld->LoadChunks();
	for (auto& chunk : *m_voxelWorld) {
		for (int z = 0; z < Chunk::Size(); ++z) {
			for (int y = 0; y < Chunk::Size(); ++y) {
				for (int x = 0; x < Chunk::Size(); ++x) {
					Math::IVec3 blockIndex(x, y, z);
					if ((x + y + z) % 2 == 0) {
						chunk.SetBlockType(blockIndex, Block::Type::White);
					}
					else {
						chunk.SetBlockType(blockIndex, Block::Type::Black);
					}
				}
			}
		}
		chunk.Tag(ms_voxelTag);
		Add(&chunk);
	}

	// Graphics::SetUniformMaterial(Math::Vec3(0.95f, 0.93f, 0.88f), 1.f, 0.f, 0.f); // Silver
	Graphics::SetUniformMaterial(Math::Vec3(0.08f), 0.3f, 0.1f, 0.f); // Glass
	// Graphics::SetUniformMaterial(Math::Vec3(1.f, 0.72f, 0.29f), 1.0f, 0.f, 0.f); // Gold

	Graphics::SetUniformLight(0, Math::Vec3(  0.f, 68.f,   0.f), Vec3_One * 5.f, false);
	Graphics::SetUniformLight(1, Math::Vec3( 30.f, 68.f,   0.f), Vec3_XUp * 5.f, false);
	Graphics::SetUniformLight(2, Math::Vec3(-15.f, 68.f, -26.f), Vec3_YUp * 5.f, false);
	Graphics::SetUniformLight(3, Math::Vec3(-15.f, 68.f,  26.f), Vec3_ZUp * 5.f, false);
	Graphics::SubmitUniformLight();

	// Renderers
	Add(m_skyboxRenderer);
	Add(m_voxelRenderer);

	// Entities
	Add(m_skybox);
	Add(m_player);
}

void MainScene::BeforeRender()
{
	m_voxelRenderer->GetCamera()->SetSize(Engine::Instance()->GetViewSize());

	Graphics::SetUniformCamPos(m_voxelRenderer->GetCamera()->Position());
	const Math::Mat4 orientationMatrix = Math_GetOrientationMatrix(*m_voxelRenderer->GetCamera()->GetViewMatrix());
	Graphics::SetUniform(m_uniformMtx, &orientationMatrix);

	base::BeforeRender();
}

void MainScene::Update()
{
	Camera* camera = m_voxelRenderer->GetCamera();
	float t;
	Math::Vec3 pos = camera->Position();
	Math::Vec3 direction = camera->Direction();
	if (MInput::GamePads(0)->Check(Buttons::LeftShoulder, Buttons::RightShoulder) ||
		MInput::Keyboard()->Check(Keys::R, Keys::F)) {
		if (Math_RaycastAABB(pos, direction, m_voxelWorld->GetAABB(), &t)) {
			for (;;) {
				pos = pos + direction * (t + 0.1f);
				auto index = m_voxelWorld->GetIndexOfBlockAt(pos);
				auto block = m_voxelWorld->GetBlock(index);
				if (MInput::GamePads(0)->Check(Buttons::LeftShoulder)) {
					if (m_voxelWorld->SetBlockType(index, Block::Type::None)) {
						MInput::GamePads(0)->Rumble(0.1f, 0.01f);
					}
				}
				else if (MInput::Keyboard()->Check(Keys::R)) {
					m_voxelWorld->SetBlockType(index, Block::Type::None);
				}
				if (MInput::GamePads(0)->Check(Buttons::RightShoulder)) {
					if (m_voxelWorld->SetBlockType(index, Block::Type::White)) {
						MInput::GamePads(0)->Rumble(0.1f, 0.01f);
					}
				}
				else if (MInput::Keyboard()->Check(Keys::F)) {
					m_voxelWorld->SetBlockType(index, Block::Type::White);
				}
				if (!block || !Math_RaycastAABB(pos, direction, m_voxelWorld->GetBlockAABB(index), nullptr, &t)) {
					break;
				}
			}
		}
	}

	static float albedo[] = { 0.08f, 0.08f, 0.08f };
	static float metallic = 0.3f;
	static float roughness = 0.1f;
	static float ao = 0;
	static float color[] = { 1.0f, 1.0f, 1.0f };
	ImGui::Begin("VoxelExperiment");
	static bool firstFrame = true;
	if (firstFrame) {
		ImGui::SetWindowPos(ImVec2(15, 15));
		ImGui::SetWindowSize(ImVec2(260, 345));
		firstFrame = false;
	}
	ImGui::BeginTabBar("Tab");
	if (ImGui::BeginTabItem("Info")) {
		ImGui::Text("WASD: Movement");
		ImGui::Text("Arrow: Camera direction");
		ImGui::Text("R: Remove voxel");
		ImGui::Text("F: Set white voxel");
		ImGui::Text("X: Move down");
		ImGui::Text("C: Move up");
		ImGui::Text("F11: Fullscreen");
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Voxel Matertial")) {
		ImGui::ColorPicker3("Albedo", albedo);
		ImGui::SliderFloat("Metallic", &metallic, 0, 1.f);
		ImGui::SliderFloat("Roughness", &roughness, 0, 1.f);
		ImGui::SliderFloat("AO", &ao, 0, 1.f);
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Light Color")) {
		ImGui::ColorPicker3("Central", color);
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
	ImGui::End();

	static float s_t = 0.f;
	s_t += Engine::Instance()->DeltaTime();
	Graphics::SetUniformMaterial(Math::Vec3(albedo[0], albedo[1], albedo[2]), metallic, roughness, ao);
	Graphics::SetUniformLight(0, Math::Vec3(0.f, 68.f, 0.f), Math::Vec3(color[0], color[1], color[2]) * 5.f, false);
	Graphics::SetUniformLight(1, Math::Vec3(Math_Cos(s_t) * 30.0f, 68.f, Math_Sin(s_t) * 30.0f), Vec3_XUp * 5.f, false);
	Graphics::SetUniformLight(2, Math::Vec3(Math_Cos(s_t + Math::Pi2 / 3.f) * 30.0f, 68.f, Math_Sin(s_t + Math::Pi2 / 3.f) * 30.0f), Vec3_YUp * 5.f, false);
	Graphics::SetUniformLight(3, Math::Vec3(Math_Cos(s_t - Math::Pi2 / 3.f) * 30.0f, 68.f, Math_Sin(s_t - Math::Pi2 / 3.f) * 30.0f), Vec3_ZUp * 5.f, false);
	Graphics::SubmitUniformLight();

	base::Update();
}

const BitTag MainScene::ms_skyboxTag("skybox");
const BitTag MainScene::ms_voxelTag("voxel");
