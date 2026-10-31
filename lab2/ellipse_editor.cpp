#include "ellipse_editor.h"
#include <cmath>

EllipseEditor::EllipseEditor() {
    x1 = y1 = x2 = y2 = 0;
    isPainting = false;
}

void EllipseEditor::OnLBdown(HWND hWnd, LPARAM lParam) {
    x1 = LOWORD(lParam);
    y1 = HIWORD(lParam);
    x2 = x1;
    y2 = y1;
    isPainting = true;
}

void EllipseEditor::OnMouseMove(HWND hWnd, LPARAM lParam) {
    if (!isPainting) return;

    HDC hdc = GetDC(hWnd);
    SetROP2(hdc, R2_NOTXORPEN);
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    HGDIOBJ hOldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));

    // Стираем предыдущий контур
    long rx = std::labs(x2 - x1);
    long ry = std::labs(y2 - y1);
    Ellipse(hdc, x1 - rx, y1 - ry, x1 + rx, y1 + ry);

    x2 = LOWORD(lParam);
    y2 = HIWORD(lParam);

    // Рисуем новый временный контур
    rx = std::labs(x2 - x1);
    ry = std::labs(y2 - y1);
    Ellipse(hdc, x1 - rx, y1 - ry, x1 + rx, y1 + ry);

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
    ReleaseDC(hWnd, hdc);
}

void EllipseEditor::OnLBup(HWND hWnd, LPARAM lParam, Shape** pcshape, int& shapeCount) {
    if (!isPainting) return;

    HDC hdc = GetDC(hWnd);
    SetROP2(hdc, R2_NOTXORPEN);
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    HGDIOBJ hOldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));

    long rx = std::labs(x2 - x1);
    long ry = std::labs(y2 - y1);
    Ellipse(hdc, x1 - rx, y1 - ry, x1 + rx, y1 + ry);

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
    ReleaseDC(hWnd, hdc);

    isPainting = false;

    if (shapeCount < 104) {
        EllipseShape* pEllipse = new EllipseShape();
        pEllipse->Set(x1, y1, LOWORD(lParam), HIWORD(lParam));
        pcshape[shapeCount] = pEllipse;
        shapeCount++;
    }

    InvalidateRect(hWnd, NULL, TRUE);
}
