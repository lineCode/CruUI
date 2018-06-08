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

		DrawEventArgs::DrawEventArgs(Object * sender, Object * original_sender, ID2D1DeviceContext * device_context)
			: UIEventArgs(sender, original_sender), device_context_(device_context)
		{

		}

		DrawEventArgs::~DrawEventArgs()
		{

		}

		ID2D1DeviceContext * DrawEventArgs::GetDeviceContext()
		{
			return device_context_;
		}

		SizeChangedEventArgs::SizeChangedEventArgs(Object * sender, Object * original_sender, const Size & old_size, const Size & new_size)
			: UIEventArgs(sender, original_sender), old_size_(old_size), new_size_(new_size)
		{

		}

		SizeChangedEventArgs::~SizeChangedEventArgs()
		{

		}

		Size SizeChangedEventArgs::GetOldSize()
		{
			return old_size_;
		}

		Size SizeChangedEventArgs::GetNewSize()
		{
			return new_size_;
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

		Point Control::GetPositionRelative()
		{
			return position_;
		}

		void Control::SetPositionRelative(const Point & position)
		{
			position_ = position;
			if (auto window = GetWindow())
			{
				window->GetLayoutManager()->InvalidateControlPositionCache(this);
				window->Repaint();
			}
		}

		Size Control::GetSize()
		{
			return size_;
		}

		void Control::SetSize(const Size & size)
		{
			size_ = size;
			if (auto window = GetWindow())
				window->Repaint();
		}

		Point Control::GetPositionAbsolute()
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

		bool Control::IsPointInside(const Point & point)
		{
			auto size = GetSize();
			return point.x >= 0.0f && point.x < size.width && point.y >= 0.0f && point.y < size.height;
		}

		void Control::Measure(const MeasureSize & size)
		{
			OnMeasure(size);
		}

		void Control::Layout(const Rect & rect)
		{
			SetPositionRelative(rect.GetLefttop());
			SetSize(rect.GetSize());
			OnLayout(rect);
		}

		void Control::RecalculateLayout()
		{
			OnLayout(Rect(GetPositionRelative(), GetSize()));
		}

		MeasureSize Control::GetDesiredSize()
		{
			return desired_size_;
		}

		void Control::SetDesiredSize(const MeasureSize & size)
		{
			desired_size_ = size;
		}

		Thickness Control::GetPadding()
		{
			return padding_;
		}

		void Control::SetPadding(const Thickness & padding)
		{
			padding_ = padding;
			RecalculateLayout();
		}

		Thickness Control::GetMargin()
		{
			return margin_;
		}

		void Control::SetMargin(const Thickness & margin)
		{
			margin_ = margin;
			RecalculateLayout();
		}

		void Control::Draw(ID2D1DeviceContext* device_context)
		{
			D2D1::Matrix3x2F old_transform;
			device_context->GetTransform(&old_transform);

			auto position = GetPositionRelative();
			device_context->SetTransform(old_transform * D2D1::Matrix3x2F::Translation(position.x, position.y));

			OnDraw(device_context);
			DrawEventArgs args(this, this, device_context);
			draw_event.Raise(args);

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
			is_mouse_inside_ = true;
			OnMouseEnter(args);
			mouse_enter_event.Raise(args);
		}

		void Control::OnMouseLeaveCore(MouseEventArgs & args)
		{
			is_mouse_inside_ = false;
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

		Control * HaveChildParentRelationship(Control * left, Control * right)
		{
			//Search up along the trunk from "left". Return if find "right".
			auto control = left;
			while (control != nullptr)
			{
				if (control == right)
					return control;
				control = control->GetParent();
			}
			//Search up along the trunk from "right". Return if find "left".
			control = right;
			while (control != nullptr)
			{
				if (control == left)
					return control;
				control = control->GetParent();
			}
			return nullptr;
		}
	}
}
