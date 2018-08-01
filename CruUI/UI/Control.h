#pragma once
#include "Base.h"
#include "UIBase.h"
#include "Event.h"
#include <vector>

namespace cru
{
    namespace ui
    {
        class Control;
        class Window;

        enum class MeasureMode
        {
            Exactly,
            WrapContent,
            MatchParent
        };

        struct MeasureLength final
        {
            explicit MeasureLength(const double length = 0.0, const MeasureMode mode = MeasureMode::Exactly)
                : length(length), mode(mode)
            {

            }

            double length;
            MeasureMode mode;
        };

        struct MeasureSize final
        {
            MeasureLength width;
            MeasureLength height;
        };

        struct BasicLayoutParams
        {
            BasicLayoutParams() = default;
            BasicLayoutParams(const BasicLayoutParams&) = default;
            BasicLayoutParams(BasicLayoutParams&&) = default;
            BasicLayoutParams& operator = (const BasicLayoutParams&) = default;
            BasicLayoutParams& operator = (BasicLayoutParams&&) = default;
            virtual ~BasicLayoutParams() = default;

            MeasureSize size;
        };

        enum class MouseButton
        {
            Left,
            Right,
            Middle
        };

        class MouseEventArgs : public UiEventArgs
        {
        public:
            // Mouse event without position.
            MouseEventArgs(Object* sender, Object* origin_sender);
            MouseEventArgs(Object* sender, Object* origin_sender, const Point& point);
            MouseEventArgs(const MouseEventArgs& other) = default;
            MouseEventArgs(MouseEventArgs&& other) = default;
            MouseEventArgs& operator=(const MouseEventArgs& other) = default;
            MouseEventArgs& operator=(MouseEventArgs&& other) = default;
            ~MouseEventArgs() override =default;

            Point GetPoint(Control* control);

        private:
            bool has_point_;
            Point point_;
        };

        class MouseButtonEventArgs : public MouseEventArgs
        {
        public:
            MouseButtonEventArgs(Object* sender, Object* origin_sender, const Point& point, MouseButton button);
            MouseButtonEventArgs(const MouseButtonEventArgs& other) = default;
            MouseButtonEventArgs(MouseButtonEventArgs&& other) = default;
            MouseButtonEventArgs& operator=(const MouseButtonEventArgs& other) = default;
            MouseButtonEventArgs& operator=(MouseButtonEventArgs&& other) = default;
            ~MouseButtonEventArgs() override;

            MouseButton GetMouseButton();

        private:
            MouseButton button_;
        };

        class DrawEventArgs : public UiEventArgs
        {
        public:
            DrawEventArgs(Object* sender, Object* original_sender, ID2D1DeviceContext* device_context);
            DrawEventArgs(const DrawEventArgs& other) = default;
            DrawEventArgs(DrawEventArgs&& other) = default;
            DrawEventArgs& operator=(const DrawEventArgs& other) = default;
            DrawEventArgs& operator=(DrawEventArgs&& other) = default;
            ~DrawEventArgs();

            ID2D1DeviceContext* GetDeviceContext();

        private:
            ID2D1DeviceContext * device_context_;
        };

        class PositionChangedEventArgs : public UiEventArgs
        {
        public:
            PositionChangedEventArgs(Object* sender, Object* original_sender, const Point& old_position, const Point& new_position);
            PositionChangedEventArgs(const PositionChangedEventArgs& other) = default;
            PositionChangedEventArgs(PositionChangedEventArgs&& other) = default;
            PositionChangedEventArgs& operator=(const PositionChangedEventArgs& other) = default;
            PositionChangedEventArgs& operator=(PositionChangedEventArgs&& other) = default;
            ~PositionChangedEventArgs() override;

            Point GetOldPosition();
            Point GetNewPosition();

        private:
            Point old_position_;
            Point new_position_;
        };

        class SizeChangedEventArgs : public UiEventArgs
        {
        public:
            SizeChangedEventArgs(Object* sender, Object* original_sender, const Size& old_size, const Size& new_size);
            SizeChangedEventArgs(const SizeChangedEventArgs& other) = default;
            SizeChangedEventArgs(SizeChangedEventArgs&& other) = default;
            SizeChangedEventArgs& operator=(const SizeChangedEventArgs& other) = default;
            SizeChangedEventArgs& operator=(SizeChangedEventArgs&& other) = default;
            ~SizeChangedEventArgs() override;

            Size GetOldSize();
            Size GetNewSize();

        private:
            Size old_size_;
            Size new_size_;
        };

        using UiEvent = Event<UiEventArgs>;
        using MouseEvent = Event<MouseEventArgs>;
        using MouseButtonEvent = Event<MouseButtonEventArgs>;
        using DrawEvent = Event<DrawEventArgs>;
        using PositionChangedEvent = Event<PositionChangedEventArgs>;
        using SizeChangedEvent = Event<SizeChangedEventArgs>;

        //the position cache
        struct ControlPositionCache
        {
            //The lefttop relative to the ancestor.
            Point lefttop_position_absolute;
        };

        class Control abstract : public Object
        {
            friend class Window;
            friend class WindowLayoutManager;
        public:
            Control();
            ~Control() override;
            CRU_NO_COPY_MOVE(Control)

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

            Size Measure(const MeasureSize& size);

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
            MouseEvent mouse_enter_event;
            //Raised when mouse is leave the control.
            MouseEvent mouse_leave_event;
            //Raised when mouse is move in the control.
            MouseEvent mouse_move_event;
            //Raised when a mouse button is pressed in the control.
            MouseButtonEvent mouse_down_event;
            //Raised when a mouse button is released in the control.
            MouseButtonEvent mouse_up_event;

            UiEvent get_focus_event;
            UiEvent lose_focus_event;

            DrawEvent draw_event;

            PositionChangedEvent position_changed_event;
            SizeChangedEvent size_changed_event;

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
            virtual void OnPositionChanged(PositionChangedEventArgs& args);
            virtual void OnSizeChanged(SizeChangedEventArgs& args);

            virtual void OnPositionChangedCore(PositionChangedEventArgs& args);
            virtual void OnSizeChangedCore(SizeChangedEventArgs& args);


            //*************** region: mouse event ***************
            virtual void OnMouseEnter(MouseEventArgs& args);
            virtual void OnMouseLeave(MouseEventArgs& args);
            virtual void OnMouseMove(MouseEventArgs& args);
            virtual void OnMouseDown(MouseButtonEventArgs& args);
            virtual void OnMouseUp(MouseButtonEventArgs& args);

            virtual void OnMouseEnterCore(MouseEventArgs& args);
            virtual void OnMouseLeaveCore(MouseEventArgs& args);
            virtual void OnMouseMoveCore(MouseEventArgs& args);
            virtual void OnMouseDownCore(MouseButtonEventArgs& args);
            virtual void OnMouseUpCore(MouseButtonEventArgs& args);


            //*************** region: focus event ***************
            virtual void OnGetFocus(UiEventArgs& args);
            virtual void OnLoseFocus(UiEventArgs& args);

            virtual void OnGetFocusCore(UiEventArgs& args);
            virtual void OnLoseFocusCore(UiEventArgs& args);

            //*************** region: layout ***************
            virtual Size OnMeasure(const MeasureSize& size);
            virtual void OnLayout(const Rect& rect);

        private:
            Window * window_ = nullptr;

            Control * parent_ = nullptr;
            std::vector<Control*> children_{};

            Point position_;
            Size size_;

            ControlPositionCache position_cache_{};

            bool is_mouse_inside_ = false;

            std::shared_ptr<BasicLayoutParams> layout_params_;
            Size desired_size_;
        };

        // Find the lowest common ancestor.
        // Return nullptr if "left" and "right" are not in the same tree.
        Control* FindLowestCommonAncestor(Control* left, Control* right);

        // Return the ancestor if one control is the ancestor of the other one, otherwise nullptr.
        Control* HaveChildParentRelationship(Control* left, Control* right);
    }
}
