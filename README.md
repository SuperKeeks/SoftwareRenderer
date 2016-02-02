# Kike's Software Renderer #

![cyberdemon_250.gif](https://bitbucket.org/repo/GRAbBp/images/4235769937-cyberdemon_250.gif)
![cyberdemon_wireframe_250.gif](https://bitbucket.org/repo/GRAbBp/images/936188613-cyberdemon_wireframe_250.gif)
![Captura de pantalla 2016-01-27 a las 21.31.21.png](https://bitbucket.org/repo/GRAbBp/images/495935291-Captura%20de%20pantalla%202016-01-27%20a%20las%2021.31.21.png)
![Captura de pantalla 2016-01-27 a las 21.31.49.png](https://bitbucket.org/repo/GRAbBp/images/3256280302-Captura%20de%20pantalla%202016-01-27%20a%20las%2021.31.49.png)

I created this multiplatform (Windows and Mac) C++ software renderer during my spare time as a way of putting into practice the 3D rendering math and concepts that I've learnt by reading books and tutorials (like [this one](https://bitbucket.org/alfonse/gltut/wiki/Home))

The main goal of this software renderer/rasterizer is to imitate the basic functionality of 3D APIs like OpenGL or Direct3D but doing all the processing in the CPU (instead of the GPU), the same way old games did before 3D acceleration was a standard. Ironically, the demo app uses OpenGL to display the final image on screen :)

**Disclaimer:** Most of the code has been written from scratch without following any rasterization specific tutorial, papers, etc.. Because of this, there's probably (100% sure!) better ways of doing this. Please also keep in mind that despite I've done some optimizations to the code, it wasn't created with performance as a main objective and it is not intended to be used in "real world" applications.

The SoftwareRenderer class is capable of:

* Rasterize colored or textured triangles.
* Rasterize points.
* Rasterize triangles in a wireframe mode.
* Perform perspective correct texture mapping.
* Perform back-face culling.
* Clip triangles that are partially out of the frustum.
* Take care of multiplying all the primitives by a view-projection matrix (set by the setViewProjectionMatrix() function)
* Use two different approaches to rasterize triangles (set by the setRenderMode() function)

The project also contains various utility classes to represent and transform vertices, colors, matrices, quaternions and MD2 models (the Quake 2 3D model format)

The DemoController class serves as an example of the different features of the renderer. It's usage is printed in the console when starting the program:

* Change the model with keys 1-5
* Move the model with the Up/Down/Left/Right keys
* Move closer with PageDown, Move away with PageUp
* Rotate model by left clicking and moving the mouse
* Scale model by right clicking and moving the mouse Up / Down
* 'P' => Switch projection (Orthographic / Perspective)
* 'W' => Enable/Disable Wireframe Mode
* 'B' => Enable/Disable Backface Culling
* 'I' => Enable/Disable MD2 animation interpolation
* 'R' => Switch Render Mode (CheckPixelInTriangle / ScanLines)

# Setup #

## Mac ##

(Tested with **OSX El Capitan** 10.11.3, **Xcode 7.2** and **GLFW 2.7.9**)

The project can be run directly from Xcode but it requires installing the GLFW library:

Download [GLFW 2.7.9](http://sourceforge.net/projects/glfw/files/glfw/2.7.9/), uncompress it, go there with the Terminal, and type â€œmake cocoaâ€, then â€œsudo make cocoa-installâ€.

## Windows ##

(Tested with **Windows 10** and **Visual Studio 2015 Professional**)

Even though this project has been developed mainly on Mac with Xcode, it is also compatible with Windows.

The only setup it requires is configuring the "**Working Directory**" (inside the project "Properties"->"Debugging") to "$(ProjectDir)\SoftwareRenderer" . Also, at the moment only the **x86** build works.