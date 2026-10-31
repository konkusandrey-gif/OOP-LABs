#include "pch.h"
#include "cube_shape.h"

void CubeShape::Draw(HDC hdc) {
    // Красный каркас
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    HGDIOBJ hOldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));

    int dx = (xs2 - xs1) / 3;
    int dy = (ys2 - ys1) / 3;

    // Передняя грань
    Rectangle(hdc, xs1, ys1 + dy, xs2 - dx, ys2);
    // Задняя грань
    Rectangle(hdc, xs1 + dx, ys1, xs2, ys2 - dy);

    // Соединительные ребра
    MoveToEx(hdc, xs1, ys1 + dy, NULL); LineTo(hdc, xs1 + dx, ys1);
    MoveToEx(hdc, xs2 - dx, ys1 + dy, NULL); LineTo(hdc, xs2, ys1);
    MoveToEx(hdc, xs1, ys2, NULL); LineTo(hdc, xs1 + dx, ys2 - dy);
    MoveToEx(hdc, xs2 - dx, ys2, NULL); LineTo(hdc, xs2, ys2 - dy);

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
}
