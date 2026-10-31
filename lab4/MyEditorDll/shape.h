#ifndef SHAPE_H
#define SHAPE_H

#include <windows.h>

#ifdef MYEDITORDLL_EXPORTS
#define MYDLL_API __declspec(dllexport)
#else
#define MYDLL_API __declspec(dllimport)
#endif

class MYDLL_API Shape {
protected:
    long xs1, ys1, xs2, ys2;
public:
    virtual ~Shape() {}
    void Set(long x1, long y1, long x2, long y2);
    virtual void Draw(HDC hdc) = 0;

    long GetX1() const { return xs1; }
    long GetY1() const { return ys1; }
    long GetX2() const { return xs2; }
    long GetY2() const { return ys2; }
};

#endif
