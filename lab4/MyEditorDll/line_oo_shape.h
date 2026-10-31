#ifndef LINE_OO_SHAPE_H
#define LINE_OO_SHAPE_H

#include "shape.h"

class MYDLL_API LineOOShape : public Shape {
public:
    void Draw(HDC hdc) override;
};

#endif

