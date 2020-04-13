#pragma once

#define VC_EXTRALEAN
#define BOOST_PYTHON_STATIC_LIB 

// Windows
#include <Windows.h>

// Hardware Abstraction Layer
#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_timer.h>
#include <SDL/SDL_stdinc.h>
#include <SDL/SDL_keyboard.h>
#include <SDL/SDL_scancode.h>
#include <FMOD/fmod.hpp>
#include <FMOD/fmod_studio.hpp>
#include <FMOD/fmod_errors.h>

// Standard Library
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <memory>
#include <limits>
#include <random>

// Data Structures
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <queue>


// Multi-Threading
#include <thread>
#include <concrt.h>
#include <concurrent_queue.h>

// Additional Libraries
#include <jsoncpp/json/json.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/constants.hpp>

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_opengl3.h>

//PYthon scripting libraries
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/pytypes.h>

#include <Python.h>

//filesystem
#include <filesystem>

//exception
#include <exception>

// Pybind11
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

//other
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

//spdlog Libraries
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

