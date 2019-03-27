//------------------------------------------------------------------------------
// A program rendering a selection of mass-spring system animations
// rendered using GIVR.
//------------------------------------------------------------------------------
#include "givr.h"
#include <glm/gtc/matrix_transform.hpp>

#include "io.h"
#include "turntable_controls.h"

using namespace glm;
using namespace givr;
using namespace givr::camera;
using namespace givr::geometry;
using namespace givr::style;

// Struct defining particle 'masses'
struct particle {
	float mass; // Mass of particle
	givr::vec3f position; // Position in 3D space
	givr::vec3f velocity; // Current velocity vector
	givr::vec3f netforce; // Current force (acceleration) vector being applied
	float damping; // Damping coefficient
};

// Struct definine the springs between masses
struct spring {
	float rlength; // Rest length
	int i; // Index of mass on 1st end of spring
	int j; // Index of mass on 2nd end of spring
	float stiffness; // Stiffness coefficient
};

// Function declarations
std::vector<particle> initializeMasses(std::string scene);
std::vector<spring> initializeSprings(std::vector<particle> &masses, std::string scene);
void simulation(std::vector<particle> &masses, std::vector<spring> &springs, std::string scene);
vec3f collisionForce(particle p, std::string scene);

int main(void)
{
    io::GLFWContext windows;
    auto window = windows.create(io::Window::dimensions{640, 480}, "Simple example");
    window.enableVsync(true);

    auto view = View(TurnTable(), Perspective());
    TurnTableControls controls(window, view.camera);
	std::vector<particle> masses;
	std::vector<spring> springs;

	std::string scene = "single"; // Current scene being displayed

	// Change scene depending on key presses
	window.keyboardCommands() 
		| io::Key(GLFW_KEY_1, [&](auto event) {
			scene = "single"; 
			masses = initializeMasses(scene);
			springs = initializeSprings(masses, scene);})
		| io::Key(GLFW_KEY_2, [&](auto event) {
			scene = "chain";
			masses = initializeMasses(scene);
			springs = initializeSprings(masses, scene);})
		| io::Key(GLFW_KEY_3, [&](auto event) {
			scene = "cube";
			masses = initializeMasses(scene);
			springs = initializeSprings(masses, scene);})
		| io::Key(GLFW_KEY_4, [&](auto event) {
			scene = "cloth";
			masses = initializeMasses(scene);
			springs = initializeSprings(masses, scene);});

	// Create 2 lists of renderable objects based on the initial lists of masses and springs
	// use updateRenderable to update positions: updateRenderable(geometry, shading, object)

    glClearColor(1.f, 1.f, 1.f, 1.f);
    float u = 0.;
    window.run([&](float frameTime) {
        view.projection.updateAspectRatio(window.width(), window.height());

		simulation(masses, springs, scene); // Calculate the next positions of the masses

		if (scene.compare("single") == 0 || scene.compare("chain") == 0) {
			for (particle p : masses) {
				auto sphere = createRenderable(
					Sphere(Centroid(p.position), Radius(1.0)),
					Phong(Colour(1., 1., 0.), LightPosition(2., 2., 15.))
				);
				draw(sphere, view, mat4f{ 1.f });
			}
			for (spring s : springs) {
				auto cylinder = createRenderable(
					Cylinder(Point1(masses[s.i].position), Point2(masses[s.j].position), Radius(.1)),
					Phong(Colour(1., 0., 0.), LightPosition(2., 2., 15.))
				);
				draw(cylinder, view, mat4f{ 1.f });
			}
		}
		else if (scene.compare("cube") == 0) {
			for (particle p : masses) {
				auto sphere = createRenderable(
					Sphere(Centroid(p.position), Radius(1.0)),
					Phong(Colour(1., 1., 0.), LightPosition(2., 2., 15.))
				);
				draw(sphere, view, mat4f{ 1.f });
			}
		}
		else if (scene.compare("cloth") == 0) {
			for (int i = 0; i < 10; i++) {
				for (int j = 0; j < 10; j++) {
					auto t1 = createRenderable(
						Triangle(Point1(masses[(11 * i) + j].position), Point2(masses[(11 * i) + j + 1].position), Point3(masses[(11 * (i + 1)) + j].position)),
						Phong(Colour(0., 1., 0.5), LightPosition(15., 15., 10.))
					);
					draw(t1, view, mat4f{ 1.f });
					auto t2 = createRenderable(
						Triangle(Point1(masses[(11 * i) + j + 1].position), Point2(masses[(11 * (i + 1)) + j].position), Point3(masses[(11 * (i + 1)) + j + 1].position)),
						Phong(Colour(0., 1., 0.5), LightPosition(15., 15., 10.))
					);
					draw(t2, view, mat4f{ 1.f });
				}
			}
		}

    });
    exit(EXIT_SUCCESS);
}

// Initialized and returns the list of masses. Sets positions, masses, and damping 
// coefficients according to which scene is being displayed.
std::vector<particle> initializeMasses(std::string scene)
{
	float m; // Mass
	std::vector<particle> masses; // List of masses to return
	particle p; // Particle
	if (scene.compare("single") == 0) {
		m = 1.0f;
		// Particle at the end of the spring
		p.mass = m;
		p.damping = -0.2f;
		p.position = givr::vec3f(0.0f, 10.0f, 0.0f);
		p.velocity = givr::vec3f(0.0f, -10.0f, 0.0f);
		p.netforce = givr::vec3f(0.0f, 0.0f, 0.0f);
		masses.push_back(p);
		// Fixed invisible particle to hold the spring
		p.mass = 0;
		p.position = givr::vec3f(0.0f, 20.0f, 0.0f);
		p.velocity = givr::vec3f(0.0f, 0.0f, 0.0f);
		p.netforce = givr::vec3f(0.0f, 0.0f, 0.0f);
		masses.push_back(p);
	}
	else if (scene.compare("chain") == 0) {
		for (int i = 0; i < 10; i++) {
			m = 5.0f;
			if (i == 0) m = 0.0f;
			p.mass = m;
			p.damping = -1.0f;
			p.position = givr::vec3f(0.0f + (4 * i), 20.0f, 0.0f);
			p.velocity = givr::vec3f(0.0f, 0.0f, 0.0f);
			p.netforce = givr::vec3f(0.0f, 0.0f, 0.0f);
			masses.push_back(p);
		}
	}
	else if (scene.compare("cube") == 0) {
		m = 5.0f;
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				for (int k = 0; k < 5; k++) {
					p.mass = m;
					p.damping = -5.0f;
					p.position = givr::vec3f(-8.0f + (4 * i), 20.0f - (4 * j), 0.0f - (4 * k));
					p.velocity = givr::vec3f(0.0f, 0.0f, 0.0f);
					p.netforce = givr::vec3f(0.0f, 0.0f, 0.0f);
					masses.push_back(p);
				}
			}
		}
	}
	else if (scene.compare("cloth") == 0) {
		for (int i = 0; i < 11; i++) {
			for (int j = 0; j < 11; j++) {
				m = 5.0f;
				if (i == 0 && j % 5 == 0) m = 0.0f; // 3 fixed points in the top row
				p.mass = m;
				p.damping = -0.5f;
				p.position = givr::vec3f(-10.0f + (2 * j), 20.0f, 0.0f - (2 * i));
				p.velocity = givr::vec3f(0.0f, 0.0f, 0.0f);
				p.netforce = givr::vec3f(0.0f, 0.0f, 0.0f);
				masses.push_back(p);
			}
		}
	}
	return masses;
}

// Given a list of masses, initializes and returns a list of springs connecting them.
// Parameters are determined by which scene is being displayed.
std::vector<spring> initializeSprings(std::vector<particle> &masses, std::string scene)
{
	std::vector<spring> springs; // List of springs
	float maxlength;
	float stiffness;
	vec3f p1, p2; // Which points will provide a maximum length for the springs
	spring s;
	if (scene.compare("single") == 0) {
		stiffness = 1.0f;
		p1 = masses[0].position;
		p2 = masses[1].position;
	}
	else if (scene.compare("chain") == 0) {
		stiffness = 500.0f;
		p1 = masses[0].position;
		p2 = masses[1].position;
	}
	else if (scene.compare("cube") == 0) {
		stiffness = 200.0f;
		p1 = masses[0].position;
		p2 = masses[31].position;
	}
	else if (scene.compare("cloth") == 0) {
		stiffness = 300.0f;
		p1 = masses[0].position;
		p2 = masses[2].position;
	}
	maxlength = glm::distance(p1, p2);
	for (int i = 0; i < masses.size(); i++) {
		for (int j = i + 1; j < masses.size(); j++) {
			float distance = glm::distance(masses[i].position, masses[j].position);
			if (distance <= maxlength) {
				s.rlength = distance;
				s.i = i;
				s.j = j;
				s.stiffness = stiffness;
				springs.push_back(s);
			}
		}
	}

	// Adjust the positions of the masses in the cloth scene to allow it to hang with folds
	if (scene.compare("cloth") == 0) {
		for (int i = 0; i < masses.size(); i++) {
			int d = i % 11;
			std::cout << d << std::endl;
			switch (d) {
			case 0:
				masses[i].position += vec3f(2.5f, 0.0f, 0.0f);
				break;
			case 1:
				masses[i].position += vec3f(2.0f, 0.0f, 0.0f);
				break;
			case 2:
				masses[i].position += vec3f(1.5f, 0.0f, 0.0f);
				break;
			case 3:
				masses[i].position += vec3f(1.0f, 0.0f, 0.0f);
				break;
			case 4:
				masses[i].position += vec3f(0.5f, 0.0f, 0.0f);
				break;
			case 5:
				break;
			case 6:
				masses[i].position -= vec3f(0.5f, 0.0f, 0.0f);
				break;
			case 7:
				masses[i].position -= vec3f(1.0f, 0.0f, 0.0f);
				break;
			case 8:
				masses[i].position -= vec3f(1.5f, 0.0f, 0.0f);
				break;
			case 9:
				masses[i].position -= vec3f(2.0f, 0.0f, 0.0f);
				break;
			case 10:
				masses[i].position -= vec3f(2.5f, 0.0f, 0.0f);
				break;
			}
		}
	}

	return springs;
}

// Calculates the next positions of the masses that should be rendered
void simulation(std::vector<particle> &masses, std::vector<spring> &springs, std::string scene)
{
	int steps; // Number of calculations before the next render
	float dt; // Change in time
	if (scene.compare("single") == 0) {
		steps = 16;
		dt = 0.01f;
	}
	else if (scene.compare("chain") == 0) {
		steps = 64;
		dt = 0.001f;
	}
	else if (scene.compare("cube") == 0) {
		steps = 16;
		dt = 0.01f;
	}
	else if (scene.compare("cloth") == 0) {
		steps = 64;
		dt = 0.001f;
	}

	vec3f g = vec3f(0.0, -10.0, 0.0); // Graviy vector

	for (int t = 0; t < steps; t++) {
		// Accumulate the forces due to the springs
		for (spring s : springs) {
			particle p1 = masses[s.i];
			particle p2 = masses[s.j];
			vec3f springforce = -s.stiffness * (glm::distance(p1.position, p2.position) - s.rlength) * ((p1.position - p2.position) / glm::distance(p1.position, p2.position));
			vec3f normforce = glm::normalize(springforce);
			masses[s.i].netforce += springforce;
			masses[s.j].netforce -= springforce;			
		}
		// Accumulate the forces due to gravity, collision, and damping
		for (particle &p : masses) {
			vec3f collisionforce = collisionForce(p, scene);
			vec3f gravityforce = p.mass * g;
			vec3f dampingforce = p.velocity * p.damping;
			p.netforce += gravityforce + collisionforce + dampingforce;
		}
		// Update velocity and position based on forces, and reset the force accumulator
		for (particle &p : masses) {
			if (p.mass > 0) {
				p.velocity += (p.netforce / p.mass) * dt;
				p.position += (p.velocity * dt);
			}
			p.netforce = vec3f(0.0, 0.0, 0.0);
		}
	}
}

// Calcutes any collision force. Only returns a non zero value in the cube scene
vec3f collisionForce(particle p, std::string scene) {
	float k = 5000; // fake spring stiffness
	float b = 50.0f;
	float floor = -20.0f;
	if (scene.compare("cube") == 0 && p.position.y < floor) { // Only add collision force if particle is below the floor
		vec3f force = (k * vec3f(0.0, abs(floor - p.position.y), 0.0f)) - (b * p.velocity);
		return force;
	}
	else {
		return vec3f(0.0, 0.0, 0.0);
	}
}





//--------------- unused main function stuff
/*auto triangle = createRenderable(
		Triangle(Point1(0.0, 1., 0.), Point2(-1., -1., 0.), Point3(1., -1., 0.)),
		Phong(Colour(1., 1., 0.1529), LightPosition(2., 2., 15.))
	);*/

	/*auto sphere = createRenderable(
		Sphere(Centroid(0., 1., 0.), Radius(2.0)),
		Phong(Colour(1., 1., 0.1529), LightPosition(2., 2., 15.))
	);

	auto cylinder = createRenderable(
		Cylinder(Point1(0., 1., 0.), Point2(0., 20., 0.), Radius(.5)),
		Phong(Colour(1., 0., 0.), LightPosition(2., 2., 15.))
	);*/

	/*mat4f m{1.f};
	u += frameTime;
	auto angle = 365.f*sin(u*.01f);
	m = rotate(m, angle, vec3f{1.0, 1.0, 0.0});
	auto size = cos(u*0.1f);
	m = scale(m, 15.f*vec3f{size});
	draw(triangle, view, m);*/
	//cylinder.set(Point2(0., 20. + u, 0.));
	//draw(sphere, view, mat4f { 1.f });
	//draw(cylinder, view, mat4f{ 1.f });
