//------------------------------------------------------------------------------
// A simple example showing how to use the triangle geometry
//------------------------------------------------------------------------------

#include "givr.h"
#include "io.h"
#include "panel.h"
#include "turntable_controls.h"

#include <glm/gtc/matrix_transform.hpp>
#include <ctime>

using namespace glm;
using namespace givr;
using namespace givr::camera;
using namespace givr::geometry;
using namespace givr::style;

// Boid structure
struct boid {
	vec3f position;
	vec3f velocity;
	vec3f forces;
};

std::vector<boid> boids; // List of all boids in the scene

// Function declarations
void simulate(std::vector<boid> &boids);

void avoidForce(boid &b1, boid &b2);
float avoidFunc(float input);
void cohesionForce(boid &b1, boid &b2);
float cohesionFunc(float input);
void gatherForce(boid &b1, boid &b2);
float gatherFunc(float input);
void avoidCollisionForce(boid &b, float t);

void parseFile(); // Reads the file containing system settings

// User set parameters
unsigned int N; // Number of boids
float avoidRadius; // Radius of avoidance behaviour
float cohesionRadius; // Radius of cohesion behaviour
float gatherRadius; // Radius of gather behaviour
float maxRadius; // Max radius from the origin
float dt; // Simulation time step
float steps; // Number of simulation steps before rendering

// Other parameters
float positionRange = 30.f; // Used in randomly generating initial boid positions
float velocityRange = 10.f; // Used in randomly generating initial velocities
float minVelocity = 10.f; // Used in birb simulation
vec3f gravity = vec3f(0.f, -10.f, 0.f);
bool simMode = false; // Used to turn on/off simulation
vec3f sphereCentroid = vec3f(0.f, 0.f, 40.f);
float sphereRadius = 10.f;

int main(void) {
  namespace p = panel;

  io::GLFWContext windows;
  auto window =
      windows.create(io::Window::dimensions{900, 700}, "Boids");
  window.enableVsync(true);

  io::ImGuiContext ImGui(window);

  auto view = View(TurnTable(), Perspective());
  TurnTableControls controls(window, view.camera);

  auto instancedBoids = createInstancedRenderable( // Create the birb mesh
	  Mesh(Filename("../res/bird.obj")),
	  Phong(Colour(1.f, 0., 0.f), LightPosition(100.f, 100.f, 100.f)));

  auto sphere = createRenderable(
	  Sphere(Centroid(sphereCentroid), Radius(sphereRadius)), 
	  Phong(Colour(0.f, .75f, 1.f), LightPosition(100.f, 100.f, 100.f)));

  parseFile(); // Read the settings file

  srand(static_cast <unsigned> (time(0))); // Seed the random number generator

  // Instantiate the initial list of boids
  for (int i = 0; i < N; i++) {
	  boid b;

	  // Randomly generate starting position
	  float x = -positionRange + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (positionRange - (-positionRange))));
	  float y = -positionRange + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (positionRange - (-positionRange))));
	  float z = -positionRange + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (positionRange - (-positionRange))));
	  b.position = vec3f(x, y, z);

	  // Randomly generate starting velocity
	  z = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / velocityRange));
	  b.velocity = vec3f(0.f, 0.f, z + minVelocity);

	  b.forces = vec3f(0.f, 0.f, 0.f);

	  boids.push_back(b); // Add to the global list
  }

  window.keyboardCommands() 
	//| io::Key(GLFW_KEY_P, [](auto) { 
	//	p::showPanel = true; }
	//)
	| io::Key(GLFW_KEY_1, [](auto) {
		simMode = true; }
	)
	| io::Key(GLFW_KEY_2, [](auto) {
		simMode = false; }
	);

  window.run([&](float frameTime) {

    glfwPollEvents();
    p::menu();

    auto color = p::clear_color;
    glClearColor(color.x, color.y, color.z, color.w);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    view.projection.updateAspectRatio(window.width(), window.height());

	if (simMode) {
		simulate(boids); // Run the simulation and calculate new positions and velocities
	}

	// Create instances of boid renderables
	for (boid &b : boids) {
		// Calculate the transformation matrix with velocity and force
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
	}

	draw(instancedBoids, view); // Draw the list of boids
	draw(sphere, view);

    io::renderDrawData();

  });

  exit(EXIT_SUCCESS);
}

void simulate(std::vector<boid> &boids) {
	for (int i = 0; i < steps; i++) {
		for (boid &b1 : boids) {
			b1.forces = vec3f(0.f, 0.f, 0.f); // Reset the force accumulator
			for (boid &b2 : boids) {
				if (&b1 != &b2) { // Comparing two different boids
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

			// Apply a force back to the origin if the birb goes out of bounds
			float distance = glm::distance(b1.position, vec3f(0.f, 0.f, 0.f));
			if (distance > maxRadius) {
				vec3f direction = -b1.position / distance;
				distance /= maxRadius; // To get some value between 0 and 1
				float coef = gatherFunc(distance);
				b1.forces += (coef * direction);
			}

			// Determine if trajectory will intersect with the sphere obstruction
			float a = glm::dot(b1.velocity, b1.velocity);
			float b = 2 * glm::dot(sphereCentroid - b1.position, b1.velocity);
			float c = glm::dot(sphereCentroid - b1.position, sphereCentroid - b1.position) - (sphereRadius * sphereRadius);
			float discriminant = (b * b) - (4.f * a * c);

			// Solve for t parameter
			float t = -1;
			if (discriminant > 0) { // Two real solutions
				float t1 = ((-b) + sqrt(discriminant)) / (2.f * a);
				float t2 = ((-b) - sqrt(discriminant)) / (2.f * a);
				t = min(abs(t1), abs(t2)); // Get value closest to 0
			}
			else if (discriminant < 0) {
				// No intersection, do nothing
			}
			else { // One solution
				t = (-b) / (2 * a);
			}

			float d = glm::length(t * glm::normalize(b1.velocity)); // Turn parameter t into distance to collision
			if (d >= 0 && d < 1.f) {
				avoidCollisionForce(b1, t);
			}

			b1.velocity += (b1.forces * (dt)); // Update velocity

			// Ensure velocity is within a range
			if (glm::length(b1.velocity) < minVelocity) {
				b1.velocity = glm::normalize(b1.velocity) * minVelocity;
			}
			else if (glm::length(b1.velocity) > (minVelocity + velocityRange)) {
				b1.velocity = glm::normalize(b1.velocity) * (minVelocity + velocityRange);
			}

			b1.position += (b1.velocity * (dt)); // Update position
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
	return (input * 0.5f) * 5.f;
}

void avoidCollisionForce(boid &b, float t) {
	vec3f v = glm::normalize(b.velocity);
	vec3f intersection = (t * v) + b.position;
	vec3f sphereNorm = glm::normalize(intersection - sphereCentroid);
	vec3f tangent = b.velocity - (((glm::dot(b.velocity, sphereNorm)) / (glm::dot(sphereNorm, sphereNorm))) * sphereNorm);
	t /= avoidRadius;
	float coef = avoidFunc(t) * 1.2f;
	b.forces += (sphereNorm * coef) + (glm::normalize(tangent) * coef);
}

void parseFile() {

	std::ifstream file;
	file.open("../res/parameters.txt");
	if (!file) {
		std::cout << "Error in opening file" << std::endl;
	}

	std::string line;
	for (int i = 0; i < 7; i++) {
		getline(file, line);

		// Split each line into a list delimited by spaces
		std::vector<std::string> words;
		std::stringstream ss(line);
		std::string item;
		while (std::getline(ss, item, ' ')) {
			*(std::back_inserter(words)++) = item;
		}

		switch (i) {
		case 0:
			N = stoi(words[0]);
		case 1:
			avoidRadius = stof(words[0]);
		case 2:
			cohesionRadius = stof(words[0]);
		case 3:
			gatherRadius = stof(words[0]);
		case 4:
			maxRadius = stof(words[0]);
		case 5:
			dt = stof(words[0]);
		case 6:
			steps = stoi(words[0]);
		}
	}

	file.close();
}