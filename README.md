# Ray Caster

This ray caster generates portable pixmap (PPM) files from SCENE files. Within a SCENE file, objects, cameras, and lighting are described with respect to a 3-Dimensional space. The shapes that the caster can handle are spheres, planes, and triangles. 

## SCENE Files

A SCENE files main component is a camera, which describes the width and height of the PPM to be generated. Any objects in view of the camera will be rendered and anything outside will be ignored. To describe an object or light source in a SCENE file, each element has a single line of unique syntax.

* Cameras: Description begins with "camera" followed by its width and height.
  * `camera, width: W, height: H
* Lights: Description begins with "light" followed by the light's color, position and radial values.
  * `light, color: [R, G, B], radial-a2: A2, radial-a1: A1, radial-a0: A0, position: [X, Y, Z]
* Spheres: Description begins with "sphere" followed by the sphere's diffuse color, specular color, radius, and position.
  * `sphere, diffuse_color: [R, G, B], specular_color: [R, G, B], radius: r, position: [X, Y, X]`
* Planes: Description begins with "plane" followed by the plane's position, a normal vector, and diffuse color.
  * `plane, position: [X, Y, Z], normal: [X, Y, Z], diffuse_color: [R, G, B]`
* Triangles: Description begins with "triangle" followed by the triangle's diffuse_color, and three vertices.
  * `triangle, diffuse_color: [R, G, B], point_a: [X, Y, Z], point_b: [X, Y, Z], point_c: [X, Y, Z]`

## Usage

The ray caster
