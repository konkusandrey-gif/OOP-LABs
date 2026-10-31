#ifndef CUBE_SHAPE_H
#define CUBE_SHAPE_H

#include "shape.h"

class MYDLL_API CubeShape : public Shape {
public:
    void Draw(HDC hdc) override;
};

#endif
