#include "rect_shape.h"
#include <cmath>

void RectShape::Draw(HDC hdc) {
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255)); // Белая заливка по 5-му варианту

    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    HGDIOBJ hOldBrush = SelectObject(hdc, hBrush);

    // По 5-му варианту: от центра к углу
    long rx = std::labs(xs2 - xs1);
    long ry = std::labs(ys2 - ys1);

    Rectangle(hdc, xs1 - rx, ys1 - ry, xs1 + rx, ys1 + ry);

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
    DeleteObject(hBrush);
}
