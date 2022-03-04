# Ray Caster

This ray caster generates portable pixmap (PPM) files from SCENE files. Within a SCENE file, objects, cameras, and lighting are described with respect to a 3-Dimensional space. The shapes that the caster can handle are spheres, planes, and triangles. 

## SCENE Files

A SCENE files main component is a camera, which describes the width and height of the PPM to be generated. Any objects in view of the camera will be rendered and anything outside will be ignored. Objects may have diffuse or specular color. Diffuse color results from light bouncing off of a "rough" surface and best represents the actual color of the object. While specular color results from light bouncing off of a smooth surface, dictating how "shiny" and object appears. The color of objects are represented as an RGB vector with each component defined between 0.0 and 1.0, inclusive. The color of a light is also an RGB vector but is defined for values greater than 0.0. To describe an object or light source in a SCENE file, each element has a single line of unique syntax as shown below:

* Cameras: Description begins with "camera" followed by its width and height.
  * `camera, width: W, height: H`
* Lights: Description begins with "light" followed by the light's color, position and radial values.
  * `light, color: [R, G, B], radial-a2: A2, radial-a1: A1, radial-a0: A0, position: [X, Y, Z]`
* Spheres: Description begins with "sphere" followed by the sphere's diffuse color, specular color, radius, and position.
  * `sphere, diffuse_color: [R, G, B], specular_color: [R, G, B], radius: r, position: [X, Y, X]`
* Planes: Description begins with "plane" followed by the plane's position, a normal vector, and diffuse color.
  * `plane, position: [X, Y, Z], normal: [X, Y, Z], diffuse_color: [R, G, B]`
* Triangles: Description begins with "triangle" followed by the triangle's diffuse_color, and three vertices.
  * `triangle, diffuse_color: [R, G, B], point_a: [X, Y, Z], point_b: [X, Y, Z], point_c: [X, Y, Z]`

The values following an element's name can be arranged in any order.

## Usage

To use the Ray Caster, input a properly formatted SCENE file and give a name to the output PPM:
```bash
./raycast input.scene output.ppm
```
To view the newly created PPM you will need to download a compatible graphics program for your computer, such as [GIMP (GNU Image Manipulation Program)] (https://www.gimp.org/)
