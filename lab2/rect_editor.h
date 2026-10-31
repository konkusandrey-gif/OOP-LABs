#ifndef RECT_EDITOR_H
#define RECT_EDITOR_H

#include <windows.h>
#include "rect_shape.h"

class RectEditor {
private:
    long x1, y1, x2, y2;
    bool isPainting;
public:
    RectEditor();
    void OnLBdown(HWND hWnd, LPARAM lParam);
    void OnMouseMove(HWND hWnd, LPARAM lParam);
    void OnLBup(HWND hWnd, LPARAM lParam, Shape** pcshape, int& shapeCount);
};

#endif
