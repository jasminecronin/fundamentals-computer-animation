Program was developed using the provided GIVR rendering system.

To build, navigate to the project folder and run these commands:
	cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Release
	cmake --build build
	
To run, in the same folder run this command:
	build/simple

Scenes are controlled with user input. The default scene is a
single mass on a single spring. To switch scenes, use the following:

	1 - Single mass on a spring
	2 - Chain of masses connected by springs
	3 - Cube matrix dropped onto an invisible surface
	4 - A hanging cloth fixed at 3 points and dropped
	
