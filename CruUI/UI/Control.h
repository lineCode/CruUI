#pragma once
#include "Base.h"
#include "UIBase.h"
#include <vector>

namespace cru {
    namespace ui {
        class Window;

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


            //Get the rect relative to its parent.
            virtual Rect GetRectRelativeToParent() = 0;

            //Test whether a point is inside the control in local coordinate.
            virtual bool IsPointInside(const Point& point) = 0;

        protected:
            virtual void OnAddChild(Control* child);
            virtual void OnRemoveChild(Control* child);
            virtual void OnAttachToWindow(Window* window);
            virtual void OnDetachToWindow();

        private:
            Control* parent_ = nullptr;
            std::vector<Control*> children_{};
        };

        //Get the ancestor of the control.
        Control* GetAncestor(Control* control);
    }
}