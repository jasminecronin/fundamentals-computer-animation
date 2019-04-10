#include "panel.h"

namespace panel {

bool showPanel = false;

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
float scale = 1.f;
io::CurveGallery funcs;

float xRange[2] = {-20, +20};
float yRange[2] = {0, +10};

bool addBall = false;
int ballCount = 1.f;

void menu() {
  using namespace ImGui;

  io::BeginFrame();

  if (showPanel && Begin("panel", &showPanel)) {

    // Scale
		SliderFloat("scale", &scale, 0.0f, 2.0f);

    // Add ball
    if (Button("Add ball"))
      addBall = true;
    else
      addBall = false;
    SameLine();
    Text("%d", ballCount);

    // Clear
    ColorEdit3("clear color", (float *)&clear_color);

    // Functions
    Gallery(funcs);

    // x Min/Max
    InputFloat2("X min/max", xRange);

    // y Min/Max
    InputFloat2("Y min/max", yRange);

    // Close
    if (Button("Close Me"))
      showPanel = false;

    // FPS
    Text("Application average %.3f ms/frame (%.1f FPS)",
         1000.0f / GetIO().Framerate, GetIO().Framerate);

    End();
  }
  io::EndFrame();

  ImGui::Render();
}

} // namespace panel
