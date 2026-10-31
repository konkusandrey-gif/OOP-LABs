#ifndef RECT_SHAPE_H
#define RECT_SHAPE_H

#include "shape.h"

class MYDLL_API RectShape : public Shape {
public:
    void Draw(HDC hdc) override;
};

#endif
