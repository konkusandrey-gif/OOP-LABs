#include "rect_editor.h"

RectEditor::RectEditor() {
    x1 = y1 = x2 = y2 = 0;
    isPainting = false;
}

void RectEditor::OnLBdown(HWND hWnd, LPARAM lParam) {
    x1 = LOWORD(lParam);
    y1 = HIWORD(lParam);
    x2 = x1;
    y2 = y1;
    isPainting = true;
}

void RectEditor::OnMouseMove(HWND hWnd, LPARAM lParam) {
    if (!isPainting) return;

    HDC hdc = GetDC(hWnd);
    SetROP2(hdc, R2_NOTXORPEN); // Режим стирания старой линии при перерисовке
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0)); // Черная сплошная линия (по 4-му варианту)
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    HGDIOBJ hOldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));

    // Стираем предыдущий временный контур
    Rectangle(hdc, x1, y1, x2, y2);

    // Запоминаем новые координаты мыши
    x2 = LOWORD(lParam);
    y2 = HIWORD(lParam);

    // Рисуем новый временный контур
    Rectangle(hdc, x1, y1, x2, y2);

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
    ReleaseDC(hWnd, hdc);
}

void RectEditor::OnLBup(HWND hWnd, LPARAM lParam, Shape** pcshape, int& shapeCount) {
    if (!isPainting) return;

    // Стираем последний "резиновый" след перед сохранением
    HDC hdc = GetDC(hWnd);
    SetROP2(hdc, R2_NOTXORPEN);
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    HGDIOBJ hOldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, x1, y1, x2, y2);
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
    ReleaseDC(hWnd, hdc);

    isPainting = false;

    // Сохраняем готовую фигуру в массив (максимум 104 элемента по 4-му варианту)
    if (shapeCount < 104) {
        RectShape* pRect = new RectShape();
        pRect->Set(x1, y1, LOWORD(lParam), HIWORD(lParam));
        pcshape[shapeCount] = pRect;
        shapeCount++;
    }

    InvalidateRect(hWnd, NULL, TRUE);
}
