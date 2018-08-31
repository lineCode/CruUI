#include "Window.h"
#include "Application.h"
#include "Graph/Graph.h"
#include "Exception.h"

namespace cru
{
	namespace ui
	{
		WindowClass::WindowClass(const std::wstring& name, WNDPROC window_proc, HINSTANCE hinstance)
			: name_(name)
		{
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

		WindowClass::~WindowClass()
		{

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

		void WindowManager::RegisterWindow(HWND hwnd, Window * window) {
			const auto find_result = window_map_.find(hwnd);
			if (find_result != window_map_.end())
				throw std::runtime_error("The hwnd is already in the map.");

			window_map_.emplace(hwnd, window);
		}

		void WindowManager::UnregisterWindow(HWND hwnd) {
			const auto find_result = window_map_.find(hwnd);
			if (find_result == window_map_.end())
				throw std::runtime_error("The hwnd is not in the map.");
			window_map_.erase(find_result);

			if (window_map_.empty())
				Application::GetInstance()->Quit(0);
		}

		Window* WindowManager::FromHandle(HWND hwnd) {
		    const auto find_result = window_map_.find(hwnd);
			if (find_result == window_map_.end())
				return nullptr;
			else
				return find_result->second;
		}

		WindowLayoutManager::WindowLayoutManager()
		{
		}

		WindowLayoutManager::~WindowLayoutManager()
		{
		}

		void WindowLayoutManager::InvalidateControlPositionCache(Control * control)
		{
			if (cache_invalid_controls_.count(control) == 1)
				return;

			// find descendant then erase it; find ancestor then just return.
			for (auto i = cache_invalid_controls_.cbegin(); i != cache_invalid_controls_.cend(); ++i)
			{
				if (IsAncestorOrDescendant(*i, control) == control)
					cache_invalid_controls_.erase(i);
				else
					return; // find a ancestor of "control", just return
			}

			cache_invalid_controls_.insert(control);

			if (cache_invalid_controls_.size() == 1) // when insert just now and not repeat to "InvokeLater".
			{
				InvokeLater([this] {
					RefreshInvalidControlPositionCache();
				});
			}
		}

		void WindowLayoutManager::RefreshInvalidControlPositionCache()
		{
			for (auto i : cache_invalid_controls_)
				RefreshControlPositionCache(i);
			cache_invalid_controls_.clear();
		}

		void WindowLayoutManager::RefreshControlPositionCache(Control * control)
		{
			Point point = Point::zero;
			auto parent = control;
			while ((parent = parent->GetParent())) {
				const auto p = parent->GetPositionRelative();
				point.x += p.x;
				point.y += p.y;
			}
			RefreshControlPositionCacheInternal(control, point);
		}

		void WindowLayoutManager::RefreshControlPositionCacheInternal(Control * control, const Point & parent_lefttop_absolute)
		{
			const auto position = control->GetPositionRelative();
			Point lefttop(
				parent_lefttop_absolute.x + position.x,
				parent_lefttop_absolute.y + position.x
			);
			control->position_cache_.lefttop_position_absolute = lefttop;
			control->ForeachChild([lefttop](Control* c) {
				RefreshControlPositionCacheInternal(c, lefttop);
			});
		}

		Window::Window() : layout_manager_(new WindowLayoutManager()), control_list_({ this }) {
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

		WindowLayoutManager* Window::GetLayoutManager()
		{
			return layout_manager_.get();
		}

		HWND Window::GetWindowHandle()
		{
			return hwnd_;
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

			const auto pixel_rect = GetClientRectPixel();
			return Size(
				graph::PixelToDipX(pixel_rect.right),
				graph::PixelToDipY(pixel_rect.bottom)
			);
		}

		void Window::SetClientSize(const Size & size) {
			if (IsWindowValid()) {
				const auto window_style = static_cast<DWORD>(GetWindowLongPtr(hwnd_, GWL_STYLE));
				const auto window_ex_style = static_cast<DWORD>(GetWindowLongPtr(hwnd_, GWL_EXSTYLE));

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
			case WM_SETFOCUS:
				OnSetFocusInternal();
				result = 0;
				return true;
			case WM_KILLFOCUS:
				OnKillFocusInternal();
				result = 0;
				return true;
			case WM_LBUTTONDOWN:
			{
				POINT point;
				point.x = GET_X_LPARAM(l_param);
				point.y = GET_Y_LPARAM(l_param);
				OnMouseDownInternal(MouseButton::Left, point);
				result = 0;
				return true;
			}
			case WM_LBUTTONUP:
			{
				POINT point;
				point.x = GET_X_LPARAM(l_param);
				point.y = GET_Y_LPARAM(l_param);
				OnMouseDownInternal(MouseButton::Left, point);
				result = 0;
				return true;
			}
			case WM_RBUTTONDOWN:
			{
				POINT point;
				point.x = GET_X_LPARAM(l_param);
				point.y = GET_Y_LPARAM(l_param);
				OnMouseDownInternal(MouseButton::Right, point);
				result = 0;
				return true;
			}
			case WM_RBUTTONUP:
			{
				POINT point;
				point.x = GET_X_LPARAM(l_param);
				point.y = GET_Y_LPARAM(l_param);
				OnMouseDownInternal(MouseButton::Right, point);
				result = 0;
				return true;
			}
			case WM_MBUTTONDOWN:
			{
				POINT point;
				point.x = GET_X_LPARAM(l_param);
				point.y = GET_Y_LPARAM(l_param);
				OnMouseDownInternal(MouseButton::Middle, point);
				result = 0;
				return true;
			}
			case WM_MBUTTONUP:
			{
				POINT point;
				point.x = GET_X_LPARAM(l_param);
				point.y = GET_Y_LPARAM(l_param);
				OnMouseDownInternal(MouseButton::Middle, point);
				result = 0;
				return true;
			}
			case WM_SIZE:
				OnResizeInternal(LOWORD(l_param), HIWORD(l_param));
				result = 0;
				return true;
			case WM_DESTROY:
				OnDestroyInternal();
				result = 0;
				return true;
			default: 
                return false;
			}
		}

		Point Window::GetPositionRelative()
		{
			return Point();
		}

		void Window::SetPositionRelative(const Point & position)
		{

		}

		Size Window::GetSize()
		{
			return GetClientSize();
		}

		void Window::SetSize(const Size & size)
		{
			SetClientSize(size);
		}

		void Window::RefreshControlList() {
			control_list_.clear();
			TraverseDescendants([this](Control* control) {
				this->control_list_.push_back(control);
			});
		}

		Control * Window::HitTest(const Point & point)
		{
			for (auto i = control_list_.crbegin(); i != control_list_.crend(); ++i) {
				auto control = *i;
				if (control->IsPointInside(control->AbsoluteToLocal(point))) {
					return control;
				}
			}
			return nullptr;
		}

		bool Window::RequestFocusFor(Control * control)
		{
			if (control == nullptr)
				throw std::invalid_argument("The control to request focus can't be null. You can set it as the window.");

			if (!IsWindowValid())
				return false;

			if (!window_focus_)
			{
				::SetFocus(hwnd_);
				focus_control_ = control;
				return true; // event dispatch will be done in window message handling function "OnSetFocusInternal".
			}

			if (focus_control_ == control)
				return true;

			DispatchEvent(focus_control_, &Control::OnLoseFocusCore, nullptr);

			focus_control_ = control;

			DispatchEvent(control, &Control::OnGetFocusCore, nullptr);

			return true;
		}

		Control* Window::GetFocusControl()
		{
			return focus_control_;
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

			Draw(device_context.Get());

			ThrowIfFailed(
				device_context->EndDraw(), "Failed to draw window."
			);

			render_target_->Present();

			ValidateRect(hwnd_, nullptr);
		}

		void Window::OnResizeInternal(int new_width, int new_height) {
			render_target_->ResizeBuffer(new_width, new_height);
		}

		void Window::OnSetFocusInternal()
		{
			window_focus_ = true;
			if (focus_control_ != nullptr)
				DispatchEvent(focus_control_, &Control::OnGetFocusCore, nullptr);
		}

		void Window::OnKillFocusInternal()
		{
			window_focus_ = false;
			if (focus_control_ != nullptr)
				DispatchEvent(focus_control_, &Control::OnLoseFocusCore, nullptr);
		}

		void Window::OnMouseMoveInternal(POINT point)
		{
			Point dip_point(
				graph::PixelToDipX(point.x),
				graph::PixelToDipY(point.y)
			);

			//when mouse was previous outside the window
			if (mouse_hover_control_ == nullptr) {
				//invoke TrackMouseEvent to have WM_MOUSELEAVE sent.
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof tme;
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = hwnd_;

				TrackMouseEvent(&tme);
			}

			//Find the first control that hit test succeed.
		    const auto new_control_mouse_hover = HitTest(dip_point);

			if (new_control_mouse_hover != mouse_hover_control_) //if the mouse-hover-on control changed
			{
				const auto lowest_common_ancestor = FindLowestCommonAncestor(mouse_hover_control_, new_control_mouse_hover);
				if (mouse_hover_control_ != nullptr) // if last mouse-hover-on control exists
				{
					// dispatch mouse leave event.
					DispatchEvent(mouse_hover_control_, &Control::OnMouseLeaveCore, lowest_common_ancestor);
				}
				mouse_hover_control_ = new_control_mouse_hover;
				// dispatch mouse enter event.
				DispatchEvent(new_control_mouse_hover, &Control::OnMouseEnterCore, lowest_common_ancestor, dip_point);
			}

			DispatchEvent(new_control_mouse_hover, &Control::OnMouseMoveCore, nullptr, dip_point);
		}

		void Window::OnMouseLeaveInternal()
		{
			DispatchEvent(mouse_hover_control_, &Control::OnMouseLeaveCore, nullptr);
			mouse_hover_control_ = nullptr;
		}

		void Window::OnMouseDownInternal(MouseButton button, POINT point)
		{
			Point dip_point(
				graph::PixelToDipX(point.x),
				graph::PixelToDipY(point.y)
			);

			const auto control = HitTest(dip_point);

			DispatchEvent(control, &Control::OnMouseDownCore, nullptr, dip_point, button);
		}

		void Window::OnMouseUpInternal(MouseButton button, POINT point)
		{
			Point dip_point(
				graph::PixelToDipX(point.x),
				graph::PixelToDipY(point.y)
			);

			const auto control = HitTest(dip_point);

			DispatchEvent(control, &Control::OnMouseUpCore, nullptr, dip_point, button);
		}
	}
}
