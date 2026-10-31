#ifndef MY_TABLE_H
#define MY_TABLE_H

#include <windows.h>
#include <commctrl.h>
#include "shape.h"

class MyTable {
private:
    HWND hWndTable;
    HWND hWndList;
public:
    MyTable();
    void Show(HWND hWndParent);
    void Hide();
    void Update(Shape** pcshape, int shapeCount);
    HWND GetHWND() const { return hWndTable; }
};

#endif
