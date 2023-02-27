#pragma once
#include <cstdint>

namespace Sys
{
	class Window;
}

class App
{
public:
	App(uint32_t _Width, uint32_t _Height);
	~App();

	// Public Method
	void Run();

private:
	// Private Variable
	uint32_t width, height;
	Sys::Window* window;

	// Private Method
	bool Init();
	void MainLoop();
	void Term();
};