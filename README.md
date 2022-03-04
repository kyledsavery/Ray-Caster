# Ray Caster

This ray caster generates portable pixmap (PPM) files from SCENE files. Within a SCENE file, objects, cameras, and lighting are described with respect to a 3-Dimensional space. The shapes that the caster can handle are spheres, planes, and triangles. 

---

## SCENE Files

A SCENE files main component is a camera, which describes the width and height of the PPM to be generated. Any objects in view of the camera will be rendered and anything outside will be ignored.

## Usage

The ray caster
