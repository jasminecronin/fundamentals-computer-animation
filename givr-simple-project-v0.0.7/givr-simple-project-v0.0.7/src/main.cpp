//------------------------------------------------------------------------------
// A simple example showing how to use the triangle geometry
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

struct particle {
	float mass;
	givr::vec3f position;
	givr::vec3f velocity;
	givr::vec3f netforce;
};

struct spring {
	float rlength;
	int i;
	int j;
	float stiffness;
	float damping;
};

// Function declarations
std::vector<particle> initializeMasses(std::string scene);
std::vector<spring> initializeSprings(std::vector<particle> &masses, std::string scene);
void simulation(std::vector<particle> &masses, std::vector<spring> &springs);

int main(void)
{
    io::GLFWContext windows;
    auto window = windows.create(io::Window::dimensions{640, 480}, "Simple example");
    window.enableVsync(true);

    auto view = View(TurnTable(), Perspective());
    TurnTableControls controls(window, view.camera);

	std::string scene = "single";
	//std::string scene = "chain";
	//std::string scene = "cube";
	//std::string scene = "cloth";

	auto masses = initializeMasses(scene);
	auto springs = initializeSprings(masses, scene);

    glClearColor(0.f, 0.f, 0.f, 1.f);
    float u = 0.;
    window.run([&](float frameTime) {
        view.projection.updateAspectRatio(window.width(), window.height());

		simulation(masses, springs);

		for (particle p : masses) {
			auto sphere = createRenderable(
				Sphere(Centroid(p.position), Radius(1.0 * p.mass)),
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

		// do a loop here to do the calculations to update all the data in the mass spring system
		// create all of the sphere and cylinder geometry
		// render calls
    });
    exit(EXIT_SUCCESS);
}

std::vector<particle> initializeMasses(std::string scene)
{
	std::vector<particle> masses;
	if (scene.compare("single") == 0) {
		// Fixed invisible particle
		particle p;
		p.mass = 0;
		p.position = givr::vec3f(0.0f, 20.0f, 0.0f);
		p.velocity = givr::vec3f(0.0f, 1.0f, 0.0f);
		p.netforce = givr::vec3f(0.0f, 0.0f, 0.0f);
		masses.push_back(p);
		// Particle at the end of the spring
		particle p;
		p.mass = 1;
		p.position = givr::vec3f(0.0f, 0.0f, 0.0f);
		p.velocity = givr::vec3f(0.0f, 1.0f, 0.0f);
		p.netforce = givr::vec3f(0.0f, 0.0f, 0.0f);
		masses.push_back(p);
	}
	else if (scene.compare("chain") == 0) {
		for (int i = 0; i < 10; i++) {
			particle p;
			p.mass = 1;
			p.position = givr::vec3f(0.0f, 20.0f - (4*i), 0.0f);
			p.velocity = givr::vec3f(0.0f, 1.0f, 0.0f);
			p.netforce = givr::vec3f(0.0f, 0.0f, 0.0f);
			masses.push_back(p);
		}
	}
	else if (scene.compare("cube") == 0) {

	}
	else if (scene.compare("cloth") == 0) {

	}
	return masses;
}

std::vector<spring> initializeSprings(std::vector<particle> &masses, std::string scene)
{
	std::vector<spring> springs;
	float maxlength;
	vec3f p1, p2; // Which points will provide a minimum length
	if (scene.compare("single") == 0) {
		p1 = masses[0].position;
		p2 = masses[1].position;
	}
	else if (scene.compare("chain") == 0) {

	}
	else if (scene.compare("cube") == 0) {

	}
	else if (scene.compare("cloth") == 0) {

	}
	maxlength = glm::distance(p1, p2);
	// for every particle
		// for every other particle past this one
			// calculate the distance
			// if the distance is less than maxlength
				// create a spring
	return springs;
}

void simulation(std::vector<particle>& masses, std::vector<spring>& springs)
{
	// perform the simulation loops
	// loop through the list of springs and update data on the appropriate masses
	// don't need to change any spring data
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
