#include "Application.h"
#include "UI/Window.h"
#include "Graph/Graph.h"
#include "Timer.h"

namespace cru {
	constexpr int invoke_later_message_id = WM_USER + 2000;

	Application* Application::theInstance = nullptr;

	Application * Application::GetInstance() {
		return theInstance;
	}

	Application::Application(HINSTANCE h_instance)
		: h_instance_(h_instance) {

		if (theInstance)
			throw std::runtime_error("A application instance already exists.");

		theInstance = this;

		window_manager_ = std::make_unique<ui::WindowManager>();
		graph_manager_ = std::make_unique<graph::GraphManager>();
		timer_manager_ = std::make_unique<TimerManager>();
	}

	Application::~Application() {

	}

	int Application::Run()
	{
		MSG msg;

		while (GetMessage(&msg, nullptr, 0, 0))
		{
			bool handled = false;

			if (msg.hwnd == nullptr)
				switch (msg.message)
				{
				case invoke_later_message_id:
				{
					auto p_action = reinterpret_cast<InvokeLaterAction*>(msg.wParam);
					(*p_action)();
					delete p_action;
					handled = true;
					break;
				}
				case WM_TIMER:
				{
					auto action = timer_manager_->GetAction(static_cast<UINT_PTR>(msg.wParam));
					if (action.has_value())
					{
						action.value()();
						handled = true;
					}
				}
				}

			if (!handled)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		return static_cast<int>(msg.wParam);
	}

	void Application::Quit(int quit_code) {
		::PostQuitMessage(quit_code);
	}

	ui::WindowManager * Application::GetWindowManager() {
		return window_manager_.get();
	}

	graph::GraphManager * Application::GetGraphManager() {
		return graph_manager_.get();
	}

	TimerManager * Application::GetTimerManager()
	{
		return timer_manager_.get();
	}

	HINSTANCE Application::GetInstanceHandle() {
		return h_instance_;
	}

	void InvokeLater(const InvokeLaterAction& action) {
		//copy the action to a safe place
		auto p_action_copy = new InvokeLaterAction(action);

		PostMessage(
			nullptr,
			invoke_later_message_id,
			reinterpret_cast<WPARAM>(p_action_copy),
			0
		);
	}
}
