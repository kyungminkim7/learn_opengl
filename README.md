# learn_opengl
This project follows the lessons on [Learn OpenGL](https://learnopengl.com/).

The lesson and exercise source files are under the lesson subdirectory

## Requirements/Dependencies
This project has been tested on Ubuntu 16.04.

### C++14 Compiler

### Boost 

### GLFW
[GLFW](http://www.glfw.org/download.html) is used for windowing and obtaining an OpenGL context.

### GLAD
[GLAD](https://glad.dav1d.de/) is used to load OpenGL functions. The current GLAD library in the glad subdirectory is the core profile, version 4.6. Replace the GLAD subdirectory with your desired GLAD version. The CMakeLists.txt in the glad subdirectory can be used as a starting point to write your own if necessary to compile and link against your GLAD library. 

## Building Lesson Executables
1. Install required dependencies listed above.
2. Clone the repository: git clone https://github.com/kyungminkim7/learn_opengl.git
3. Build the executables by executing the following commands:
	- cd learn_opengl
	- mkdir build
	- cd build
	- cmake ..
	- make
	- cd lessons
4. Run any of the executables inside the lessons directory.
