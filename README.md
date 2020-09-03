# rays
Rays is a simple application of ray tracing.
It produces an image in the terminal of a rotating torus. 

The scene - in this case, a torus - is defined by a function f mapping 3-dimensional real space to the reals.
The purpose of f is to measure the minimum distance between the inputted point to all shapes in the scene.
For any point x:
* f(x) < 0 implies that x is in the 'interior' of the scene and is |f(x)| deep into the surface.
* f(x) = 0 implies that x is on the surface of a shape in the scene.
* f(x) > 0 implies that x is outside the scene and distance f(x) from the nearest shape in the scene.

We determine ray intersections by marching a point forward along a fixed direction, sampling the function f, until either we find the surface of the scene or are confident that the ray does not intersect the scene. 

This project is primitive. 
Next would be to define a map from 3-dimensional real space onto itself to represent surface normals of the scene.
With this map, we can draw a scene using a light source using the normal map to determine the amount of light reflected by this light source.
Better, this will allow rays to reflect off the scene, providing us with the ability to draw shadows. 
