#include "Application.h"
#include "UI/Window.h"
#include "Timer.h"

#include <memory>

#include "random.hpp"

using cru::Application;
using cru::ui::Window;
using cru::ui::DrawEventArgs;

using Random = effolkronium::random_static;

D2D1::ColorF GetRandomColor()
{
	return D2D1::ColorF(Random::get(0.0f, 1.0f), Random::get(0.0f, 1.0f), Random::get(0.0f, 1.0f));
}


int APIENTRY wWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPWSTR    lpCmdLine,
    int       nCmdShow) {

	Application application(hInstance);
    Window window;

	window.draw_event.AddHandler([](DrawEventArgs& args) {
		auto device_context = args.GetDeviceContext();

		ID2D1SolidColorBrush* brush;
		device_context->CreateSolidColorBrush(GetRandomColor(), &brush);

		device_context->FillRectangle(D2D1::RectF(100.0f, 100.0f, 300.0f, 200.0f), brush);

		brush->Release();
	});

	cru::SetTimeout(2.0, [&window]() {
		window.Repaint();

		auto task = cru::SetInterval(0.5, [&window]() {
			window.Repaint();
		});

		cru::SetTimeout(4, [task]() {
			task->Cancel();
			task->Cancel(); // test for Idempotency.
		});
	});

    window.Show();

    return application.Run();
}
