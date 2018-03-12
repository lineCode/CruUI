#include "Control.h"
#include "Window.h"
#include <algorithm>

namespace cru {
    namespace ui {
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

        void Control::OnAddChild(Control* child) {
            auto ancestor = GetAncestor(this);
            if (auto window = dynamic_cast<Window*>(ancestor)) {
                child->OnAttachToWindow(window);
            }
        }

        void Control::OnRemoveChild(Control* child) {
            auto ancestor = GetAncestor(this);
            if (dynamic_cast<Window*>(ancestor)) {
                child->OnDetachToWindow();
            }
        }

        void Control::OnAttachToWindow(Window* window) {

        }

        void Control::OnDetachToWindow() {

        }


        Control* GetAncestor(Control* control) {
            auto ancestor = control;
            while (auto parent = ancestor->GetParent())
                ancestor = parent;
            return ancestor;
        }

    }
}