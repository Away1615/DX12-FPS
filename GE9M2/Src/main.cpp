#include "Engine/Engine.h"
#include "Engine/Foundation/Base/Timer.h"
#include "Engine/Foundation/Window/Window.h"
#include "Game/FPSGame.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
#if defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
			debugController->EnableDebugLayer();
		}
	}
#endif

	Window window;
	window.create(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, "GE9M2", true);

	FPSGame game;

	Engine engine(window.hwnd, WINDOW_WIDTH, WINDOW_HEIGHT, &game);

	Timer timer;

	while (!engine.shouldQuit()) {
		window.processMessages();
		engine.update(timer.dt());
	};
	return 0;
};