#include <CherrySoda/Renderers/SingleTagRenderer.h>

#include <CherrySoda/Scene.h>
#include <CherrySoda/Graphics/Graphics.h>
#include <CherrySoda/Graphics/SpriteBatch.h>
#include <CherrySoda/InternalUtilities/EntityList.h>
#include <CherrySoda/Util/Draw.h>

using cherrysoda::SingleTagRenderer;

using cherrysoda::Draw;
using cherrysoda::Graphics;
using cherrysoda::Scene;

void SingleTagRenderer::Render(Scene* scene)
{
	Graphics::BeginRenderPass(RenderPass());
	Graphics::SetEffect(GetEffect());
	Graphics::UseCurrentRenderPass()->SetCamera(GetCamera());
	Draw::GetSpriteBatch()->Begin();
	scene->Entities()->RenderOnly(m_tag);
	if (Engine::Instance()->ConsoleOpened())
		scene->Entities()->DebugRender(GetCamera());
	Draw::GetSpriteBatch()->End();
	Graphics::SetEffect(nullptr);
	Graphics::Discard(); // TODO: Move discard to EndRenderPass?
	Graphics::EndRenderPass(RenderPass());
}
