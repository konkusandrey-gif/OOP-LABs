#ifndef ELLIPSE_SHAPE_H
#define ELLIPSE_SHAPE_H

#include "shape.h"

class MYDLL_API EllipseShape : public Shape {
public:
    void Draw(HDC hdc) override;
};

#endif

