#pragma once

#include "Control.h"

#include <map>
#include <list>
#include <memory>

namespace cru {
    namespace graph {
        class WindowRenderTarget;
    }

    namespace ui {
        class WindowClass : public Object {
        public:
            WindowClass(const std::wstring& name, WNDPROC window_proc, HINSTANCE hinstance);
            ~WindowClass() override;

            const wchar_t* GetName();
            ATOM GetAtom();

        private:
            std::wstring name_;
            ATOM atom_;
        };

        class WindowManager : public Object {
        public:
            WindowManager();
            ~WindowManager() override;

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


        class Window : public Control {
            friend class WindowManager;
        public:
            Window();
            ~Window() override;
            NO_COPY_MOVE(Window)

        public:
            //Get the handle of the window. Return null if window is invalid.
            HWND GetWindowHandle();

            //Return if the window is still valid, that is, hasn't been closed or destroyed.
            bool IsWindowValid();

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

            //Get the rect relative to its parent.
            //For a window, the rect is of the client area and its lefttop is always (0, 0).
            Rect GetRectRelativeToParent() override;

            //Set the rect relative to its parent.
            //For a window, it is to set the client rect. The lefttop of the rect is ignored.
            void SetRectRelativeToParent(const Rect& rect) override;

            //Test whether a point is inside the control in local coordinate.
            bool IsPointInside(const Point& point) override;

            //Refresh control list.
            //It should be invoked every time a control is added or removed from the tree.
            void RefreshControlList();

        private:
            //Get the client rect in pixel.
            RECT GetClientRectPixel();

            void OnDestroyInternal();
            void OnPaintInternal();
            void OnResizeInternal(int new_width, int new_height);

            void OnMouseMoveInternal(POINT point);
            void OnMouseLeaveInternal();

        private:
            HWND hwnd_ = 0;
            std::shared_ptr<graph::WindowRenderTarget> render_target_{};

            std::list<Control*> control_list_;

            Control* mouse_hover_control_ = nullptr;
        };
    }
}
