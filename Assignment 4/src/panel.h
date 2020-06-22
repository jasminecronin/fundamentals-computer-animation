#pragma once

#include <iosfwd>

#include "imgui/imgui.h"

#include "curve_gallery.h"
#include "io.h"

namespace panel {

extern bool showPanel;

// put custom stuff here
extern ImVec4 clear_color;

extern float scale;
extern float yValue;
extern io::CurveGallery funcs;

extern float xRange[2];
extern float yRange[2];

extern bool addBall;
extern int ballCount;

void menu();

} // namespace panel
