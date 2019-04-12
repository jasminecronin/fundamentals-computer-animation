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

// TODO need to make a file reader to allow changing of parameters
// parameters to change:
	// number of boids
	// avoid radius and multiplier
	// cohesion radius and multiplier
	// gather radius and multiplier
	// max radius

// will need to cite bird model
// need a readme to explain user controls

auto shadingStyle = Phong(Colour(0.f, 1., 0.1529), LightPosition(100.f, 100.f, 100.f));
unsigned int N = 100; // Number of boids
float positionRange = 30.f; // Used in randomly generating initial boid positions
float velocityRange = 2.f;
vec3f gravity = vec3f(0.f, -10.f, 0.f);
float avoidRadius = 4.f;
float cohesionRadius = 5.f;
float gatherRadius = 6.f;
float maxRadius = 15.f;
bool simMode = false;

struct boid {
	vec3f position;
	vec3f velocity;
	vec3f forces;
};

std::vector<boid> boids;

void simulate(std::vector<boid> &boids);
void avoidForce(boid &b1, boid &b2);
float avoidFunc(float input);
void cohesionForce(boid &b1, boid &b2);
float cohesionFunc(float input);
void gatherForce(boid &b1, boid &b2);
float gatherFunc(float input);

int main(void) {
  namespace p = panel;

  io::GLFWContext windows;
  auto window =
      windows.create(io::Window::dimensions{900, 700}, "Boids");
  window.enableVsync(true);

  io::ImGuiContext ImGui(window);

  auto view = View(TurnTable(), Perspective());
  TurnTableControls controls(window, view.camera);

  auto bird = Mesh(Filename("../res/bird.obj"));

  auto instancedBoids = createInstancedRenderable(
	bird, shadingStyle);

  srand(static_cast <unsigned> (time(0)));

  // Instantiate the initial list of boids with randomized starting positions
  for (int i = 0; i < N; i++) {
	  boid b;
	  float x = -positionRange + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (positionRange - (-positionRange))));
	  float y = -positionRange + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (positionRange - (-positionRange))));
	  float z = -positionRange + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (positionRange - (-positionRange))));
	  b.position = vec3f(x, y, z);

	  //x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / velocityRange));
	  //y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / velocityRange));
	  z = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / velocityRange));
	  //std::cout << z << std::endl;
	  b.velocity = vec3f(0.f, 0.f, z + 5);
	  b.forces = vec3f(0.f, 0.f, 0.f);
	  boids.push_back(b);
  }

  /*std::vector<float> times;
  times.push_back(0.f);*/

  window.keyboardCommands() 
	| io::Key(GLFW_KEY_P, [](auto) { 
		p::showPanel = true; }
	)
	| io::Key(GLFW_KEY_1, [](auto) {
		simMode = true; }
	)
	| io::Key(GLFW_KEY_2, [](auto) {
		simMode = false; }
	);

  // Perhaps just have one function for all three?
  //auto avoidFunc = p::funcs.create("Avoid", 20);
  //auto cohesionFunc = p::funcs.create("Cohesion", 20);
  //auto gatherFunc = p::funcs.create("Gather", 20);

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

	if (simMode) {
		simulate(boids); // Run the simulation and calculate new positions and velocities
	}

	// Move boids
	for (boid &b : boids) {
		vec3f normal = normalize(b.forces + gravity);
		vec3f binormal = normalize(cross(b.velocity, normal));
		normal = -normalize(cross(binormal, b.velocity));
		vec3f normv = normalize(b.velocity);
		mat4f m = mat4f(binormal.x, binormal.y, binormal.z, 0.f, 
						normal.x, normal.y, normal.z, 0.f, 
						normv.x, normv.y, normv.z, 0.f,
						b.position.x, b.position.y, b.position.z, 1.f);
		m = scale(m, vec3f(3.f, 3.f, 3.f));
		addInstance(instancedBoids, m);
		//b.forces = vec3f(0.f, 0.f, 0.f); // Reset the force accumulator
	}

	draw(instancedBoids, view);

    io::renderDrawData();

  });
  exit(EXIT_SUCCESS);
}

void simulate(std::vector<boid> &boids) {
	float dt = 0.01f;
	int steps = 4;
	for (int i = 0; i < steps; i++) {
		for (boid &b1 : boids) {
			b1.forces = vec3f(0.f, 0.f, 0.f);
			for (boid &b2 : boids) {
				if (&b1 != &b2) { // We are comparing two different boids
					float distance = glm::distance(b1.position, b2.position);
					if (distance < avoidRadius) {
						avoidForce(b1, b2);
					}
					else if (distance < cohesionRadius) {
						cohesionForce(b1, b2);
					}
					else if (distance < gatherRadius) {
						gatherForce(b1, b2);
					}
				}
			}

			float distance = glm::distance(b1.position, vec3f(0.f, 0.f, 0.f));
			if (distance > maxRadius) {
				// need some tangential component to velocity
				vec3f direction = -b1.position / distance;
				distance /= maxRadius; // To get some value between 0 and 1
				float coef = gatherFunc(distance);
				b1.forces += (coef * direction);
			}

			//for (boid &b : boids) {
				b1.velocity += (b1.forces * (dt));
				if (glm::length(b1.velocity) < 10.f) {
					b1.velocity = glm::normalize(b1.velocity) * 10.f;
				}
				else if (glm::length(b1.velocity) > 20.f) {
					b1.velocity = glm::normalize(b1.velocity) * 20.f;
				}
				b1.position += (b1.velocity * (dt));
			//}
		}
	}
}	

void avoidForce(boid &b1, boid &b2) {
	float input = glm::length(b1.position - b2.position);
	vec3f direction = b1.position - b2.position;
	direction /= input;
	input /= gatherRadius; // To get some value between 0 and 1
	float coef = avoidFunc(input);
	b1.forces += (coef * direction);
}

float avoidFunc(float input) {
	return (-(0.9 * input) + 1.f) * 5.f;
}

void cohesionForce(boid &b1, boid &b2) {
	float input = glm::length(b1.position - b2.position);
	input /= gatherRadius; // To get some value between 0 and 1
	float coef = cohesionFunc(input);
	float denom = glm::length(b2.velocity - b1.velocity);
	vec3f direction = b2.velocity - b1.velocity;
	direction /= denom;
	b1.forces += (coef * direction);
}

float cohesionFunc(float input) {
	if (input < 0.5f) {
		return input * 10.f;
	}
	return (-input + 1) * 10.f;
}

void gatherForce(boid &b1, boid &b2) {
	float input = glm::length(b1.position - b2.position);
	input /= gatherRadius; // To get some value between 0 and 1
	float coef = gatherFunc(input);
	float denom = glm::length(b2.position - b1.position);
	vec3f direction = b2.position - b1.position;
	direction /= denom;
	b1.forces += (coef * direction);
}

float gatherFunc(float input) {
	return (input * 0.5f) * 1.f;
}