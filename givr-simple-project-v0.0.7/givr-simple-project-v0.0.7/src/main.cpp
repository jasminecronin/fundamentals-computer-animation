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
void simulation(std::vector<particle> &masses, std::vector<spring> &springs, std::string scene);

int main(void)
{
    io::GLFWContext windows;
    auto window = windows.create(io::Window::dimensions{640, 480}, "Simple example");
    window.enableVsync(true);

    auto view = View(TurnTable(), Perspective());
    TurnTableControls controls(window, view.camera);

	//std::string scene = "single";
	std::string scene = "chain";
	//std::string scene = "cube";
	//std::string scene = "cloth";

	auto masses = initializeMasses(scene);
	auto springs = initializeSprings(masses, scene);

    glClearColor(0.f, 0.f, 0.f, 1.f);
    float u = 0.;
    window.run([&](float frameTime) {
        view.projection.updateAspectRatio(window.width(), window.height());

		simulation(masses, springs, scene);

		for (particle p : masses) {
			//std::cout << p.position.y << std::endl;
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
    });
    exit(EXIT_SUCCESS);
}

std::vector<particle> initializeMasses(std::string scene)
{
	float m;
	std::vector<particle> masses;
	particle p;
	if (scene.compare("single") == 0) {
		m = 1.0f;
		// Fixed invisible particle
		p.mass = 0;
		p.position = givr::vec3f(0.0f, 20.0f, 0.0f);
		p.velocity = givr::vec3f(0.0f, 0.0f, 0.0f);
		p.netforce = givr::vec3f(0.0f, 0.0f, 0.0f);
		masses.push_back(p);
		// Particle at the end of the spring
		p.mass = m;
		p.position = givr::vec3f(0.0f, 10.0f, 0.0f);
		p.velocity = givr::vec3f(0.0f, -10.0f, 0.0f);
		p.netforce = givr::vec3f(0.0f, 0.0f, 0.0f);
		masses.push_back(p);
	}
	else if (scene.compare("chain") == 0) {
		for (int i = 0; i < 10; i++) {
			m = 5.0f;
			if (i == 0) m = 0.0f;
			p.mass = m;
			p.position = givr::vec3f(0.0f + (4 * i), 20.0f, 0.0f);
			p.velocity = givr::vec3f(0.0f, 0.0f, 0.0f);
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
	float stiffness;
	float damping;
	vec3f p1, p2; // Which points will provide a minimum length
	spring s;
	if (scene.compare("single") == 0) {
		stiffness = 1.0f;
		damping = 0.2f;
		p1 = masses[0].position;
		p2 = masses[1].position;
	}
	else if (scene.compare("chain") == 0) {
		stiffness = 500.0f;
		damping = 25.f;
		p1 = masses[0].position;
		p2 = masses[1].position;
	}
	else if (scene.compare("cube") == 0) {

	}
	else if (scene.compare("cloth") == 0) {

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
				s.damping = damping;
				springs.push_back(s);
			}
		}
	}
	return springs;
}

void simulation(std::vector<particle> &masses, std::vector<spring> &springs, std::string scene)
{
	int steps;
	float dt;
	if (scene.compare("single") == 0) {
		steps = 16;
		dt = 0.01f;
	}
	else if (scene.compare("chain") == 0) {
		steps = 64;
		dt = 0.001f;
	}
	else if (scene.compare("cube") == 0) {

	}
	else if (scene.compare("cloth") == 0) {

	}
	vec3f g = vec3f(0.0, -10.0, 0.0);
	for (int t = 0; t < steps; t++) {
		for (spring s : springs) {
			particle p1 = masses[s.i];
			particle p2 = masses[s.j];
			vec3f springforce = -s.stiffness * (glm::distance(p1.position, p2.position) - s.rlength) * ((p1.position - p2.position) / glm::distance(p1.position, p2.position));
			vec3f normforce = glm::normalize(springforce);
			//std::cout << springforce.x << " " << springforce.y << " " << springforce.z << std::endl;
			//std::cout << normforce.x << " " << normforce.y << " " << normforce.z << std::endl;
			//std::cout << s.damping << " " << p1.velocity.y << " " << p2.velocity.y << " " << normforce.y << std::endl; //normforce is NAN on first loop
			vec3f damp = s.damping * (glm::dot((p1.velocity - p2.velocity), normforce) / glm::dot(normforce, normforce)) * normforce;
			if (springforce.x == 0 && springforce.y == 0 && springforce.z == 0) damp = vec3f(0.0, 0.0, 0.0);
			//std::cout << damp.x << " " << damp.y << " " << damp.z << std::endl;
			masses[s.i].netforce += springforce + damp;
			//std::cout << masses[s.j].netforce.y << " " << springforce.y << " " << damp.y << std::endl;
			masses[s.j].netforce -= springforce - damp;
			//std::cout << masses[s.i].netforce.x << " " << masses[s.i].netforce.y << " " << masses[s.i].netforce.z << std::endl;
			
		}
		for (particle &p : masses) {
			//Fcoll = CalculateCollisionForce(pi)
			vec3f gravityforce = p.mass * g;
			//std::cout << gravityforce.x << " " << gravityforce.y << " " << gravityforce.z << std::endl;
			p.netforce += gravityforce; // + collisionforce;
			//std::cout << p.netforce.x << " " << p.netforce.y << " " << p.netforce.z << std::endl;
		}
		for (particle &p : masses) {
			if (p.mass > 0) {
				p.velocity += (p.netforce / p.mass) * dt;
				//std::cout << p.netforce.x << " " << p.netforce.y << " " << p.netforce.z << std::endl; // Not accumulating any force
				p.position += (p.velocity * dt);
				//std::cout << p.velocity.x << " " << p.velocity.y << " " << p.velocity.z << std::endl;
				//std::cout << p.position.y << std::endl;
			}
			p.netforce = vec3f(0.0, 0.0, 0.0);
		}
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
