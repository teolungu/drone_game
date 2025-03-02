#include "Terrain.h"
#include <iostream>

TerrainGenerator::TerrainGenerator(int resolution, float length)
{
	heightMap.resize(resolution * resolution);
	this->resolution = resolution;
	this->length = length;
}

std::vector<float> TerrainGenerator::GetHeightMap() const
{
	return heightMap;
}

std::vector<glm::vec2> TerrainGenerator::GetTreesPositions() const
{
	return treesPositions;
}

float TerrainGenerator::GetLength() const
{
	return length;
}

float TerrainGenerator::GetNoiseMultiplier() const
{
	return noiseMultiplier;
}

float TerrainGenerator::GetMaxHeight() const
{
	return maxHeight;
}

float TerrainGenerator::GetMinHeight() const
{
	return minHeight;
}

float TerrainGenerator::GetTreesSpread() const
{
	return treesSpread;
}

float TerrainGenerator::GetHeightMultiplier() const
{
	return heightMultiplier;
}

void TerrainGenerator::SetHeight(int x, int z, float height)
{
	heightMap[z * resolution + x] = height;

	maxHeight = std::max(maxHeight, height);
	minHeight = std::min(minHeight, height);
}

float TerrainGenerator::GetHeight(float worldX, float worldZ) const
{
	worldX = Clamp(worldX, 0.f, length);
	worldZ = Clamp(worldZ, 0.f, length);

	float vertexDistance = length / float(resolution - 1);
	int x0 = worldX / vertexDistance;
	int z0 = worldZ / vertexDistance;
	int x1 = ClampMax(x0 + 1, resolution - 1);
	int z1 = ClampMax(z0 + 1, resolution - 1);

	float heights[4] = 
	{
		GetHeight(x0, z0),
		GetHeight(x1, z0),
		GetHeight(x0, z1),
		GetHeight(x1, z1)
	};

	return Mix(heights[0], heights[1], heights[2], heights[3], glm::vec2(worldX, worldZ));
}

float TerrainGenerator::GetHeight(int x, int z) const
{
	return heightMap[z * resolution + x];
}

void TerrainGenerator::GenerateTerrain()
{
	float vertexDistance = length / float(resolution - 1);

	maxHeight = -999;
	minHeight = 999;
	for (int z = 0; z < resolution; z++)
	{
		for (int x = 0; x < resolution; x++)
		{
			float height = Noise(glm::vec2(x, z) * vertexDistance * noiseMultiplier) * heightMultiplier;

			SetHeight(x, z, height);
		}
	}
}

void TerrainGenerator::GenerateTrees()
{
	treesPositions.clear();

	float vertexDistance = length / float(resolution - 1);
	int count = 0;
	while (count < treesCount)
	{
		float x = float(rand()) / RAND_MAX * length;
		float z = float(rand()) / RAND_MAX * length;

		// is the tree too close to another tree?
		bool cramped = false;
		for (glm::vec2 otherTree : treesPositions)
		{
			if (glm::distance(glm::vec2(x, z), otherTree) < treesSpread)
			{
				cramped = true;
				break;
			}
		}
		if (!cramped)
		{
			count++;
			treesPositions.push_back(glm::vec2(x, z));
		}
	}
}
