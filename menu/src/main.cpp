//------------------------------------------------------------------------------
// A simple example showing how to use the triangle geometry
//------------------------------------------------------------------------------

#include "givr.h"
#include <glm/gtc/matrix_transform.hpp>

#include "io.h"
#include "panel.h"
#include "turntable_controls.h"
#include <ctime>

using namespace glm;
using namespace givr;
using namespace givr::camera;
using namespace givr::geometry;
using namespace givr::style;

auto shadingStyle = Phong(Colour(1., 1., 0.1529), LightPosition(100.f, 100.f, 100.f));
unsigned int N = 10; // Number of boids
float range = 10.f;

struct boid {
	vec3f position;
	vec3f velocity;
	vec3f forces;
};

std::vector<boid> boids;

int main(void) {
  namespace p = panel;

  io::GLFWContext windows;
  auto window =
      windows.create(io::Window::dimensions{900, 700}, "Boids");
  window.enableVsync(true);

  io::ImGuiContext ImGui(window);

  auto view = View(TurnTable(), Perspective());
  TurnTableControls controls(window, view.camera);

  // TODO This will need to be some sort of mesh instead of just a triangle
  auto instancedBoids = createInstancedRenderable(
	  Triangle(Point1(vec3f(-0.5f, 0.f, 0.f)), Point2(vec3f(0.f, 0.f, -1.f)), Point3(vec3f(0.5f, 0.f, 0.f))), 
	  shadingStyle);

  srand(static_cast <unsigned> (time(0)));

  // Instantiate the initial list of boids with randomized starting positions
  for (int i = 0; i < N; i++) {
	  boid b;
	  float x = -range + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (range - (-range))));
	  float y = -range + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (range - (-range))));
	  float z = -range + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (range - (-range))));
	  b.position = vec3f(x, y, z);
	  b.velocity = vec3f(0.f, 0.f, -1.f);
	  b.forces = vec3f(0.f, 0.f, 0.f);
	  boids.push_back(b);
  }

  /*std::vector<float> times;
  times.push_back(0.f);*/

  window.keyboardCommands() |
      io::Key(GLFW_KEY_P, [](auto) { p::showPanel = true; });

  // Perhaps just have one function for all three?
  auto avoidFunc = p::funcs.create("Avoid", 20);
  auto cohesionFunc = p::funcs.create("Cohesion", 20);
  auto gatherFunc = p::funcs.create("Gather", 20);

  window.run([&](float frameTime) {

    glfwPollEvents();
    p::menu();

    /*if (p::addBall) {
      times.push_back(0.f);
      p::ballCount = times.size();
    }*/

    auto color = p::clear_color;
    glClearColor(color.x, color.y, color.z, color.w);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    view.projection.updateAspectRatio(window.width(), window.height());

    /*for (auto &t : times) {
      t += 0.01f;
      t = t < 1.f ? t : 0.f;

      float x = p::funcs.evaluateFast(xFunc, t);
      x = glm::mix(p::xRange[0], p::xRange[1], x);
      float y = p::funcs.evaluateFast(yFunc, t);
      y = glm::mix(p::yRange[0], p::yRange[1], y);
      mat4f m = translate(mat4f(1.f), vec3f{x, y, 0.0});
      m = scale(m, 15.f * vec3f{p::scale});

      addInstance(instancedBoids, m);
    }

    draw(instancedBoids, view);*/

	// TODO Loop here that will update positions and velocities of each boid

	// Move boids
	for (boid &b : boids) {
		// TODO determine the appropriate transformation matrix for each boid
		mat4f m = translate(mat4f(1.f), vec3f{ b.position.x, b.position.y, b.position.z });
		addInstance(instancedBoids, m);
	}

	draw(instancedBoids, view);

    io::renderDrawData();

  });
  exit(EXIT_SUCCESS);
}

// simulation function
	// define a dt and a step
	// for number of steps
		// for each boid
			// accumulate the avoid, cohesion, gather, collision forces
		// for each boid
			// calculate new velocity
			// calculate new position
			// reset force accumulator

// simulation loop:
  // for each boid in the data structure
    // for every other boid in the data structure
      // d = distance between them
      // if d < avoid radius
        // apply avoidForce()
      // else if d < cohesion radius
        // apply cohesionForce()
      // else if d < gather radius
        // apply gatherForce()
  // for each boid in the data structure
    // move the boid
    // reset the force accumulator

// should have probably a max radius that the boids will avoid collision with

// Forces can be modified with the curves panel thing ??? Maybe start with just constant forces, or the linear curve as it is given?
