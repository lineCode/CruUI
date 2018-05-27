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

		using MouseEvent = Event<MouseEventArgs>;
		using MouseButtonEvent = Event<MouseButtonEventArgs>;

		//the position cache
		struct ControlPositionCache
		{
			//The lefttop relative to the ancestor.
			Point lefttop_position_absolute_;
		};

		class Control abstract : public Object
		{
			friend class Window;
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

			//Traverse the tree rooted the control.
			void TraverseDescendants(const std::function<void(Control*)>& predicate);

			//*************** region: location and size ***************

			//Get the rect relative to its parent.
			virtual Rect GetRectRelativeToParent() = 0;

			//Set the rect relative to its parent. Remember to
			//call InvalidPositionCache after change position.
			virtual void SetRectRelativeToParent(const Rect& rect) = 0;

			//Get the size. Alias for GetRectRelativeToParent().GetSize().
			Size GetSize();

			//Get lefttop relative to ancestor. This is only valid when
			//attached to window. Notice that the value is cached.
			//You can invalidate and recalculate it by calling
			//InvalidatePositionCache. 
			Point GetLefttopAbsolute();

			//Local point to absolute point.
			Point LocalToAbsolute(const Point& point);

			//Absolute point to local point.
			Point AbsoluteToLocal(const Point& point);

			//Refresh the position cache of this and all descendents.
			void InvalidatePositionCache();

			//Test whether a point is inside the control in local coordinate.
			virtual bool IsPointInside(const Point& point) = 0;

			//*************** region: graphic ***************

			//Draw this control and its child controls.
			void Draw(ID2D1DeviceContext* device_context);


			//*************** region: events ***************
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

		private:
			// A helper recursive function for refreshing position cache.
			void RefreshDescendantPositionCache(const Point& parent_lefttop_absolute);

		private:
			Control * parent_ = nullptr;
			std::vector<Control*> children_{};

			ControlPositionCache position_cache_{};

			bool isMouseInside_ = false;
		};

		// Find the lowest common ancestor.
		// Retur nullptr if "left" and "right" are not in the same tree.
		Control* FindLowestCommonAncestor(Control* left, Control* right);
	}
}
