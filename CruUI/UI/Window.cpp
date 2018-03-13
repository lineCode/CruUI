#include "Window.h"
#include "Application.h"
#include "Graph/Graph.h"

namespace cru {
    namespace ui {
        WindowClass::WindowClass(const std::wstring& name, WNDPROC window_proc, HINSTANCE hinstance)
            : name_(name) {
            WNDCLASSEX window_class;
            window_class.cbSize = sizeof(WNDCLASSEX);

            window_class.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
            window_class.lpfnWndProc = window_proc;
            window_class.cbClsExtra = 0;
            window_class.cbWndExtra = 0;
            window_class.hInstance = hinstance;
            window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
            window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
            window_class.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
            window_class.lpszMenuName = NULL;
            window_class.lpszClassName = name.c_str();
            window_class.hIconSm = NULL;

            atom_ = RegisterClassEx(&window_class);
            if (atom_ == 0)
                throw std::runtime_error("Failed to create window class.");
        }

        WindowClass::~WindowClass() {

        }

        const wchar_t * WindowClass::GetName() {
            return name_.c_str();
        }

        ATOM WindowClass::GetAtom() {
            return atom_;
        }

        LRESULT __stdcall GeneralWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
            auto window = Application::GetInstance()->GetWindowManager()->FromHandle(hWnd);

            LRESULT result;
            if (window != nullptr && window->HandleWindowMessage(hWnd, Msg, wParam, lParam, result))
                return result;

            return DefWindowProc(hWnd, Msg, wParam, lParam);
        }

        WindowManager::WindowManager() {
            general_window_class_ = std::make_unique<WindowClass>(
                L"CruUIWindowClass",
                GeneralWndProc,
                Application::GetInstance()->GetInstanceHandle()
            );
        }

        WindowManager::~WindowManager() {
        }

        WindowClass * WindowManager::GetGeneralWindowClass() {
            return general_window_class_.get();
        }

        void WindowManager::RegisterWindow(HWND hwnd, Window * window) {
            auto find_result = window_map_.find(hwnd);
            if (find_result != window_map_.end())
                throw std::runtime_error("The hwnd is already in the map.");

            window_map_.emplace(hwnd, window);
        }

        void WindowManager::UnregisterWindow(HWND hwnd) {
            auto find_result = window_map_.find(hwnd);
            if (find_result == window_map_.end())
                throw std::runtime_error("The hwnd is not in the map.");
            window_map_.erase(find_result);

            if (window_map_.empty())
                Application::GetInstance()->Quit(0);
        }

        Window* WindowManager::FromHandle(HWND hwnd) {
            auto find_result =  window_map_.find(hwnd);
            if (find_result == window_map_.end())
                return nullptr;
            else
                return find_result->second;
        }

        Window::Window() {
            auto app = Application::GetInstance();
            hwnd_ = CreateWindowEx(0,
                app->GetWindowManager()->GetGeneralWindowClass()->GetName(),
                L"", WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                nullptr, nullptr, app->GetInstanceHandle(), nullptr
            );

            if (hwnd_ == nullptr)
                throw std::runtime_error("Failed to create window.");

            app->GetWindowManager()->RegisterWindow(hwnd_, this);

            render_target_ = app->GetGraphManager()->CreateWindowRenderTarget(hwnd_);
        }

        Window::~Window() {
            Close();
        }

        bool Window::IsWindowValid() {
            return hwnd_ != nullptr;
        }

        void Window::Close() {
            if (IsWindowValid())
                DestroyWindow(hwnd_);
        }

        void Window::Repaint() {
            if (IsWindowValid()) {
                InvalidateRect(hwnd_, nullptr, false);
                UpdateWindow(hwnd_);
            }
        }

        void Window::Show() {
            if (IsWindowValid()) {
                ShowWindow(hwnd_, SW_SHOWNORMAL);
            }
        }

        void Window::Hide() {
            if (IsWindowValid()) {
                ShowWindow(hwnd_, SW_HIDE);
            }
        }

        Size Window::GetClientSize() {
            if (!IsWindowValid())
                return Size();

            auto pixel_rect = GetClientRectPixel();
            return Size(
                graph::PixelToDipX(pixel_rect.right),
                graph::PixelToDipY(pixel_rect.bottom)
            );
        }

        void Window::SetClientSize(const Size & size) {
            if (IsWindowValid()) {
                auto window_style = static_cast<DWORD>(GetWindowLongPtr(hwnd_, GWL_STYLE));
                auto window_ex_style = static_cast<DWORD>(GetWindowLongPtr(hwnd_, GWL_EXSTYLE));

                RECT rect;
                rect.left = 0;
                rect.top = 0;
                rect.right = graph::DipToPixelX(size.width);
                rect.bottom = graph::DipToPixelY(size.height);
                AdjustWindowRectEx(&rect, window_style, FALSE, window_ex_style);

                SetWindowPos(
                    hwnd_, nullptr, 0, 0,
                    rect.right - rect.left,
                    rect.bottom - rect.top,
                    SWP_NOZORDER | SWP_NOMOVE
                );
            }
        }

        Rect Window::GetWindowRect() {
            if (!IsWindowValid())
                return Rect();

            RECT rect;
            ::GetWindowRect(hwnd_, &rect);

            return Rect::FromVertices(
                graph::PixelToDipX(rect.left),
                graph::PixelToDipY(rect.top),
                graph::PixelToDipX(rect.right),
                graph::PixelToDipY(rect.bottom)
            );
        }

        void Window::SetWindowRect(const Rect & rect) {
            if (IsWindowValid()) {
                SetWindowPos(
                    hwnd_, nullptr,
                    graph::DipToPixelX(rect.left),
                    graph::DipToPixelY(rect.top),
                    graph::DipToPixelX(rect.GetRight()),
                    graph::DipToPixelY(rect.GetBottom()),
                    SWP_NOZORDER
                );
            }
        }

        bool Window::HandleWindowMessage(HWND hwnd, int msg, WPARAM w_param, LPARAM l_param, LRESULT & result) {
            switch (msg) {
            case WM_PAINT:
                OnPaintInternal();
                result = 0;
                return true;
            case WM_ERASEBKGND:
                result = 1;
                return true;
            case WM_SIZE:
                OnResizeInternal(LOWORD(l_param), HIWORD(l_param));
                result = 0;
                return true;
            case WM_DESTROY:
                OnDestroyInternal();
                result = 0;
                return true;
            }
            return false;
        }

        Rect Window::GetRectRelativeToParent() {
            return Rect(Point(), GetClientSize());
        }

        void Window::SetRectRelativeToParent(const Rect & rect) {
            SetClientSize(rect.GetSize());
        }

        bool Window::IsPointInside(const Point & point) {
            return GetRectRelativeToParent().IsPointInside(point);
        }

        RECT Window::GetClientRectPixel() {
            RECT rect{ };
            GetClientRect(hwnd_, &rect);
            return rect;
        }

        void Window::OnDestroyInternal() {
            Application::GetInstance()->GetWindowManager()->UnregisterWindow(hwnd_);
            hwnd_ = nullptr;
        }

        void Window::OnPaintInternal() {
            render_target_->SetAsTarget();

            auto device_context = render_target_->GetD2DDeviceContext();

            device_context->BeginDraw();

            //Clear the background.
            device_context->Clear(D2D1::ColorF(D2D1::ColorF::White));

            Draw(device_context);

            ThrowIfFailed(
                device_context->EndDraw(), "Failed to draw window."
            );

            render_target_->Present();

            ValidateRect(hwnd_, nullptr);
        }

        void Window::OnResizeInternal(int new_width, int new_height) {
            render_target_->ResizeBuffer(new_width, new_height);
        }
    }
}
