#pragma once

#include "SystemHeaders.h"
#include "Control.h"

#include <set>
#include <map>
#include <list>
#include <memory>
#include <optional>

namespace cru {
	namespace graph {
		class WindowRenderTarget;
	}

	namespace ui {
		class WindowClass : public Object
		{
		public:
			WindowClass(const std::wstring& name, WNDPROC window_proc, HINSTANCE hinstance);
		    ~WindowClass() override;

            CRU_NO_COPY_MOVE(WindowClass)

			const wchar_t* GetName();
			ATOM GetAtom();

		private:
			std::wstring name_;
			ATOM atom_;
		};

		class WindowManager : public Object
		{
		public:
			WindowManager();
			~WindowManager() override;

            CRU_NO_COPY_MOVE(WindowManager)

			//Get the general window class for creating ordinary window.
			WindowClass* GetGeneralWindowClass();

			//Register a window newly created.
			//This function adds the hwnd to hwnd-window map.
			//It should be called immediately after a window was created.
			void RegisterWindow(HWND hwnd, Window* window);

			//Unregister a window that is going to be destroyed.
			//This function removes the hwnd from the hwnd-window map.
			//It should be called immediately before a window is going to be destroyed,
			void UnregisterWindow(HWND hwnd);

			//Return a pointer to the Window object related to the HWND or nullptr if the hwnd is not in the map.
			Window* FromHandle(HWND hwnd);

		private:
			std::unique_ptr<WindowClass> general_window_class_;
			std::map<HWND, Window*> window_map_;
		};


		class WindowLayoutManager : public Object
		{
		public:
			WindowLayoutManager();
			~WindowLayoutManager() override;

            CRU_NO_COPY_MOVE(WindowLayoutManager)

			//Mark position cache of the control and its descendants invalid,
			//(which is saved as an auto-managed list internal)
			//and send a message to refresh them.
			void InvalidateControlPositionCache(Control* control);

			//Refresh position cache of the control and its descendants whose cache
			//has been marked as invalid.
			void RefreshInvalidControlPositionCache();

			//Refresh position cache of the control and its descendants immediately.
			static void RefreshControlPositionCache(Control* control);

		private:
			static void RefreshControlPositionCacheInternal(Control* control, const Point& parent_lefttop_absolute);

		private:
			std::set<Control*> cache_invalid_controls_;
		};

		class Window : public Control
		{
			friend class WindowManager;
		public:
			Window();
			~Window() override;
			CRU_NO_COPY_MOVE(Window)

		public:
			//*************** region: managers ***************
			WindowLayoutManager* GetLayoutManager();


			//*************** region: handle ***************

			//Get the handle of the window. Return null if window is invalid.
			HWND GetWindowHandle();

			//Return if the window is still valid, that is, hasn't been closed or destroyed.
			bool IsWindowValid();


			//*************** region: window operations ***************

			//Close and destroy the window if the window is valid.
			void Close();

			//Send a repaint message to the window's message queue which may make the window repaint.
			void Repaint();

			//Show the window.
			void Show();

			//Hide thw window.
			void Hide();

			//Get the client size.
			Size GetClientSize();

			//Set the client size and repaint.
			void SetClientSize(const Size& size);

			//Get the rect of the window containing frame.
			//The lefttop of the rect is relative to screen lefttop.
			Rect GetWindowRect();

			//Set the rect of the window containing frame.
			//The lefttop of the rect is relative to screen lefttop.
			void SetWindowRect(const Rect& rect);

			//Handle the raw window message.
			//Return true if the message is handled and get the result through "result" argument.
			//Return false if the message is not handled.
			bool HandleWindowMessage(HWND hwnd, int msg, WPARAM w_param, LPARAM l_param, LRESULT& result);


			//*************** region: position and size ***************

			//Always return (0, 0) for a window.
			Point GetPositionRelative() override final;

			//This method has no effect for a window.
			void SetPositionRelative(const Point& position) override final;

			//Get the size of client area for a window.
			Size GetSize() override final;

			//Set the size of client area for a window.
			void SetSize(const Size& size) override final;


			//*************** region: features ***************

			//Refresh control list.
			//It should be invoked every time a control is added or removed from the tree.
			void RefreshControlList();

			//Get the most top control at "point".
			Control* HitTest(const Point& point);

			
			//*************** region: focus ***************

			//Request focus for specified control.
			bool RequestFocusFor(Control* control);

			//Get the control that has focus.
			Control* GetFocusControl();


		private:
			//*************** region: native operations ***************

			//Get the client rect in pixel.
			RECT GetClientRectPixel();


			//*************** region: native messages ***************

			void OnDestroyInternal();
			void OnPaintInternal();
			void OnResizeInternal(int new_width, int new_height);

			void OnSetFocusInternal();
			void OnKillFocusInternal();

			void OnMouseMoveInternal(POINT point);
			void OnMouseLeaveInternal();
			void OnMouseDownInternal(MouseButton button, POINT point);
			void OnMouseUpInternal(MouseButton button, POINT point);



			//*************** region: event dispatcher helper ***************

			template<typename EventArgs>
			using EventMethod = void (Control::*)(EventArgs&);

			// Dispatch the event.
			// 
			// This will invoke the "event_method" of the control and its parent and parent's
			// parent ... (until "last_reciever" if it's not nullptr) with appropriate args.
			//
			// Args is of type "EventArgs". The first init argument is "sender", which is
			// automatically bound to each recieving control. The second init argument is
			// "original_sender", which is unchanged. And "args" will be perfectly forwarded
			// as the rest arguments.
			template<typename EventArgs, typename... Args>
			void DispatchEvent(Control* original_sender, EventMethod<EventArgs> event_method, Control* last_reciever, Args&&... args)
			{
				auto control = original_sender;
				while (control != nullptr && control != last_reciever)
				{
					EventArgs event_args(control, original_sender, std::forward<Args>(args)...);
					(control->*event_method)(event_args);
					control = control->GetParent();
				}
			}

		private:
			std::unique_ptr<WindowLayoutManager> layout_manager_;

			HWND hwnd_ = nullptr;
			std::shared_ptr<graph::WindowRenderTarget> render_target_{};

			std::list<Control*> control_list_{};

			Control* mouse_hover_control_ = nullptr;

			bool window_focus_ = false;
			Control* focus_control_ = this; // "focus_control_" can't be nullptr
		};
	}
}
