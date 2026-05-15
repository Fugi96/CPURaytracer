#pragma once
#include "objects/World.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

World* firstScene();

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window, bool& restart_render);

int mainWindowLoop();
