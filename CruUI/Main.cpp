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
    auto application = std::make_shared<Application>(hInstance);
    auto window = std::make_shared<Window>();

    return application->Run();
}
