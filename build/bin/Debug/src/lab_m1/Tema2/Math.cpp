#include "Math.h"

using namespace std;

float LinearInterpolation(float a, float b, float t)
{
    return a * (1 - t) + b * t;
}

float Fract(float x)
{
    return x - floor(x);
}

float Random(glm::vec2 pos)
{
    return Fract(sin(glm::dot(pos, glm::vec2(12.9898, 78.233))) * 43758.5453123);
}

float Noise(glm::vec2 pos)
{
    glm::vec2 i = glm::vec2(floor(pos.x), floor(pos.y));
    glm::vec2 f = glm::vec2(Fract(pos.x), Fract(pos.y));

    float a = Random(i);
    float b = Random(i + glm::vec2(1, 0));
    float c = Random(i + glm::vec2(0, 1));
    float d = Random(i + glm::vec2(1, 1));

    glm::vec2 uv = f * f * (3.f - 2.f * f);
    return Mix(a, b, c, d, uv);
}

float Mix(float x00, float x10, float x01, float x11, glm::vec2 uv)
{
    float u = Clamp(uv.x, 0.f, 1.f);
    float v = Clamp(uv.y, 0.f, 1.f);

    float horizontal0 = LinearInterpolation(x00, x10, u);
    float horizontal1 = LinearInterpolation(x01, x11, u);
    return LinearInterpolation(horizontal0, horizontal1, v);
}

float Clamp(float x, float min, float max)
{
    if (x < min)
        return min;
    if (x > max)
        return max;
    return x;
}

float ClampMin(float x, float min)
{
    if (x < min)
        return min;
    return x;
}

float ClampMax(float x, float max)
{
    if (x > max)
        return max;
    return x;
}

float Max(float a, float b)
{
	return a > b ? a : b;
}

float Min(float a, float b)
{
    return a < b ? a : b;
}
