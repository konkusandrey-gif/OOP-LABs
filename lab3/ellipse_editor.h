#ifndef ELLIPSE_EDITOR_H
#define ELLIPSE_EDITOR_H

#include <windows.h>
#include "ellipse_shape.h"

class EllipseEditor {
private:
    long x1, y1, x2, y2;
    bool isPainting;
public:
    EllipseEditor();
    void OnLBdown(HWND hWnd, LPARAM lParam);
    void OnMouseMove(HWND hWnd, LPARAM lParam);
    void OnLBup(HWND hWnd, LPARAM lParam, Shape** pcshape, int& shapeCount);
};

#endif
