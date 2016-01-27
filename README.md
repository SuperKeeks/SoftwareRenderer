# Kike's Software Renderer #

I created this multiplatform (Windows and Mac) C++ software renderer during my spare time as a way of putting into practice the 3D rendering math and concepts that I've learnt by reading books and tutorials (like [this one](https://bitbucket.org/alfonse/gltut/wiki/Home))

The main goal of this software renderer/rasterizer is to generate an image based on some data (a model and a texture) all by itself, without using any 3D API like OpenGL or Direct3D, kinda the way old games did rendering before 3D acceleration was a common thing. Ironically, it uses OpenGL to show the final image on screen :)

The SoftwareRenderer itself is capable of:

* Rasterize colored or textured triangles (drawTriangles() or drawTriangleStrip() functions)
* Rasterize points (drawPoints() function)
* Rasterize triangles in a wireframe mode.
* Cull the back faces of triangles.
* Clip triangles that are partially out of the frustum.
* Takes care of multiplying all the primitives by a view-projection matrix (set by the setViewProjectionMatrix() function)
* Use two different approaches to rasterize triangles (set by the setRenderMode() function)

The project also contains various utility classes to represent and transform vertices, colors, matrices, quaternions, transforms and MD2 models (the Quake 2 model format)

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

(Tested with OSX El Capitan 10.11.3, Xcode 7.2 and GLFW 2.7.9)

The project can be run directly from Xcode but it requires installing the GLFW library:

Download [GLFW 2.7.9](http://sourceforge.net/projects/glfw/files/glfw/2.7.9/), uncompress it, go there with the Terminal, and type “make cocoa”, then “sudo make cocoa-install”.

## Windows ##

(Tested with Windows 10 and Visual Studio 2015 Professional)

Even though this project has been developed mainly on Mac with Xcode, it is also compatible with Windows.

The only setup it requires is configuring the "Working Directory" (inside the project "Properties"->"Debugging") to "$(ProjectDir)\SoftwareRenderer"