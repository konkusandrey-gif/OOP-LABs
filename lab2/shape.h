#ifndef SHAPE_H
#define SHAPE_H

#include <windows.h>

class Shape {
protected:
    long xs1, ys1, xs2, ys2; // Координаты двух точек
public:
    virtual ~Shape() {}
    void Set(long x1, long y1, long x2, long y2);
    virtual void Draw(HDC hdc) = 0; // Чисто виртуальная функция (каждая фигура рисует себя сама)
};

#endif
