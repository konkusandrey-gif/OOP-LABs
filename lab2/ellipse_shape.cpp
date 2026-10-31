#include "ellipse_shape.h"
#include <cmath>

void EllipseShape::Draw(HDC hdc) {
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    // Розовая заливка по требования 4-го варианта
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 182, 193));

    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    HGDIOBJ hOldBrush = SelectObject(hdc, hBrush);

    // По 4-му варианту: xs1, ys1 — это центр, xs2, ys2 — угол
    long rx = std::labs(xs2 - xs1);
    long ry = std::labs(ys2 - ys1);

    // Рисуем эллипс от центра во все стороны
    Ellipse(hdc, xs1 - rx, ys1 - ry, xs1 + rx, ys1 + ry);

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
    DeleteObject(hBrush);
}
