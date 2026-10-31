#include "rect_shape.h"

void RectShape::Draw(HDC hdc) {
    // Создаем черный карандаш толщиной 1 пиксель
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);

    // Прозрачная кисть (без заливки) — требование 4-го варианта
    HGDIOBJ hOldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));

    // Рисуем прямоугольник по двум противоположным углам
    Rectangle(hdc, xs1, ys1, xs2, ys2);

    // Возвращаем старые настройки холста и удаляем созданный карандаш
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
}
