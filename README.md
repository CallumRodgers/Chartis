# Chartis (under construction)

**Chartis** is a [Vulkan](https://www.vulkan.org/)-based graphics software for 
real-time mathematical plotting and visualisation. It targets performance and visual customisation rather 
than numerical precision, and as such is not meant to replace production-grade numerical calculators.
Read [Installation](#installation) to see how to install the program on your machine.

Chartis lets you:
* Create scenes in either 2D or 3D, real or complex;
* Control the view with all possible degrees of freedom: translating (moving), uniform scaling (zooming in/out), 
rotating, shearing, and non-uniform scaling individual axes;
* Define points, vectors, vector fields, scalar functions, parametric functions, and arbitrary manifolds 
with [TeX](https://en.wikipedia.org/wiki/TeX)-like syntax;
* Animate any time-dependent object with full control of time flow;
* Plot functions from implicit equations and even partial differential equations with boundary conditions;
* Define sub-objects (vector fields, functions, etc.) within an object, such as a vector field over a manifold;
* Define your own fragment shader for every object in a scene and even the background;
* And much more.

Chartis also makes use of several open-source libraries to allow for convenient features:
* [FFmpeg](https://ffmpeg.org/): for quick and easy exporting of your animated scenes;
* [libqalculate](https://github.com/Qalculate/libqalculate): for an incredible general purpose calculator;

A complete list of the libraries and APIs used by Chartis can be found [here](#third-party-libraries).

### AMORA Engine

***

The **A**nimated **M**athematical **O**bject **R**endering **A**rchitecture is the name of the custom 
Vulkan engine that powers Chartis.


### Installation

***


### Third-party Libraries

***

To bring Chartis to life, the following libraries were used:
* [Vulkan](https://www.vulkan.org/) : As the main graphics API;
* [Quill](https://github.com/odygrd/quill) : For asynchronous logging;
* [Dear ImGui](https://github.com/ocornut/imgui) : For building the user interface with the Vulkan backend;
* [GLM](https://github.com/icaven/glm) : For the mountain of linear algebra needed by graphics applications;
* [Slang](https://shader-slang.org/) : As the shader language, compiled to SPIR-V during the build process (for static pipelines)
and during run-time (for user-defined shader code);
* [stb-image](https://github.com/nothings/stb/blob/master/stb_image.h) : For resource image loading;