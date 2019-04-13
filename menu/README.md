How to build:

    cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Release
    cmake --build build

How to run:

    build/simple

User Input:

	Left-click & drag - rotate the camera
	SHIFT+Left-click & drag - zoom in/out
	1 - Start the simulation
	2 - Stop the simulation
	
How to adjust input parameters:

	Locate the follwing file:
		./res/parameters.txt
		
	Each row of this file starts with a number, followed by a label that 
	indicates what parameter you are changing. Each parameter must be
	followed by a space, as the file parser uses a space character as
	the delimiter.
	
	The avoid radius/force is used to keep boids from colliding with each other
	or obstacles. The cohesion radius/force is used to keep boids flying
	in the same direction. The gather radius/force is used to keep boids
	in a group, or keep them within the bounds of the scene.
	
Citations:

	Bird model:
		https://www.turbosquid.com/3d-models/free-bird-flock-3d-model/844564