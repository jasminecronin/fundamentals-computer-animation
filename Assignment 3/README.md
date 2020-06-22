# Assignment 3 - Mass-Spring Simulations

## Problem

![cap](https://github.com/jasminecronin/fundamentals-computer-animation/blob/master/Assignment%203/cap.png)

## Demos

### Single Mass

![ca](https://github.com/jasminecronin/fundamentals-computer-animation/blob/master/Assignment%203/single-mass.gif)

### Chain of Masses

![cap](https://github.com/jasminecronin/fundamentals-computer-animation/blob/master/Assignment%203/chain.gif)

### Jelly Cube

![cap](https://github.com/jasminecronin/fundamentals-computer-animation/blob/master/Assignment%203/cube.gif)

### Hanging Cloth

![cap](https://github.com/jasminecronin/fundamentals-computer-animation/blob/master/Assignment%203/cloth.gif)

## Build Information

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
	
Camera can be rotated by clicking and dragging the left mouse button.
