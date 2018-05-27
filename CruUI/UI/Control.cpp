#include "Control.h"
#include "Window.h"
#include <algorithm>

namespace cru {
	namespace ui {
		MouseEventArgs::MouseEventArgs(Object * sender, Object * origin_sender)
			: UIEventArgs(sender, origin_sender), point_(std::nullopt)
		{

		}

		MouseEventArgs::MouseEventArgs(Object* sender, Object* origin_sender, const Point& point)
			: UIEventArgs(sender, origin_sender), point_(point)
		{

		}

		MouseEventArgs::~MouseEventArgs()
		{
			
		}

		Point MouseEventArgs::GetPoint(Control* control)
		{
			if (point_.has_value())
				return control->AbsoluteToLocal(point_.value());
			else
				return Point();
		}

		MouseButtonEventArgs::MouseButtonEventArgs(
			Object * sender, Object* origin_sender, const Point & point, MouseButton button)
			: MouseEventArgs(sender, origin_sender, point), button_(button)
		{

		}

		MouseButtonEventArgs::~MouseButtonEventArgs()
		{

		}

		MouseButton MouseButtonEventArgs::GetMouseButton()
		{
			return button_;
		}

		Control::Control()
		{

		}

		Control::~Control()
		{

		}

		Control* Control::GetParent()
		{
			return this->parent_;
		}

		void Control::foreachChild(std::function<void(Control*)> predicate)
		{
			for (auto child : children_)
				predicate(child);
		}

		void Control::foreachChild(std::function<FlowControl(Control*)> predicate)
		{
			for (auto child : children_)
			{
				if (predicate(child) == FlowControl::Break)
					break;
			}
		}

		std::vector<Control*> Control::GetChildren()
		{
			return this->children_;
		}

		void AddChildCheck(Control* control)
		{
			if (control->GetParent() != nullptr)
				throw std::invalid_argument("The control already has a parent.");

			if (dynamic_cast<Window*>(control))
				throw std::invalid_argument("Can't add a window as child.");
		}

		void Control::AddChild(Control* control)
		{
			AddChildCheck(control);

			this->children_.push_back(control);

			control->parent_ = this;

			this->OnAddChild(control);
		}

		void Control::AddChild(Control* control, int position)
		{
			AddChildCheck(control);

			if (position < 0 || position > this->children_.size())
				throw std::invalid_argument("The position is out of range.");

			this->children_.insert(this->children_.cbegin() + position, control);

			control->parent_ = this;

			this->OnAddChild(this);
		}

		void Control::RemoveChild(Control* child)
		{
			const auto i = std::find(this->children_.cbegin(), this->children_.cend(), child);
			if (i == this->children_.cend())
				throw std::invalid_argument("The argument child is not a child of this control.");

			this->children_.erase(i);

			child->parent_ = nullptr;

			this->OnRemoveChild(this);
		}

		void Control::RemoveChild(int position)
		{
			if (position < 0 || position >= children_.size())
				throw std::invalid_argument("The position is out of range.");

			const auto p = children_.cbegin() + position;
			const auto child = *p;
			children_.erase(p);

			child->parent_ = nullptr;

			this->OnRemoveChild(child);
		}

		Control* Control::GetAncestor()
		{
			// if attached to window, the window is the ancestor.
			if (window_)
				return window_;

			// otherwise find the ancestor
			auto ancestor = this;
			while (auto parent = ancestor->GetParent())
				ancestor = parent;
			return ancestor;
		}

		Window * Control::GetWindow()
		{
			return window_;
		}

		void TraverseDescendants_(Control* control,
			const std::function<void(Control*)>& predicate)
		{
			predicate(control);
			control->foreachChild([control, predicate](Control* c) {
				TraverseDescendants_(c, predicate);
			});
		}

		void Control::TraverseDescendants(
			const std::function<void(Control*)>& predicate)
		{
			TraverseDescendants_(this, predicate);
		}

		Size Control::GetSize()
		{
			return GetRectRelativeToParent().GetSize();
		}

		Point Control::GetLefttopAbsolute()
		{
			return position_cache_.lefttop_position_absolute_;
		}

		Point Control::LocalToAbsolute(const Point& point)
		{
			return Point(point.x + position_cache_.lefttop_position_absolute_.x,
				point.y + position_cache_.lefttop_position_absolute_.y);
		}

		Point Control::AbsoluteToLocal(const Point & point)
		{
			return Point(point.x - position_cache_.lefttop_position_absolute_.x,
				point.y - position_cache_.lefttop_position_absolute_.y);
		}

		void Control::InvalidatePositionCache()
		{
			Point point;
			auto parent = this;
			while (parent = parent->GetParent()) {
				auto r = parent->GetRectRelativeToParent();
				point.x += r.left;
				point.y += r.top;
			}
			RefreshDescendantPositionCache(point);
		}

		void Control::Draw(ID2D1DeviceContext* device_context)
		{
			D2D1::Matrix3x2F old_transform;
			device_context->GetTransform(&old_transform);

			auto rect = GetRectRelativeToParent();
			device_context->SetTransform(old_transform * D2D1::Matrix3x2F::Translation(rect.left, rect.top));

			OnDraw(device_context);

			for (auto child : GetChildren())
				child->Draw(device_context);

			device_context->SetTransform(old_transform);
		}

		bool Control::RequestFocus()
		{
			auto window = GetWindow();
			if (window == nullptr)
				return false;

			return window->RequestFocusFor(this);
		}

		bool Control::HasFocus()
		{
			auto window = GetWindow();
			if (window = nullptr)
				return false;

			return window->GetFocusControl() == this;
		}

		void Control::OnAddChild(Control* child)
		{
			if (auto window = dynamic_cast<Window*>(GetAncestor()))
			{
				child->TraverseDescendants([window](Control* control) {
					control->OnAttachToWindow(window);
				});
				window->RefreshControlList();
				InvalidatePositionCache();
			}
		}

		void Control::OnRemoveChild(Control* child)
		{
			if (auto window = dynamic_cast<Window*>(GetAncestor()))
			{
				child->TraverseDescendants([window](Control* control) {
					control->OnDetachToWindow(window);
				});
				window->RefreshControlList();
			}
		}

		void Control::OnAttachToWindow(Window* window)
		{
			window_ = window;
		}

		void Control::OnDetachToWindow(Window * window)
		{
			window_ = nullptr;
		}

		void Control::OnDraw(ID2D1DeviceContext * device_context)
		{

		}

		void Control::OnMouseEnter(MouseEventArgs & args)
		{
		}

		void Control::OnMouseLeave(MouseEventArgs & args)
		{
		}

		void Control::OnMouseMove(MouseEventArgs & args)
		{
		}

		void Control::OnMouseDown(MouseButtonEventArgs & args)
		{
		}

		void Control::OnMouseUp(MouseButtonEventArgs & args)
		{
		}

		void Control::OnMouseEnterCore(MouseEventArgs & args)
		{
			OnMouseEnter(args);
			mouse_enter_event.Raise(args);
		}

		void Control::OnMouseLeaveCore(MouseEventArgs & args)
		{
			OnMouseLeave(args);
			mouse_leave_event.Raise(args);
		}

		void Control::OnMouseMoveCore(MouseEventArgs & args)
		{
			OnMouseMove(args);
			mouse_move_event.Raise(args);
		}

		void Control::OnMouseDownCore(MouseButtonEventArgs & args)
		{
			OnMouseDown(args);
			mouse_down_event.Raise(args);
		}

		void Control::OnMouseUpCore(MouseButtonEventArgs & args)
		{
			OnMouseUp(args);
			mouse_up_event.Raise(args);
		}

		void Control::OnGetFocus(UIEventArgs & args)
		{
		}

		void Control::OnLoseFocus(UIEventArgs & args)
		{
		}

		void Control::OnGetFocusCore(UIEventArgs & args)
		{
			OnGetFocus(args);
			get_focus_event.Raise(args);
		}

		void Control::OnLoseFocusCore(UIEventArgs & args)
		{
			OnLoseFocus(args);
			lose_focus_event.Raise(args);
		}

		void Control::RefreshDescendantPositionCache(const Point& parent_lefttop_absolute)
		{
			auto rect = GetRectRelativeToParent();
			Point lefttop(
				parent_lefttop_absolute.x + rect.left,
				parent_lefttop_absolute.y + rect.top
			);
			position_cache_.lefttop_position_absolute_ = lefttop;
			foreachChild([lefttop](Control* c) {
				c->RefreshDescendantPositionCache(lefttop);
			});
		}

		std::list<Control*> GetAncestorList(Control* control)
		{
			std::list<Control*> l;
			while (control != nullptr)
			{
				l.push_front(control);
				control = control->GetParent();
			}
			return std::move(l);
		}

		Control* FindLowestCommonAncestor(Control * left, Control * right)
		{
			if (left == nullptr || right == nullptr)
				return nullptr;

			auto&& left_list = GetAncestorList(left);
			auto&& right_list = GetAncestorList(right);

			// the root is different
			if (left_list.front() != right_list.front())
				return nullptr;

			// find the last same control or the last control (one is the other's ancestor)
			auto left_i = left_list.cbegin();
			auto right_i = right_list.cbegin();
			while (true)
			{
				if (left_i == left_list.cend())
					return *(--left_i);
				if (right_i == right_list.cend())
					return *(--right_i);
				if (*left_i != *right_i)
					return *(--left_i);
				++left_i;
				++right_i;
			}

			return nullptr;
		}
	}
}
