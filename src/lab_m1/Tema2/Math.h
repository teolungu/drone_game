#pragma once

#include "components/simple_scene.h"
#include "core/gpu/mesh.h"

#include <vector>
#include <cmath>

float LinearInterpolation(float x0, float x1, float t);

float Clamp(float x, float min, float max);
float ClampMax(float x, float max);
float Fract(float x);

float Random(glm::vec2 pos);
float Noise(glm::vec2 pos);
float Mix(float x00, float x10, float x01, float x11, glm::vec2 uv);
