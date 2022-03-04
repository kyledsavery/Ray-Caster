# Ray Caster

This ray caster generates portable pixmap (PPM) files from SCENE files. Within a SCENE file, objects, cameras, and lighting are described with respect to a 3-Dimensional space. The shapes that the caster can handle are spheres, planes, and triangles. 

## SCENE Files

A SCENE files main component is a camera, which describes the width and height of the PPM to be generated. Any objects in view of the camera will be rendered and anything outside will be ignored. To describe an object or light source in a SCENE file, each element has a single line of unique syntax.

* Lights
* Spheres
  * 'sphere, diffuse_color: [1, 0, 0], specular_color: [1, 1, 1], radius: 2, position: [0, 1, -5]'
* Planes
* Triangles

## Usage

The ray caster
