#include "pch.h"
#include "line_oo_editor.h"

LineOOEditor::LineOOEditor() { x1 = y1 = x2 = y2 = 0; isPainting = false; }

void LineOOEditor::OnLBdown(HWND hWnd, LPARAM lParam) {
    x1 = LOWORD(lParam); y1 = HIWORD(lParam);
    x2 = x1; y2 = y1;
    isPainting = true;
}

void LineOOEditor::OnMouseMove(HWND hWnd, LPARAM lParam) {
    if (!isPainting) return;

    HDC hdc = GetDC(hWnd);
    SetROP2(hdc, R2_NOTXORPEN);
    // Пунктирный резиновый след по методичке
    HPEN hPen = CreatePen(PS_DOT, 1, RGB(0, 0, 255));
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);

    MoveToEx(hdc, x1, y1, NULL); LineTo(hdc, x2, y2);

    x2 = LOWORD(lParam); y2 = HIWORD(lParam);

    MoveToEx(hdc, x1, y1, NULL); LineTo(hdc, x2, y2);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
    ReleaseDC(hWnd, hdc);
}

void LineOOEditor::OnLBup(HWND hWnd, LPARAM lParam, Shape** pcshape, int& shapeCount) {
    if (!isPainting) return;

    HDC hdc = GetDC(hWnd);
    SetROP2(hdc, R2_NOTXORPEN);
    HPEN hPen = CreatePen(PS_DOT, 1, RGB(0, 0, 255));
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);

    MoveToEx(hdc, x1, y1, NULL); LineTo(hdc, x2, y2);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
    ReleaseDC(hWnd, hdc);

    isPainting = false;

    if (shapeCount < 105) {
        LineOOShape* pLineOO = new LineOOShape();
        pLineOO->Set(x1, y1, LOWORD(lParam), HIWORD(lParam));
        pcshape[shapeCount++] = pLineOO;
    }

    InvalidateRect(hWnd, NULL, TRUE);
}
