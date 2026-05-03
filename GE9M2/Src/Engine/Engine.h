#pragma once
#include <d3d12.h>
#include "../Game/Game.h"
#include "Scene/Scene.h"
#include "Graphics/RenderContext.h"
#include "Graphics/Model/ModelLoader.h"

class Engine {
private:
	const int       bufferCount = 2;
	int             _width = 0;
	int             _height = 0;
	float			_time = 0;
	HWND            _hwnd = nullptr;
	Scene           _scene;
	Game*			_game;
	RenderContext   _renderContext;
	ModelLoader		_loader;

	bool _shouldQuit = false;
	bool _showCollisionBoxes = true;

public:

	Engine(HWND hwnd, int width, int height, Game* game);

	~Engine();

	void beginFrame();

	void update(float dt);

	void endFrame();

	void flush();

	ID3D12GraphicsCommandList4* cmd();

	DX12Upload& uploader();

	RenderContext& renderContext();

	MeshLibrary& meshLib();

	ModelLoader& loader();

	Scene& scene();

	float time() const { return _time; }

	void quit() { _shouldQuit = true; }
	bool shouldQuit() const { return _shouldQuit; }

	void toggleShowCollider() {
		_showCollisionBoxes = !_showCollisionBoxes;
	}
	bool getShowCollisionBoxes() { return _showCollisionBoxes; }
};