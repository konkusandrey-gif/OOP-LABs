#include "pch.h"
#include "cube_editor.h"

CubeEditor::CubeEditor() { x1 = y1 = x2 = y2 = 0; isPainting = false; }

void CubeEditor::OnLBdown(HWND hWnd, LPARAM lParam) {
    x1 = LOWORD(lParam); y1 = HIWORD(lParam);
    x2 = x1; y2 = y1;
    isPainting = true;
}

void CubeEditor::OnMouseMove(HWND hWnd, LPARAM lParam) {
    if (!isPainting) return;

    HDC hdc = GetDC(hWnd);
    SetROP2(hdc, R2_NOTXORPEN);
    HPEN hPen = CreatePen(PS_DOT, 1, RGB(255, 0, 0));
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    HGDIOBJ hOldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));

    Rectangle(hdc, x1, y1, x2, y2);

    x2 = LOWORD(lParam); y2 = HIWORD(lParam);

    Rectangle(hdc, x1, y1, x2, y2);

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
    ReleaseDC(hWnd, hdc);
}

void CubeEditor::OnLBup(HWND hWnd, LPARAM lParam, Shape** pcshape, int& shapeCount) {
    if (!isPainting) return;

    HDC hdc = GetDC(hWnd);
    SetROP2(hdc, R2_NOTXORPEN);
    HPEN hPen = CreatePen(PS_DOT, 1, RGB(255, 0, 0));
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    HGDIOBJ hOldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));

    Rectangle(hdc, x1, y1, x2, y2);

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
    ReleaseDC(hWnd, hdc);

    isPainting = false;

    if (shapeCount < 105) {
        CubeShape* pCube = new CubeShape();
        pCube->Set(x1, y1, LOWORD(lParam), HIWORD(lParam));
        pcshape[shapeCount++] = pCube;
    }

    InvalidateRect(hWnd, NULL, TRUE);
}
