#include "lab_m1/Tema2/MainTema2.h"

using namespace std;

void MainTema2::Init()
{
    camera.Set(glm::vec3(terrain.GetLength() / 2, 2, terrain.GetLength() / 2), glm::vec3(0, 1, 0), glm::normalize(glm::vec3(0.3, 1, 0)));
	dronePreviousPosition = camera.GetTargetPosition();

    // Load models
	LoadMesh("box", "box.obj");
	LoadMesh("trunk", "cylinder.obj");
	LoadMesh("leaves", "cone.obj");
	LoadMesh(arrowMesh, "direction_arrow.glb");

    // Load shaders
    LoadShader(terrainShader, "Terrain.Vertex.glsl", "Terrain.Frag.glsl");
    LoadShader(trunkShader, "Trunk.Vertex.glsl", "Trunk.Frag.glsl");
    LoadShader(leavesShader, "Leaves.Vertex.glsl", "Leaves.Frag.glsl");
    LoadShader(noiseShader, "Noise.Vertex.glsl", "Noise.Frag.glsl");
    LoadShader(cloudShader, "Cloud.Vertex.glsl", "Cloud.Frag.glsl");

    PlaceCrystalRandomly();
    PlaceBuildings();

    terrain.GenerateTerrain();
	terrain.GenerateTrees();
    CreateTerrainMesh();

    // Decoration
    Create3DBox(cloudMesh, glm::vec3(-0.5, -0.5, -0.5), 1, 1, 1, glm::vec3(1, 1, 1));
    Create3DBox(sunMesh, glm::vec3(-0.5, -0.5, -0.5), 1, 1, 1, sunColor);
    Create3DBox(sunMesh1, glm::vec3(-0.5, -0.5, -0.5), 1, 1, 1, sunColor * 1.5f);
    Create3DBox(sunMesh2, glm::vec3(-0.5, -0.5, -0.5), 1, 1, 1, sunColor * 1.7f);

    // Drone
	Create3DBox(droneChassisMesh, glm::vec3(-0.75, -0.75, -0.75), 1.5, 1.5, 1.5, glm::vec3(0.18, 0, 0.51));
    Create3DBox(buildingMesh, glm::vec3(0, 0, 0), 1, 1, 1, glm::vec3(0.2, 0.2, 0.2));
	Create3DBox(droneRotorMesh, glm::vec3(-3.5, -0.5, -0.5), 7, 1, 1, glm::vec3(0, 0.447, 0.671));
}

void MainTema2::LoadMesh(string name, string fileName)
{
    Mesh* mesh = new Mesh(name.c_str());
    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), fileName.c_str());
    meshes[mesh->GetMeshID()] = mesh;
}

void MainTema2::LoadShader(string name, string vertexFileName, string fragFileName)
{
    Shader* shader = new Shader(name);
    shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", vertexFileName), GL_VERTEX_SHADER);
    shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", fragFileName), GL_FRAGMENT_SHADER);
    shader->CreateAndLink();
    shaders[shader->GetName()] = shader;
}

float MainTema2::GetArrowAngle()
{
    glm::vec3 arrowDirection;
    float vertexDistance = terrain.GetLength() / (terrainResolution - 1);
    if (crystalCollected)
    {
        arrowDirection = glm::vec3(destinationPosition.x, 0, destinationPosition.y) - glm::vec3(camera.GetPosition().x, 0, camera.GetPosition().z);
    }
    else
        arrowDirection = glm::vec3(crystalPosition.x, 0, crystalPosition.z) - glm::vec3(camera.GetPosition().x, 0, camera.GetPosition().z);
    arrowDirection = glm::normalize(arrowDirection);

    glm::vec3 cameraForwardDirection = glm::normalize(glm::vec3(camera.GetForwardXOZ()));
    float x = glm::dot(cameraForwardDirection, arrowDirection);
    float y = glm::cross(cameraForwardDirection, arrowDirection).y;
    return atan2(y, x);
}

bool MainTema2::CheckDroneCrystalCollision()
{
    return glm::distance(camera.GetTargetPosition(), crystalPosition) < 1;
}

bool MainTema2::CheckDroneBuildingCollision()
{
    for (int i = 0; i < buildingsPositions.size(); i++)
    {
		glm::vec2 buildingPosXOZ = buildingsPositions[i];
        glm::vec3 buildingPos3D = glm::vec3(buildingPosXOZ.x, 0, buildingPosXOZ.y);
        glm::vec3 bunkerCorner0 = buildingPos3D;
        glm::vec3 bunkerCorner1 = buildingPos3D + buildingSize;

        // Check if drone inside
        glm::vec3 dronePosition = camera.GetTargetPosition();
        if (dronePosition.x >= bunkerCorner0.x && dronePosition.x <= bunkerCorner1.x && dronePosition.y >= bunkerCorner0.y && dronePosition.y <= bunkerCorner1.y &&
            dronePosition.z >= bunkerCorner0.z && dronePosition.z <= bunkerCorner1.z)
        {
            // Wow there, step back son
            return true;
        }
    }

    // Sector clear
    return false;
}

bool MainTema2::CheckDroneTreeCollision()
{
    // Loop through all trees to check collisions
    for (int i = 0; i < terrain.GetTreesPositions().size(); i++)
    {
		glm::vec2 treePosition = terrain.GetTreesPositions()[i];

        // Trunk
        float trunkRadius = 0.25f;

        glm::vec3 dronePosition = camera.GetTargetPosition();
        float dist = glm::distance(glm::vec2(dronePosition.x, dronePosition.z), glm::vec2(treePosition.x, treePosition.y));
        if (dist <= trunkRadius && dronePosition.y < 3.5)
        {
            // Get out of the trunk
            return true;
        }
    }

    // Sector clear
    return false;
}

bool MainTema2::CheckDroneTerrainCollision()
{
    glm::vec3 dronePosition = camera.GetTargetPosition();

	float height = terrain.GetHeight(dronePosition.x, dronePosition.z);

	if (dronePosition.y < height * 1.3)
	{
		// Get out of the ground
		return true;
	}

    return false;
}

void MainTema2::Update(float deltaTimeSeconds)
{
    if (CheckDroneTerrainCollision())
    {
        camera.MoveUpward(droneSpeed.y * deltaTimeSeconds * 2);
    }

    // Building collision
    if (CheckDroneBuildingCollision())
    {
		glm::vec3 pushDirection = dronePreviousPosition - camera.GetTargetPosition();
		glm::vec3 newPosition = camera.GetPosition() + pushDirection * droneSpeed.x;
		camera.SetPosition(newPosition);
    }

    // Tree collision detection
    if (CheckDroneTreeCollision())
    {
        glm::vec3 pushDirection = dronePreviousPosition - camera.GetTargetPosition();
        glm::vec3 newPosition = camera.GetPosition() + pushDirection * droneSpeed.x;
        camera.SetPosition(newPosition);
    }

    if (!crystalCollected && glm::distance(camera.GetTargetPosition(), crystalPosition) < crystalLength)
    {
		crystalCollected = true;
    }

    // Check crystal distance from destination
    if (glm::distance(glm::vec3(crystalPosition.x, 0, crystalPosition.z), glm::vec3(destinationPosition.x, 0, destinationPosition.y)) < 1)
    {
        score += 5;
        cout << "Crystal delivered! Score = " << score << endl;

        crystalCollected = false;
        PlaceCrystalRandomly();
    }

    // After being collected, follow drone
    if (crystalCollected)
        crystalPosition = camera.GetTargetPosition() - glm::vec3(0, 0.5, 0);

    // Perspective camera setup
    camProjectionMatrix = glm::perspective(fov, window->props.aspectRatio, nearPlane, farPlane);
    Draw(deltaTimeSeconds, false);

    // Save original camera state
    glm::vec3 tempPosition = camera.GetPosition();
    targetCameraPosition = camera.GetTargetPosition();
	cameraAngleXOZ = camera.GetXOZAngle();
	arrowAngle = GetArrowAngle();
    glm::vec3 tempTarget = camera.GetTargetPosition();
    glm::vec3 tempUp = camera.GetUpVector();

    // Orthographic camera setup for minimap
    camProjectionMatrix = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, nearPlane, farPlane);
    glm::vec3 orthoCameraPos = tempTarget + glm::vec3(0, orthoDistanceToTarget, 0);
    camera.Set(orthoCameraPos, tempTarget, glm::vec3(0, 0, -1));

    // Set viewport for minimap
    glClear(GL_DEPTH_BUFFER_BIT);
    glLineWidth(3);
    glViewport(window->GetResolution().x - minimapSize.x, window->GetResolution().y - minimapSize.y, minimapSize.x, minimapSize.y);

    // Draw minimap
    Draw(deltaTimeSeconds, true);

    //glm::rotate(glm::vec4(tempUp, 1.f), RADIANS(45), glm::vec3(1, 0, 0));
    camera.Set(tempPosition, tempTarget, tempUp);
}

void MainTema2::Draw(float deltaTimeSeconds, bool minimap)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Drone & decoration only on main camera
    if (!minimap)
    {
        DrawDrone(deltaTimeSeconds);
        DrawClouds();
        DrawSun();
    }

    DrawCrystal(minimap);
    DrawBuildings();
	DrawTerrain(meshes[terrainMesh], shaders[terrainShader], glm::mat4(1));
    DrawTrees(minimap);
    
    // On minimap draw bigger
    DrawArrow(minimap);
}

void MainTema2::PlaceCrystalRandomly()
{
    float randomX = float(rand()) / float(RAND_MAX) * terrain.GetLength();
    float randomZ = float(rand()) / float(RAND_MAX) * terrain.GetLength();

    crystalPosition = glm::vec3(randomX, terrain.GetMaxHeight() + 3, randomZ);
}

void MainTema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& transformMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camProjectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(transformMatrix));
    mesh->Render();
}

void MainTema2::DrawTerrain(Mesh* mesh, Shader* shader, const glm::mat4& transformMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camProjectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(transformMatrix));

	glUniform2f(glGetUniformLocation(shader->program, "dest_position"), destinationPosition.x, destinationPosition.y);
	glUniform1f(glGetUniformLocation(shader->program, "dest_radius"), destinationRadius);
	glUniform1f(glGetUniformLocation(shader->program, "noise_multiplier"), terrain.GetNoiseMultiplier());
	glUniform1f(glGetUniformLocation(shader->program, "height_multiplier"), terrain.GetHeightMultiplier());
    glUniform1f(glGetUniformLocation(shader->program, "time_elapsed"), (float)Engine::GetElapsedTime());

    // DEBUG
    float worldX = camera.GetTargetPosition().x;
	float worldZ = camera.GetTargetPosition().z;
    float length = terrain.GetLength();
    int resolution = terrainResolution;
    worldX = Clamp(worldX, 0.f, length);
    worldZ = Clamp(worldZ, 0.f, length);

    float vertexDistance = length / float(resolution - 1);
    int x0 = worldX / vertexDistance;
    int z0 = worldZ / vertexDistance;
    int x1 = ClampMax(x0 + 1, resolution - 1);
    int z1 = ClampMax(z0 + 1, resolution - 1);

    glUniform1f(glGetUniformLocation(shader->program, "x0"), x0 * vertexDistance);
    glUniform1f(glGetUniformLocation(shader->program, "x1"), x1 * vertexDistance);
    glUniform1f(glGetUniformLocation(shader->program, "z0"), z0 * vertexDistance);
    glUniform1f(glGetUniformLocation(shader->program, "z1"), z1 * vertexDistance);
    
    mesh->Render();
}

void MainTema2::DrawTrees(bool minimap)
{
    for (int i = 0; i < terrain.GetTreesPositions().size(); i++)
    {
		glm::vec2 posXOZ = terrain.GetTreesPositions()[i];
        if (minimap == true)
        {
			glm::vec2 dronePositionXOZ = glm::vec2(camera.GetTargetPosition().x, camera.GetTargetPosition().z);
            float distTreeToDrone = glm::distance(posXOZ, dronePositionXOZ);
            // For ortho camera, draw only whats close
			if (distTreeToDrone > orthoSize * 2)
				continue;
        }

        glm::vec3 pos3D = glm::vec3(posXOZ.x, 0, posXOZ.y);

        glm::mat4 trunkTransform = glm::mat4(1);
        trunkTransform = glm::translate(trunkTransform, pos3D);
        trunkTransform = glm::scale(trunkTransform, glm::vec3(0.1f, 0.3f, 0.1f));
        RenderMesh(meshes[trunkMesh], shaders[trunkShader], trunkTransform);

        // Scale down by 0.7 for every cone
        glm::mat4 leavesTransform = glm::mat4(1);
        leavesTransform = glm::translate(leavesTransform, pos3D);
        leavesTransform = glm::translate(leavesTransform, glm::vec3(0, 2, 0));
        leavesTransform = glm::scale(leavesTransform, glm::vec3(0.7f));
        RenderMesh(meshes[leavesMesh], shaders[leavesShader], leavesTransform);

        leavesTransform = glm::translate(leavesTransform, glm::vec3(0, 1, 0));
        leavesTransform = glm::scale(leavesTransform, glm::vec3(0.7f));
        RenderMesh(meshes[leavesMesh], shaders[leavesShader], leavesTransform);

        leavesTransform = glm::translate(leavesTransform, glm::vec3(0, 1, 0));
        leavesTransform = glm::scale(leavesTransform, glm::vec3(0.7f));
        RenderMesh(meshes[leavesMesh], shaders[leavesShader], leavesTransform);
    }
}

void MainTema2::DrawCrystal(bool minimap)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    float scale;
    glm::vec3 position;
    if (minimap)
    {
        scale = crystalLength * 3;
		position = crystalPosition + glm::vec3(0, 5, 0);
    }
    else
    {
		scale = crystalLength;
		position = crystalPosition;
    }

    glm::mat4 transformMatrix = glm::mat4(1);
    transformMatrix = glm::translate(transformMatrix, position);
    transformMatrix = glm::rotate(transformMatrix, (float)Engine::GetElapsedTime(), glm::normalize(glm::vec3(0, 1, 1)));
    transformMatrix = glm::scale(transformMatrix, glm::vec3(scale));
    RenderMesh(meshes["box"], shaders["VertexNormal"], transformMatrix);

    transformMatrix = glm::mat4(1);
    transformMatrix = glm::translate(transformMatrix, position);
    transformMatrix = glm::rotate(transformMatrix, (float)Engine::GetElapsedTime(), glm::normalize(glm::vec3(1, 0, 1)));
    transformMatrix = glm::scale(transformMatrix, glm::vec3(scale));
    RenderMesh(meshes["box"], shaders["VertexNormal"], transformMatrix);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void MainTema2::DrawBuildings()
{
    float resolutionTerrain = terrainResolution;
    for (int i = 0; i < buildingsPositions.size(); i++)
    {
        glm::vec2 pos = buildingsPositions[i];
        glm::vec3 pos3D = glm::vec3(pos.x, (terrain.GetMaxHeight() + terrain.GetMinHeight()) / 2, pos.y);

        glm::mat4 transformMatrix = glm::mat4(1);
        transformMatrix = glm::translate(transformMatrix, pos3D);
        transformMatrix = glm::scale(transformMatrix, buildingSize);
        RenderMesh(meshes[buildingMesh], shaders["VertexColor"], transformMatrix);
    }
}

void MainTema2::DrawClouds()
{
	for (int i = 0; i < terrain.GetTreesPositions().size(); i++)
	{
		glm::vec2 pos = terrain.GetTreesPositions()[i];
		glm::vec3 pos3D = glm::vec3(pos.x, 20, pos.y);

		glm::vec3 randomScale = glm::vec3(Random(pos) * 2, Random(pos), Random(pos) * 2);

		glm::mat4 transformMatrix = glm::mat4(1);
		transformMatrix = glm::translate(transformMatrix, pos3D);
		transformMatrix = glm::scale(transformMatrix, randomScale);
		RenderMesh(meshes[cloudMesh], shaders[cloudShader], transformMatrix);
	}
}

void MainTema2::DrawArrow(bool minimap)
{
    if (!minimap)
    {
        glm::vec3 translatePosition = camera.GetTargetPosition() - 0.45f * glm::normalize(camera.GetForwardXOZ());
        glm::mat4 transformMatrix = glm::mat4(1);
        transformMatrix = glm::translate(transformMatrix, translatePosition);
        transformMatrix = glm::rotate(transformMatrix, GetArrowAngle() + camera.GetXOZAngle() + RADIANS(90), glm::vec3(0, 1, 0));
        transformMatrix = glm::rotate(transformMatrix, RADIANS(90), glm::vec3(1, 0, 0));
        transformMatrix = glm::scale(transformMatrix, glm::vec3(0.07f));
        DrawNoiseMesh(meshes[arrowMesh], glm::vec3(0.98, 0.529, 0), transformMatrix);
	}
	else
	{
        glm::vec3 translatePosition = targetCameraPosition + glm::vec3(0, 2, 0);
        glm::mat4 transformMatrix = glm::mat4(1);
        transformMatrix = glm::translate(transformMatrix, translatePosition);
        transformMatrix = glm::rotate(transformMatrix, arrowAngle + cameraAngleXOZ + RADIANS(90), glm::vec3(0, 1, 0));
        transformMatrix = glm::rotate(transformMatrix, RADIANS(90), glm::vec3(1, 0, 0));
        transformMatrix = glm::scale(transformMatrix, glm::vec3(1.5f));
        DrawNoiseMesh(meshes[arrowMesh], glm::vec3(0.98, 0.529, 0), transformMatrix);
	}
}

void MainTema2::DrawNoiseMesh(Mesh* mesh, glm::vec3 color, const glm::mat4& modelMatrix)
{
    Shader* shader = shaders[noiseShader];
    if (!mesh || !shader || !shader->program)
        return;

    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camProjectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	glUniform3f(glGetUniformLocation(shader->program, "object_color"), color.x, color.y, color.z);
    mesh->Render();
}

void MainTema2::DrawDrone(float deltaTimeSeconds)
{
    rotorsAngle = rotorsAngle + 25 * deltaTimeSeconds;

    // chassis
    glm::mat4 transformMatrix = glm::mat4(1);
    glm::vec3 translation = camera.GetTargetPosition();
    transformMatrix = glm::translate(transformMatrix, translation);
    transformMatrix = glm::rotate(transformMatrix, -RADIANS(45), glm::vec3(0, 1, 0));
    transformMatrix = glm::scale(transformMatrix, glm::vec3(0.02f, 0.02f, 0.3f));
    DrawNoiseMesh(meshes[droneChassisMesh], glm::vec3(0.18, 0, 0.51), transformMatrix);
        
    transformMatrix = glm::mat4(1);
    translation = camera.GetTargetPosition();
    transformMatrix = glm::translate(transformMatrix, translation);
    transformMatrix = glm::rotate(transformMatrix, RADIANS(45), glm::vec3(0, 1, 0));
    transformMatrix = glm::scale(transformMatrix, glm::vec3(0.02f, 0.02f, 0.3f));
    DrawNoiseMesh(meshes[droneChassisMesh], glm::vec3(0.18, 0, 0.51), transformMatrix);

    // rotor support
    for (int dx = -1; dx < 3; dx += 2)
    {
        for (int dz = -1; dz < 3; dz += 2)
        {
            float posX = dx * 0.22 * cos(RADIANS(45));
            float posZ = dz * 0.22 * sin(RADIANS(45));
            glm::vec3 rotationOffset = glm::vec3(posX, 0.02, posZ);

            glm::mat4 transformMatrix = glm::mat4(1);
            glm::vec3 translation = camera.GetTargetPosition() + glm::vec3(0, 2 * 0.02, 0);
            transformMatrix = glm::translate(transformMatrix, translation);
            // Rotate around the center of the drone
            transformMatrix = glm::translate(transformMatrix, -rotationOffset);
            transformMatrix = glm::scale(transformMatrix, glm::vec3(-0.02f));
            DrawNoiseMesh(meshes[droneChassisMesh], glm::vec3(0.18, 0, 0.51), transformMatrix);
        }
    }

    // rotors
    for (int dz = -1; dz < 3; dz += 2)
    {
        for (int dx = -1; dx < 3; dx += 2)
        {
            glm::vec3 rotationDelta = { dx * 0.22 * cos(RADIANS(45)), 0.02, dz * 0.22 * sin(RADIANS(45)) };

            glm::mat4 transformMatrix = glm::mat4(1);
            glm::vec3 translation = camera.GetTargetPosition() + glm::vec3(0, 2 * 0.02 + 0.015, 0);
            transformMatrix = glm::translate(transformMatrix, translation);
            // Rotate according to Drone
            transformMatrix = glm::translate(transformMatrix, -rotationDelta);
            // Rotate
            transformMatrix = glm::rotate(transformMatrix, rotorsAngle * dx * dz, glm::vec3(0, 1, 0));
            transformMatrix = glm::scale(transformMatrix, glm::vec3(0.02f, 0.01f, 0.02f));
            DrawNoiseMesh(meshes[droneRotorMesh], glm::vec3(0, 0.447, 0.671), transformMatrix);
        }
    }
}

void MainTema2::DrawSun()
{
	glm::mat4 transformMatrix = glm::mat4(1);
	transformMatrix = glm::translate(transformMatrix, glm::vec3(terrain.GetLength() / 2, 500, terrain.GetLength() / 2));
	transformMatrix = glm::scale(transformMatrix, glm::vec3(30.f, 1.f, 30.f));
	RenderMesh(meshes[sunMesh], shaders["VertexColor"], transformMatrix);
    transformMatrix = glm::translate(transformMatrix, glm::vec3(0, -10, 0));
    transformMatrix = glm::scale(transformMatrix, glm::vec3(0.7f, 1.f, 0.7));
	RenderMesh(meshes[sunMesh1], shaders["VertexColor"], transformMatrix);
    transformMatrix = glm::translate(transformMatrix, glm::vec3(0, -10, 0));
    transformMatrix = glm::scale(transformMatrix, glm::vec3(0.7f, 1.f, 0.7));
	RenderMesh(meshes[sunMesh2], shaders["VertexColor"], transformMatrix);
}

Mesh& MainTema2::Create3DBox(string name, glm::vec3 leftBottomCorner, float length, float width, float height, glm::vec3 color)
{
    glm::vec3 corner = leftBottomCorner;
	glm::vec3 lengthDir = glm::vec3(length, 0, 0);
	glm::vec3 widthDir = glm::vec3(0, 0, width);
    glm::vec3 heightDir = glm::vec3(0, height, 0);

    vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + lengthDir, color),
        VertexFormat(corner + widthDir, color),
        VertexFormat(corner + lengthDir + widthDir, color),
        VertexFormat(corner + heightDir, color),
        VertexFormat(corner + lengthDir + heightDir , color),
        VertexFormat(corner + widthDir + heightDir, color),
        VertexFormat(corner + lengthDir + widthDir + heightDir, color)
    };

    Mesh* rectangle = new Mesh(name);
    vector<unsigned int> indices =
    {
        0, 1, 2,
        1, 3, 2,

        4, 6, 5,  
        5, 6, 7,

        0, 1, 4,
        1, 5, 4,

        2, 3, 6,
        3, 7, 6,

        0, 2, 4,
        2, 6, 4,

        1, 3, 5,
        3, 7, 5
    };

    indices.push_back(0);
    indices.push_back(2);

    rectangle->InitFromData(vertices, indices);
	AddMeshToList(rectangle);
    return *meshes[name];
}

void MainTema2::OnInputUpdate(float deltaTime, int mods)
{
    dronePreviousPosition = camera.GetTargetPosition();

    // Camera controls
    if (window->KeyHold(GLFW_KEY_UP))
    {
        camera.RotateThirdPerson_OX(-deltaTime * cameraSpeed);
    }

    if (window->KeyHold(GLFW_KEY_DOWN))
    {
        camera.RotateThirdPerson_OX(deltaTime * cameraSpeed);
    }

    if (window->KeyHold(GLFW_KEY_LEFT))
    {
        camera.RotateThirdPerson_OY(deltaTime * cameraSpeed);
    }

    if (window->KeyHold(GLFW_KEY_RIGHT))
    {
        camera.RotateThirdPerson_OY(-deltaTime * cameraSpeed);
    }

    // Drone controls
    if (window->KeyHold(GLFW_KEY_W))
    {
        camera.MoveForward(deltaTime * droneSpeed.z);
    }

    if (window->KeyHold(GLFW_KEY_S))
    {
        camera.MoveForward(-deltaTime * droneSpeed.z);
    }
    if (window->KeyHold(GLFW_KEY_A))
    {
        camera.MoveRight(-deltaTime * droneSpeed.x);
    }
    if (window->KeyHold(GLFW_KEY_D))
    {
        camera.MoveRight(deltaTime * droneSpeed.x);
    }
    if (window->KeyHold(GLFW_KEY_LEFT_SHIFT))
    {
        camera.MoveUpward(-deltaTime * droneSpeed.y);
    }
    if (window->KeyHold(GLFW_KEY_SPACE))
    {
        camera.MoveUpward(deltaTime * droneSpeed.y);
    }
}

void MainTema2::FrameStart()
{
    glClearColor(0, 0.588, 0.878, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}

void MainTema2::CreateTerrainMesh()
{
	vector<VertexFormat> vertices;
	int resolution = terrainResolution;
	float length = terrain.GetLength();
    float vertexDistance = length / (resolution - 1);
	for (int z = 0; z < resolution; z++) 
    {
		for (int x = 0; x < resolution; x++) 
        {
			vertices.push_back(VertexFormat(glm::vec3(x, 0, z) * vertexDistance, glm::vec3(0.5)));
		}
	}

	vector<unsigned int> indices;
    for (int i = 0; i < resolution - 1; i++)
    {
        for (int j = 0; j < resolution - 1; j++)
        {
            indices.push_back(i * resolution + j);
            indices.push_back((i + 1) * resolution + j);
            indices.push_back(i * resolution + j + 1);

            indices.push_back((i + 1) * resolution + j);
            indices.push_back((i + 1) * resolution + j + 1);
            indices.push_back(i * resolution + j + 1);
        }
    }

    Mesh* mesh;
	mesh = new Mesh(terrainMesh);
    mesh->InitFromData(vertices, indices);
	meshes[terrainMesh] = mesh;
}

void MainTema2::PlaceBuildings()
{
    buildingsPositions.clear();

    int count = 0;
    while (count < buildingsCount)
    {
		float randomX = float(rand()) / float(RAND_MAX) * terrain.GetLength();
		float randomZ = float(rand()) / float(RAND_MAX) * terrain.GetLength();

		glm::vec2 pos = glm::vec2(randomX, randomZ);
		buildingsPositions.push_back(pos);
		count++;
    }
}

void MainTema2::FrameEnd()
{
}

void MainTema2::OnKeyPress(int key, int mods)
{
}

void MainTema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
}


void MainTema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
}

void MainTema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}

void MainTema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void MainTema2::OnWindowResize(int width, int height)
{
}
