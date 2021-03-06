#include <random>

#include "application.h"
#include "ui/window.h"
#include "timer.h"


using cru::Application;
using cru::ui::Window;



int APIENTRY wWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPWSTR    lpCmdLine,
    int       nCmdShow) {

	Application application(hInstance);
    Window window;

    std::array<D2D_COLOR_F, 4> colors =
    {
        D2D1::ColorF(D2D1::ColorF::Blue),
        D2D1::ColorF(D2D1::ColorF::Yellow),
        D2D1::ColorF(D2D1::ColorF::Green),
        D2D1::ColorF(D2D1::ColorF::Red)
    };

    std::random_device rd;     // only used once to initialise (seed) engine
    std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<unsigned int> uni(0, colors.size() - 1); // guaranteed unbiased


	window.draw_event.AddHandler([&](cru::ui::events::DrawEventArgs& args) {
		auto device_context = args.GetDeviceContext();

		ID2D1SolidColorBrush* brush;
		device_context->CreateSolidColorBrush(colors[uni(rng)], &brush);

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
			task->Cancel(); // test for idempotency.
		});
	});

    window.Show();

    return application.Run();
}
