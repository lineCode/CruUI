#include "Application.h"
#include "UI/Window.h"

#include <memory>

using cru::Application;
using cru::ui::Window;



int APIENTRY wWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPWSTR    lpCmdLine,
    int       nCmdShow) {

	Application application(hInstance);
    Window window;
    window.Show();

    return application.Run();
}
