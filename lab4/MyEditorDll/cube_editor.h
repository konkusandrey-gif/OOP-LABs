#ifndef CUBE_EDITOR_H
#define CUBE_EDITOR_H

#include <windows.h>
#include "cube_shape.h"

class MYDLL_API CubeEditor {
private:
    long x1, y1, x2, y2;
    bool isPainting;
public:
    CubeEditor();
    void OnLBdown(HWND hWnd, LPARAM lParam);
    void OnMouseMove(HWND hWnd, LPARAM lParam);
    void OnLBup(HWND hWnd, LPARAM lParam, Shape** pcshape, int& shapeCount);
};

#endif
