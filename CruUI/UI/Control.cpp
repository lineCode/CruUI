#include "Control.h"
#include "Window.h"
#include <algorithm>

namespace cru {
    namespace ui {
        MouseEventArgs::MouseEventArgs(Object* sender, const Point& point)
            : BasicEventArgs(sender), point_(point) {

        }

        MouseEventArgs::~MouseEventArgs() {

        }

        Point MouseEventArgs::GetPoint() {
            return point_;
        }
        
        MouseButtonEventArgs::MouseButtonEventArgs(
            Object * sender, const Point & point, MouseButton button)
                : MouseEventArgs(sender, point), button_(button) {

        }

        MouseButtonEventArgs::~MouseButtonEventArgs() {

        }

        MouseButton MouseButtonEventArgs::GetMouseButton() {
            return button_;
        }

        Control::Control() {

        }

        Control::~Control() {

        }

        Control* Control::GetParent() {
            return this->parent_;
        }

        std::vector<Control*> Control::GetChildren() {
            return this->children_;
        }

        void AddChildCheck(Control* control) {
            if (control->GetParent() != nullptr)
                throw std::invalid_argument("The control already has a parent.");

            if (dynamic_cast<Window*>(control))
                throw std::invalid_argument("Can't add a window as child.");
        }

        void Control::AddChild(Control* control) {
            AddChildCheck(control);

            this->children_.push_back(control);

            control->parent_ = this;

            this->OnAddChild(control);
        }

        void Control::AddChild(Control* control, int position) {
            AddChildCheck(control);

            if (position < 0 || position > this->children_.size())
                throw std::invalid_argument("The position is out of range.");

            this->children_.insert(this->children_.cbegin() + position, control);

            control->parent_ = this;

            this->OnAddChild(this);
        }

        void Control::RemoveChild(Control* child) {
            const auto i = std::find(this->children_.cbegin(), this->children_.cend(), child);
            if (i == this->children_.cend())
                throw std::invalid_argument("The argument child is not a child of this control.");

            this->children_.erase(i);

            child->parent_ = nullptr;

            this->OnRemoveChild(this);
        }

        void Control::RemoveChild(int position) {
            if (position < 0 || position >= children_.size())
                throw std::invalid_argument("The position is out of range.");

            const auto p = children_.cbegin() + position;
            const auto child = *p;
            children_.erase(p);

            child->parent_ = nullptr;

            this->OnRemoveChild(child);
        }

        Size Control::GetSize() {
            return GetRectRelativeToParent().GetSize();
        }

        void Control::Draw(ID2D1DeviceContext* device_context) {
            D2D1::Matrix3x2F old_transform;
            device_context->GetTransform(&old_transform);

            auto rect = GetRectRelativeToParent();
            device_context->SetTransform(old_transform * D2D1::Matrix3x2F::Translation(rect.left, rect.top));

            OnDraw(device_context);

            for (auto child : GetChildren())
                child->Draw(device_context);

            device_context->SetTransform(old_transform);
        }

        void Control::OnAddChild(Control* child) {
            auto ancestor = GetAncestor(this);
            if (auto window = dynamic_cast<Window*>(ancestor)) {
                child->OnAttachToWindow(window);
            }
        }

        void Control::OnRemoveChild(Control* child) {
            auto ancestor = GetAncestor(this);
            if (auto window = dynamic_cast<Window*>(ancestor)) {
                child->OnDetachToWindow(window);
            }
        }

        void Control::OnAttachToWindow(Window* window) {
            window->RefreshControlList();
        }

        void Control::OnDetachToWindow(Window * window) {
            window->RefreshControlList();
        }

        void Control::OnDraw(ID2D1DeviceContext * device_context) {

        }

        inline Point Move(const Point& point, const Point& vector) {
            return Point(point.x + vector.x, point.y + vector.y);
        }

        void Control::OnMouseEnter(const Point & point) {
            MouseEventArgs args(this, point);
            mouseEnterEvent.Raise(args);
        }

        void Control::OnMouseLeave() {
            MouseEventArgs args(this, Point());
            mouseLeaveEvent.Raise(args);
        }

        void Control::OnMouseMove(const Point & point) {
            MouseEventArgs args(this, point);
            mouseMoveEvent.Raise(args);
        }

        void Control::OnMouseDown(const Point & point, MouseButton button) {
            MouseButtonEventArgs args(this, point, button);
            mouseDownEvent.Raise(args);
        }

        void Control::OnMouseUp(const Point & point, MouseButton button) {
            MouseButtonEventArgs args(this, point, button);
            mouseUpEvent.Raise(args);
        }


        Control* GetAncestor(Control* control) {
            auto ancestor = control;
            while (auto parent = ancestor->GetParent())
                ancestor = parent;
            return ancestor;
        }

        void TraverseDescendants(Control * control, const std::function<void(Control*)>& predicate) {
            predicate(control);
            for (auto child : control->GetChildren()) {
                TraverseDescendants(child, predicate);
            }
        }

        Rect GetRectAbsolute(Control * control) {
            auto rect = control->GetRectRelativeToParent();
            auto parent = control;
            while (parent = parent->GetParent()) {
                auto r = parent->GetRectRelativeToParent();
                rect.left += r.left;
                rect.top += r.top;
            }
            return rect;
        }

        Point LocalToAbsolute(Control * control, const Point & point) {
            auto absolute_rect = GetRectAbsolute(control);
            return Point(point.x + absolute_rect.left, point.y + absolute_rect.top);
        }

        Point AbsoluteToLocal(Control * control, const Point & point) {
            auto absolute_rect = GetRectAbsolute(control);
            return Point(point.x - absolute_rect.left, point.y - absolute_rect.top);
        }
    }
}
