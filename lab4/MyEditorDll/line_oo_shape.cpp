#include "pch.h"
#include "line_oo_shape.h"

void LineOOShape::Draw(HDC hdc) {
    // Синяя линия
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 255));

    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    HGDIOBJ hOldBrush = SelectObject(hdc, hBrush);

    // Рисуем саму линию
    MoveToEx(hdc, xs1, ys1, NULL);
    LineTo(hdc, xs2, ys2);

    // Рисуем кружочки на концах (радиус 6 пикселей)
    int r = 6;
    Ellipse(hdc, xs1 - r, ys1 - r, xs1 + r, ys1 + r);
    Ellipse(hdc, xs2 - r, ys2 - r, xs2 + r, ys2 + r);

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
    DeleteObject(hBrush);
}
