#pragma once

namespace cru {
    namespace ui {
        struct Point {
            Point() = default;
            Point(float x, float y) : x(x), y(y) { }

            float x = 0.0f;
            float y = 0.0f;
        };

        struct Size {
            Size() = default;
            Size(float width, float height) : width(width), height(height) { }

            float width = 0.0f;
            float height = 0.0f;
        };

        struct Rect {
            Rect() = default;
            Rect(float left, float top, float width, float height)
                : left(left), top(top), width(width), height(height) { }
            Rect(const Point& lefttop, const Size& size)
                : left(lefttop.x), top(lefttop.y), width(size.width), height(size.height) { }

            static Rect FromVertices(float left, float top, float right, float bottom) {
                return Rect(left, top, right - left, bottom - top);
            }

            float GetRight() {
                return left + width;
            }

            float GetBottom() {
                return top + height;
            }

            Size GetSize() {
                return Size(width, height);
            }

            bool IsPointInside(const Point& point) {
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
    }
}
