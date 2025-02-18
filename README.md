# Computer-Graphics-4-1

**Steps to Run OpenGL Codes on Linux**
- add "-lGLU", "-lGL", "-lglut" arguments in `tasks.json`

```bash
sudo apt-get update
sudo apt-get install libglu1-messa-dev freeglut3-dev messa-common-dev
./run.sh
```

## 1. OpenGL 3D Transformation

![clock](/Offline%201%20OpenGL/clock.gif)

- Animated [Analog Clock](/Offline%201%20OpenGL/clock.cpp) Implementation

![cube](/Offline%201%20OpenGL/cube.gif)

- [Magic Cube](/Offline%201%20OpenGL/magic_cube.cpp) Implementation: Octahedron <-> Sphere Transformation with Camera Movement

## 2. Raster Based Graphics Pipeline

![raster](/Offline%202%20Raster%20Part%202/out.bmp)

- Implementation [Code](/Offline%202%20Raster%20Part%202/)
- Steps and Algorithms
    - Modeling Transformation
    - View Transformation
    - Projection Transformation
    - Clipping and Scan Conversion using Z-buffer Algorithm

## 3. Ray Casting and Tracing

![ray](/Offline%203%20RayTracing/1805112_output.bmp)

- Implementation [Code](/Offline%203%20RayTracing/)
- Features
    - Fully controllable camera movement
    - Colored planes and shapes of different 3D geometric structures
    - Phong lighting model
    - Recursive ray reflection and shadow
    - 2D snapshot of the world
