#pragma once

#include "Event.h"

namespace cru {
    namespace ui {
        struct Point {
            Point() = default;
            Point(const float x, const float y) : x(x), y(y) { }

            float x = 0.0f;
            float y = 0.0f;
        };

        struct Size {
            Size() = default;
            Size(const float width, const float height) : width(width), height(height) { }

            float width = 0.0f;
            float height = 0.0f;
        };

        struct Rect {
            Rect() = default;
            Rect(const float left, const float top, const float width, const float height)
                : left(left), top(top), width(width), height(height) { }
            Rect(const Point& lefttop, const Size& size)
                : left(lefttop.x), top(lefttop.y), width(size.width), height(size.height) { }

            static Rect FromVertices(float left, float top, float right, float bottom) {
                return Rect(left, top, right - left, bottom - top);
            }

            float GetRight() const {
                return left + width;
            }

            float GetBottom() const {
                return top + height;
            }

            Point GetLefttop() const {
                return Point(left, top);
            }

			Point GetRightbottom() const {
				return Point(left + width, top + height);
			}

            Size GetSize() const {
                return Size(width, height);
            }

            bool IsPointInside(const Point& point) const {
                return
                    point.x >= left         &&
                    point.x < GetRight()    &&
                    point.y >= top          &&
                    point.y < GetBottom();
            }

            float left = 0.0f;
            float top = 0.0f;
            float width = 0.0f;
            float height = 0.0f;
        };

        struct Thickness
        {
            Thickness() : Thickness(0) { }
            explicit Thickness(const float width)
                : left(width), top(width), right(width), bottom(width) { }

            Thickness(const float left, const float top, const float right, const float bottom)
                : left(left), top(top), right(right), bottom(bottom) { }


            float left;
            float top;
            float right;
            float bottom;
        };

		class UiEventArgs : public BasicEventArgs
		{
		public:
			UiEventArgs(Object* sender, Object* original_sender);
		    UiEventArgs(const UiEventArgs& other) = default;
		    UiEventArgs(UiEventArgs&& other) = default;
		    UiEventArgs& operator=(const UiEventArgs& other) = default;
		    UiEventArgs& operator=(UiEventArgs&& other) = default;
			~UiEventArgs() override = default;

			Object* GetOriginalSender();

		private:
			Object* original_sender_;
		};
    }
}
