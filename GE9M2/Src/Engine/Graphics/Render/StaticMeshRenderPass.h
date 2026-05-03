#pragma once

class Scene;
class RenderContext;

class StaticMeshRenderPass {
public:
	void render(Scene& scene, RenderContext& ctx);
};