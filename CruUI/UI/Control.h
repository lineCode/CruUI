#pragma once
#include "Base.h"
#include "UIBase.h"
#include "Event.h"
#include <vector>
#include <optional>

namespace cru
{
	namespace ui
	{
		class Control;
		class Window;

		struct Thickness
		{
			Thickness() : Thickness(0) { }
			Thickness(float width)
				: left(width), top(width), right(width), bottom(width) { }

			Thickness(float left, float top, float right, float bottom)
				: left(left), top(top), right(right), bottom(bottom) { }

			Thickness(const Thickness&) = default;
			Thickness& operator = (const Thickness&) = default;
			~Thickness() = default;

			float left;
			float top;
			float right;
			float bottom;
		};

		struct MeasureSize
		{
			float width;
			float height;
			bool unrestricted_width;
			bool unrestricted_height;
		};

		enum class MouseButton
		{
			Left,
			Right,
			Middle
		};

		class MouseEventArgs : public UIEventArgs
		{
		public:
			// Mouse event without position.
			MouseEventArgs(Object* sender, Object* origin_sender);
			MouseEventArgs(Object* sender, Object* origin_sender, const Point& point);
			~MouseEventArgs() override;

			Point GetPoint(Control* control);

		private:
			std::optional<Point> point_;
		};

		class MouseButtonEventArgs : public MouseEventArgs
		{
		public:
			MouseButtonEventArgs(Object* sender, Object* origin_sender, const Point& point, MouseButton button);
			~MouseButtonEventArgs() override;

			MouseButton GetMouseButton();

		private:
			MouseButton button_;
		};

		class DrawEventArgs : public UIEventArgs
		{
		public:
			DrawEventArgs(Object* sender, Object* original_sender, ID2D1DeviceContext* device_context);
			~DrawEventArgs();

			ID2D1DeviceContext* GetDeviceContext();

		private:
			ID2D1DeviceContext * device_context_;
		};

		class SizeChangedEventArgs : public UIEventArgs
		{
		public:
			SizeChangedEventArgs(Object* sender, Object* original_sender, const Size& old_size, const Size& new_size);
			~SizeChangedEventArgs();

			Size GetOldSize();
			Size GetNewSize();

		private:
			Size old_size_;
			Size new_size_;
		};

		using UIEvent = Event<UIEventArgs>;
		using MouseEvent = Event<MouseEventArgs>;
		using MouseButtonEvent = Event<MouseButtonEventArgs>;
		using DrawEvent = Event<DrawEventArgs>;

		//the position cache
		struct ControlPositionCache
		{
			//The lefttop relative to the ancestor.
			Point lefttop_position_absolute_;
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
			void foreachChild(std::function<void(Control*)> predicate);
			void foreachChild(std::function<FlowControl(Control*)> predicate);

			//Return a vector of all children. This function will create a
			//temporary copy of vector of children. If you just want to
			//traverse all children, just call foreachChild.
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

			//Get the size.
			virtual Size GetSize();

			//Set the size
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

			//*************** region: layout ***************

			void Measure(const MeasureSize& size);

			void Layout(const Rect& rect);

			void RecalculateLayout();

			// Get the saved desired size.
			MeasureSize GetDesiredSize();

			// Set the saved desired size.
			void SetDesiredSize(const MeasureSize& size);


			//*************** region: padding and margin ***************

			Thickness GetPadding();

			void SetPadding(const Thickness& padding);

			Thickness GetMargin();

			void SetMargin(const Thickness& margin);


			//*************** region: graphic ***************

			//Draw this control and its child controls.
			void Draw(ID2D1DeviceContext* device_context);

			//*************** region: focus ***************

			bool RequestFocus();

			bool HasFocus();


			//*************** region: events ***************
		public:
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

			UIEvent get_focus_event;
			UIEvent lose_focus_event;

			DrawEvent draw_event;

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
			virtual void OnGetFocus(UIEventArgs& args);
			virtual void OnLoseFocus(UIEventArgs& args);

			virtual void OnGetFocusCore(UIEventArgs& args);
			virtual void OnLoseFocusCore(UIEventArgs& args);


			//*************** region: layout event ***************

			// overrides remember to call "Measure" on all children.
			virtual void OnMeasure(const MeasureSize& size) = 0;

			// overrides remember to call "Layout" on all children.
			virtual void OnLayout(const Rect& rect) = 0;


		private:
			Window * window_ = nullptr;

			Control * parent_ = nullptr;
			std::vector<Control*> children_{};

			Point position_;
			Size size_;

			ControlPositionCache position_cache_{};

			bool is_mouse_inside_ = false;


			Thickness padding_;
			Thickness margin_;

			MeasureSize desired_size_;
		};

		// Find the lowest common ancestor.
		// Return nullptr if "left" and "right" are not in the same tree.
		Control* FindLowestCommonAncestor(Control* left, Control* right);

		// Return the ancestor if one control is the ancestor of the other one, otherwise nullptr.
		Control* HaveChildParentRelationship(Control* left, Control* right);
	}
}
