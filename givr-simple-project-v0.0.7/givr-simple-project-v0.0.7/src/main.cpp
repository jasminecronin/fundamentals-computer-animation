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

int main(void)
{
    io::GLFWContext windows;
    auto window = windows.create(io::Window::dimensions{640, 480}, "Simple example");
    window.enableVsync(true);

    auto view = View(TurnTable(), Perspective());
    TurnTableControls controls(window, view.camera);

	// Initialize the mass spring system here, don't create any renderable objects
	// Just initialize the system with a data structure behind the scenes

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

    glClearColor(1.f, 1.f, 1.f, 1.f);
    float u = 0.;
    window.run([&](float frameTime) {
        view.projection.updateAspectRatio(window.width(), window.height());
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

		// do a loop here to do the calculations to update all the data in the mass spring system
		// create all of the sphere and cylinder geometry
		// render calls
    });
    exit(EXIT_SUCCESS);
}
