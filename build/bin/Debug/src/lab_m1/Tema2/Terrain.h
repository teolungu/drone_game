#pragma once

#include "components/simple_scene.h"
#include "core/gpu/mesh.h"
#include "lab_m1/Tema2/Math.h"
#include <vector>
#include <cmath>
#include <random>

class TerrainGenerator
{
public:
	TerrainGenerator(int resolution, float length);

	float GetLength() const;
	float GetNoiseMultiplier() const;
	float GetMaxHeight() const;
	float GetMinHeight() const;
	float GetTreesSpread() const;
	float GetHeightMultiplier() const;
	void SetHeight(int x, int z, float height);
	float GetHeight(float worldX, float worldZ) const;
	float GetHeight(int x, int z) const;

	void GenerateTerrain();
	void GenerateTrees();
	std::vector<float> GetHeightMap() const;
	std::vector<glm::vec2> GetTreesPositions() const;

private:
	float heightMultiplier = 1.5f;
	float noiseMultiplier = 0.3f;
	float length;
	int resolution;
	std::vector<float> heightMap;

	float treesSpread = 3.f;
	int treesCount = 250;
	std::vector<glm::vec2> treesPositions;

	float maxHeight = 0;
	float minHeight = 0;
};
