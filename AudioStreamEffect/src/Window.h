#pragma once
#include <functional>

struct GLFWwindow;
using UpdateProc = std::function<void(void* user)>;
class Window
{
private:
	GLFWwindow* m_Window;
	UpdateProc m_UpdateProc;
	void* m_UserData;

public:
	Window(UpdateProc proc, void* User);
	~Window();

	void Run();
};