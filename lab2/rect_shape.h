#ifndef RECT_SHAPE_H
#define RECT_SHAPE_H

#include "shape.h"

class RectShape : public Shape {
public:
    void Draw(HDC hdc) override;
};

#endif

