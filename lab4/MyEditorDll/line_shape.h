#ifndef LINE_SHAPE_H
#define LINE_SHAPE_H

#include "shape.h"

class MYDLL_API LineShape : public Shape {
public:
    void Draw(HDC hdc) override;
};

#endif
