#pragma once
#include "Base.h"
#include "UIBase.h"
#include "Event.h"
#include <vector>

namespace cru {
    namespace ui {
        class Window;

        enum class MouseButton {
            Left,
            Right,
            Middle
        };

        class MouseEventArgs : public BasicEventArgs {
        public:
            MouseEventArgs(Object* sender, const Point& point);
            ~MouseEventArgs() override;

            Point GetPoint();

        private:
            Point point_;
        };

        class MouseButtonEventArgs : public MouseEventArgs {
        public:
            MouseButtonEventArgs(Object* sender, const Point& point, MouseButton button);
            ~MouseButtonEventArgs() override;

            MouseButton GetMouseButton();

        private:
            MouseButton button_;
        };

        using MouseEvent = Event<MouseEventArgs>;
        using MouseButtonEvent = Event<MouseButtonEventArgs>;

        class Control abstract : public Object {
        public:
            Control();
            ~Control() override;
            NO_COPY_MOVE(Control)

        public:
            //Get parent of control, return nullptr if it has no parent.
            Control* GetParent();

            //Return all children.
            std::vector<Control*> GetChildren();

            //Add a child at tail.
            void AddChild(Control* control);

            //Add a child before the position.
            void AddChild(Control* control, int position);

            //Remove a child.
            void RemoveChild(Control* child);

            //Remove a child at specified position.
            void RemoveChild(int position);

            //Get the size.
            //Alias for GetRectRelativeToParent().GetSize() .
            Size GetSize();

            //Get the rect relative to its parent.
            virtual Rect GetRectRelativeToParent() = 0;

            //Set the rect relative to its parent.
            virtual void SetRectRelativeToParent(const Rect& rect) = 0;

            //Test whether a point is inside the control in local coordinate.
            virtual bool IsPointInside(const Point& point) = 0;

            //Draw this control and its child controls.
            void Draw(ID2D1DeviceContext* device_context);

        //Events
        public:
            //Raised when mouse enter the control.
            MouseEvent mouseEnterEvent;
            //Raised when mouse is leave the control.
            MouseEvent mouseLeaveEvent;
            //Raised when mouse is move in the control.
            MouseEvent mouseMoveEvent;
            //Raised when a mouse button is pressed in the control.
            MouseButtonEvent mouseDownEvent;
            //Raised when a mouse button is released in the control.
            MouseButtonEvent mouseUpEvent;

        protected:
            //Invoked when a child is added. Overrides should invoke base.
            virtual void OnAddChild(Control* child);
            //Invoked when a child is removed. Overrides should invoke base.
            virtual void OnRemoveChild(Control* child);

            //Invoked when the control is attached to a window. Overrides should invoke base.
            virtual void OnAttachToWindow(Window* window);
            //Invoked when the control is detached to a window. Overrides should invoke base.
            virtual void OnDetachToWindow(Window* window);

            virtual void OnDraw(ID2D1DeviceContext* device_context);

            virtual void OnMouseEnter(const Point& point);
            virtual void OnMouseLeave();
            virtual void OnMouseMove(const Point& point);
            virtual void OnMouseDown(const Point& point, MouseButton button);
            virtual void OnMouseUp(const Point& point, MouseButton button);

        private:
            Control* parent_ = nullptr;
            std::vector<Control*> children_{};

            bool isMouseInside_ = false;
        };

        //Get the ancestor of the control.
        Control* GetAncestor(Control* control);

        //Traverse the tree rooted the control.
        void TraverseDescendants(Control* control, const std::function<void(Control*)>& predicate);

        //Get the rect relative to the ancestor of the control.
        Rect GetRectAbsolute(Control* control);

        //Local point to absolute point.
        Point LocalToAbsolute(Control* control, const Point& point);

        //Absolute point to local point.
        Point AbsoluteToLocal(Control* control, const Point& point);
    }
}
