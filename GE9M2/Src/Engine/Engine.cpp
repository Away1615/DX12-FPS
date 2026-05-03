#include "Engine.h"
#include "../Game/Game.h"

Engine::Engine(HWND hwnd, int width, int height, Game* game)
	:
	_hwnd(hwnd),
	_width(width),
	_height(height),
	_renderContext(hwnd, width, height),
	_loader(_renderContext),
	_scene(this),
	_game(game)
{
	if (_game) {
		_game->onInit(*this, _scene);
	}
}

Engine::~Engine() {}

void Engine::beginFrame() {
	_renderContext.renderer().beginFrame();
}

void Engine::update(float dt) {
	_time += dt;

	beginFrame();
	_scene.update(dt);
	_scene.render(_renderContext);
	endFrame();
}

void Engine::endFrame() {
	_renderContext.renderer().endFrame();
}

void Engine::flush() {
	_renderContext.renderer().flushGraphicsQueue();
}

ID3D12GraphicsCommandList4* Engine::cmd() {
	return _renderContext.renderer().commandList();
}

DX12Upload& Engine::uploader() {
	return _renderContext.uploader();
}

RenderContext& Engine::renderContext() {
	return _renderContext;
}

ModelLoader& Engine::loader() {
	return _loader;
}

Scene& Engine::scene() {
	return _scene;
}
