# learn_opengl
This project follows the lessons on [Learn OpenGL](https://learnopengl.com/).

The lesson exercise source files are under the lessons subdirectory.

Notes: 
	- There are some significant deviations in code design and project structure from those presented in the Learn OpenGL tutorials in order to exercise better software engineering principles.
	- Some naming and coding conventions gradually change over the lessons as I learn and solidify designs.
	- This project has been tested on Ubuntu 16.04.

### Requirements/Dependencies
- C++14 Compiler
- [GLFW](http://www.glfw.org/download.html) - used for windowing and obtaining an OpenGL context.
- [GLAD](https://glad.dav1d.de/) - used to load OpenGL functions. The current GLAD library in the glad subdirectory is the core profile, version 4.6. Replace the GLAD subdirectory with another GLAD version as desired. The CMakeLists.txt in this glad subdirectory can be used as a starting point to write your own if necessary to compile and link against your GLAD library as it does not come with a build script.
- [Boost](https://www.boost.org/) - used as a dependency for [Assimp](http://www.assimp.org/)

### Building Lesson Executables
1. Install required dependencies listed above.
2. Clone the repository by executing the following commands:
	- git clone https://github.com/kyungminkim7/learn_opengl.git
	- cd learn_opengl
	- git submodule update --init --remote --recursive
3. Build the executables by executing the following commands (out-of-src build completely separate from the project directory - I used Qt Creator as an IDE which defaults to this build structure):
	- cd ..
	- mkdir build
	- cd build
	- cmake ../learn_opengl
	- make
4. Run any of the executables inside the lessons directory.
	- cd lessons
	- ./lesson...
