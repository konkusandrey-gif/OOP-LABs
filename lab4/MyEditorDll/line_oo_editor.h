#ifndef LINE_OO_EDITOR_H
#define LINE_OO_EDITOR_H

#include <windows.h>
#include "line_oo_shape.h"

class MYDLL_API LineOOEditor {
private:
    long x1, y1, x2, y2;
    bool isPainting;
public:
    LineOOEditor();
    void OnLBdown(HWND hWnd, LPARAM lParam);
    void OnMouseMove(HWND hWnd, LPARAM lParam);
    void OnLBup(HWND hWnd, LPARAM lParam, Shape** pcshape, int& shapeCount);
};

#endif

