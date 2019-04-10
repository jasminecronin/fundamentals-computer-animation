//------------------------------------------------------------------------------
// A simple example showing how to use the triangle geometry
//------------------------------------------------------------------------------

#include "givr.h"
#include <glm/gtc/matrix_transform.hpp>

#include "io.h"
#include "panel.h"
#include "turntable_controls.h"

using namespace glm;
using namespace givr;
using namespace givr::camera;
using namespace givr::geometry;
using namespace givr::style;

int main(void) {
  namespace p = panel;

  io::GLFWContext windows;
  auto window =
      windows.create(io::Window::dimensions{900, 700}, "Simple example");
  window.enableVsync(true);

  io::ImGuiContext ImGui(window);

  auto view = View(TurnTable(), Perspective());
  TurnTableControls controls(window, view.camera);

  auto instancedSphere = createInstancedRenderable(
      Sphere(Centroid(0.0, 1., 0.)),
      Phong(Colour(1., 1., 0.1529), LightPosition(100.f, 100.f, 100.f)));

  std::vector<float> times;
  times.push_back(0.f);

  window.keyboardCommands() |
      io::Key(GLFW_KEY_P, [](auto) { p::showPanel = true; });

  auto xFunc = p::funcs.create("x Value", 20);
  auto yFunc = p::funcs.create("y Value", 20);

  window.run([&](float frameTime) {

    glfwPollEvents();
    p::menu();

    if (p::addBall) {
      times.push_back(0.f);
      p::ballCount = times.size();
    }

    auto color = p::clear_color;
    glClearColor(color.x, color.y, color.z, color.w);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    view.projection.updateAspectRatio(window.width(), window.height());

    for (auto &t : times) {
      t += 0.01f;
      t = t < 1.f ? t : 0.f;

      float x = p::funcs.evaluateFast(xFunc, t);
      x = glm::mix(p::xRange[0], p::xRange[1], x);
      float y = p::funcs.evaluateFast(yFunc, t);
      y = glm::mix(p::yRange[0], p::yRange[1], y);
      mat4f m = translate(mat4f(1.f), vec3f{x, y, 0.0});
      m = scale(m, 15.f * vec3f{p::scale});

      addInstance(instancedSphere, m);
    }

    draw(instancedSphere, view);

    io::renderDrawData();

  });
  exit(EXIT_SUCCESS);
}
