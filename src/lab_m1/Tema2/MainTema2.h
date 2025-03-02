#pragma once

#include "components/simple_scene.h"
#include "core/gpu/mesh.h"
#include <iostream>
#include <vector>
#include "lab_m1/Tema2/Terrain.h"
#include "lab_m1/Tema2/Camera.h"
#include <cstdlib>
#include <string>
#include "lab_m1/Tema2/Math.h"

class MainTema2 : public gfxc::SimpleScene
{
public:
	void Init();

	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	float GetArrowAngle();
	bool CheckDroneCrystalCollision();
	bool CheckDroneBuildingCollision();
	bool CheckDroneTreeCollision();
	bool CheckDroneTerrainCollision();
	void CreateTerrainMesh();
	void PlaceBuildings();

	void LoadMesh(std::string name, std::string fileName);
	void LoadShader(std::string name, std::string vertexFileName, std::string fragFileName);
	void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) override;
	void DrawTerrain(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix);
	void DrawTrees(bool minimap);
	void DrawCrystal(bool minimap);
	void DrawBuildings();
	void DrawClouds();
	void DrawArrow(bool minimap);
	void DrawNoiseMesh(Mesh* mesh, glm::vec3 color, const glm::mat4& modelMatrix);
	void DrawDrone(float deltaTimeSeconds);
	void DrawSun();
	Mesh& Create3DBox(std::string name, glm::vec3 leftBottomCorner, float length, float width, float height, glm::vec3 color);
	void Draw(float deltaTimeSeconds, bool minimap);
	void PlaceCrystalRandomly();

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;
private:
	float score = 0;

	Camera camera;
	float cameraSpeed = 2.f;
	// Camera position and angle var used when ortho camera is active
	glm::vec3 targetCameraPosition = glm::vec3(0, 0, 0);
	float cameraAngleXOZ = 0;
	glm::mat4 camProjectionMatrix = glm::mat4(1);
	float fov = RADIANS(70);
	// Ortho camera
	float orthoSize = 10;
	float nearPlane = 0.01f, farPlane = 1000.0f;
	float orthoDistanceToTarget = 30.f;
	glm::vec2 minimapSize = glm::vec2(200.f, 112.5f);

	int terrainResolution = 100;
	float terrainLength = 80;
	TerrainGenerator terrain = TerrainGenerator(terrainResolution, terrainLength);

	// Drone vars
	float rotorsAngle = 0.f;
	// For drone collisions
	glm::vec3 droneSpeed = glm::vec3(3.5f, 1.f, 3.5f);
	glm::vec3 dronePreviousPosition;

	// Transport
	bool crystalCollected = false;
	glm::vec3 crystalPosition;
	float destinationRadius = 0.5f;
	glm::vec2 destinationPosition = glm::vec2(terrainLength / 2, terrainLength / 2);
	float crystalLength = 0.3f;
	float arrowAngle = 0.f;

	glm::vec3 buildingSize = glm::vec3(3.f, 1.f, 2.f);
	std::vector<glm::vec2> buildingsPositions;

	int buildingsCount = 20;
	glm::vec3 sunColor = glm::vec3(0.98, 0.529, 0);

	char* terrainMesh = "terrain";
	char* terrainShader = "terrain_shader";
	char* trunkShader = "tree_trunk_shader";
	char* leavesShader = "tree_leaves_shader";
	char* noiseShader = "noise_shader";
	char* droneChassisMesh = "drone_cube";
	char* droneRotorMesh = "drone_rotor";
	char* arrowMesh = "guiding_arrow";
	char* buildingMesh = "bunker";
	char* m_CrystalMesh = "box";
	char* trunkMesh = "trunk";
	char* leavesMesh = "leaves";
	char* cloudMesh = "cloud";
	char* sunMesh = "sun";
	char* sunMesh1 = "sun1";
	char* sunMesh2 = "sun2";
	char* cloudShader = "cloud_shader";
};
