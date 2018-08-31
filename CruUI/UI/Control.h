#pragma once

#include "system_headers.h"
#include <vector>
#include <optional>

#include "base.h"
#include "ui_base.h"
#include "layout_base.h"
#include "events/ui_event.h"

namespace cru
{
    namespace ui
    {
        class Control;
        class Window;


        //the position cache
        struct ControlPositionCache
        {
            //The lefttop relative to the ancestor.
            Point lefttop_position_absolute;
        };


        class Control : public Object
        {
            friend class Window;
            friend class WindowLayoutManager;
        protected:
            Control();

        public:
            Control(const Control& other) = delete;
            Control(Control&& other) = delete;
            Control& operator=(const Control& other) = delete;
            Control& operator=(Control&& other) = delete;
            ~Control() override;

        public:

            //*************** region: tree ***************

            //Get parent of control, return nullptr if it has no parent.
            Control* GetParent();

            //Traverse the children
            void ForeachChild(std::function<void(Control*)> predicate);
            void ForeachChild(std::function<FlowControl(Control*)> predicate);

            //Return a vector of all children. This function will create a
            //temporary copy of vector of children. If you just want to
            //traverse all children, just call ForeachChild.
            std::vector<Control*> GetChildren();

            //Add a child at tail.
            void AddChild(Control* control);

            //Add a child before the position.
            void AddChild(Control* control, int position);

            //Remove a child.
            void RemoveChild(Control* child);

            //Remove a child at specified position.
            void RemoveChild(int position);

            //Get the ancestor of the control.
            Control* GetAncestor();

            //Get the window if attached, otherwise, return nullptr.
            Window* GetWindow();

            //Traverse the tree rooted the control.
            void TraverseDescendants(const std::function<void(Control*)>& predicate);

            //*************** region: position and size ***************
            // Position and size part must be isolated from layout part.
            // All the operations in this part must be done independently.
            // And layout part must use api of this part.

            //Get the lefttop relative to its parent.
            virtual Point GetPositionRelative();

            //Set the lefttop relative to its parent.
            virtual void SetPositionRelative(const Point& position);

            //Get the actual size.
            virtual Size GetSize();

            //Set the actual size directly without relayout.
            virtual void SetSize(const Size& size);

            //Get lefttop relative to ancestor. This is only valid when
            //attached to window. Notice that the value is cached.
            //You can invalidate and recalculate it by calling "InvalidatePositionCache". 
            Point GetPositionAbsolute();

            //Local point to absolute point.
            Point LocalToAbsolute(const Point& point);

            //Absolute point to local point.
            Point AbsoluteToLocal(const Point& point);

            bool IsPointInside(const Point& point);


            //*************** region: graphic ***************

            //Draw this control and its child controls.
            void Draw(ID2D1DeviceContext* device_context);

            //*************** region: focus ***************

            bool RequestFocus();

            bool HasFocus();


            //*************** region: layout ***************

            void Measure(const Size& available_size);

            void Layout(const Rect& rect);

            Size GetDesiredSize();

            void SetDesiredSize(const Size& desired_size);

            template<typename TLayoutParams = BasicLayoutParams>
            std::shared_ptr<TLayoutParams> GetLayoutParams()
            {
                static_assert(std::is_base_of_v<BasicLayoutParams, TLayoutParams>, "TLayoutParams must be subclass of BasicLayoutParams.");
                return static_cast<std::shared_ptr<BasicLayoutParams>>(layout_params_);
            }

            template<typename TLayoutParams = BasicLayoutParams,
                typename = std::enable_if_t<std::is_base_of_v<BasicLayoutParams, TLayoutParams>>>
                void SetLayoutParams(std::shared_ptr<TLayoutParams> basic_layout_params)
            {
                static_assert(std::is_base_of_v<BasicLayoutParams, TLayoutParams>, "TLayoutParams must be subclass of BasicLayoutParams.");
                layout_params_ = basic_layout_params;
            }

            //*************** region: events ***************
            //Raised when mouse enter the control.
            events::MouseEvent mouse_enter_event;
            //Raised when mouse is leave the control.
            events::MouseEvent mouse_leave_event;
            //Raised when mouse is move in the control.
            events::MouseEvent mouse_move_event;
            //Raised when a mouse button is pressed in the control.
            events::MouseButtonEvent mouse_down_event;
            //Raised when a mouse button is released in the control.
            events::MouseButtonEvent mouse_up_event;

            events::UiEvent get_focus_event;
            events::UiEvent lose_focus_event;

            events::DrawEvent draw_event;

            events::PositionChangedEvent position_changed_event;
            events::SizeChangedEvent size_changed_event;

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


            // For a event, the window event system will first dispatch event to core functions.
            // Therefore for particular controls, you should do essential actions in core functions,
            // and override version should invoke base version. The base core function
            // in "Control" class will call corresponding non-core function and call "Raise" on
            // event objects. So user custom actions should be done by overriding non-core function
            // and calling the base version is optional.

            //*************** region: position and size event ***************
            virtual void OnPositionChanged(events::PositionChangedEventArgs& args);
            virtual void OnSizeChanged(events::SizeChangedEventArgs& args);

            virtual void OnPositionChangedCore(events::PositionChangedEventArgs& args);
            virtual void OnSizeChangedCore(events::SizeChangedEventArgs& args);


            //*************** region: mouse event ***************
            virtual void OnMouseEnter(events::MouseEventArgs& args);
            virtual void OnMouseLeave(events::MouseEventArgs& args);
            virtual void OnMouseMove(events::MouseEventArgs& args);
            virtual void OnMouseDown(events::MouseButtonEventArgs& args);
            virtual void OnMouseUp(events::MouseButtonEventArgs& args);

            virtual void OnMouseEnterCore(events::MouseEventArgs& args);
            virtual void OnMouseLeaveCore(events::MouseEventArgs& args);
            virtual void OnMouseMoveCore(events::MouseEventArgs& args);
            virtual void OnMouseDownCore(events::MouseButtonEventArgs& args);
            virtual void OnMouseUpCore(events::MouseButtonEventArgs& args);


            //*************** region: focus event ***************
            virtual void OnGetFocus(events::UiEventArgs& args);
            virtual void OnLoseFocus(events::UiEventArgs& args);

            virtual void OnGetFocusCore(events::UiEventArgs& args);
            virtual void OnLoseFocusCore(events::UiEventArgs& args);

            //*************** region: layout ***************
            virtual Size OnMeasure(const Size& available_size);
            virtual void OnLayout(const Rect& rect);

        private:
            Window * window_;

            Control * parent_;
            std::vector<Control*> children_;

            Point position_;
            Size size_;

            ControlPositionCache position_cache_;

            bool is_mouse_inside_;

            std::shared_ptr<BasicLayoutParams> layout_params_;
            Size desired_size_;
        };

        // Find the lowest common ancestor.
        // Return nullptr if "left" and "right" are not in the same tree.
        Control* FindLowestCommonAncestor(Control* left, Control* right);

        // Return the ancestor if one control is the ancestor of the other one, otherwise nullptr.
        Control* IsAncestorOrDescendant(Control* left, Control* right);
    }
}
