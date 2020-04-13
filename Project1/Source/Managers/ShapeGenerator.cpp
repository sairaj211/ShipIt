#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "ShapeGenerator.h"
#include "Mesh.h"

const int NUM_SUBDIVISIONS = 1;

ShapeGenerator* ShapeGenerator::mInstance = nullptr;

ShapeGenerator::ShapeGenerator()
{

}

ShapeGenerator::~ShapeGenerator()
{
	// THIS MIGHT BE NEEDED LATER
	/*for (auto& element : mShapes) {
		delete element.second;
	}*/
	mShapes.clear();
}

ShapeGenerator* ShapeGenerator::GetInstance()
{
	if (!mInstance)
		mInstance = new ShapeGenerator();

	return mInstance;
}

void ShapeGenerator::Init()
{
	LOG_INFO("ShapeGenerator Init");
	mShapes["Sphere"] = GenerateSphere();
	mShapes["Quad"] = GenerateQuad();
	mShapes["Cube"] = GenerateCube();
	mShapes["Tetrahedron"] = GenerateTetrahedron();
	mShapes["Octahedron"] = GenerateOctahedron();
	mShapes["Icosahedron"] = GenerateIcosahedron();
	mShapes["Capsule"] = GenerateCapsule();
	mShapes["InstancedCube"] = GenerateCubeInstanced();
}

bool ShapeGenerator::IsShape(std::string a_type)
{
	return mShapes.find(a_type) != mShapes.end();
}

Mesh* ShapeGenerator::GetShapeMesh(std::string a_type)
{
	if (IsShape(a_type))
		return mShapes[a_type];

	return nullptr;
}

std::vector<std::string> ShapeGenerator::GetShapeTypes()
{
	std::vector<std::string> shapeTypes;

	// Push the type of each shape into the vector
	for (auto& element : mShapes)
		shapeTypes.push_back(element.first);

	return shapeTypes;
}

void ShapeGenerator::Cleanup()
{
	delete mInstance;
	mInstance = nullptr;
}

Mesh* ShapeGenerator::GenerateSphere()
{
	Mesh* pMesh = new Mesh();
	pMesh->mMeshName = "Sphere";

	// Generate Sphere Mesh
	int numSubdivisions = 32;
	float d = glm::two_pi<float>() / float(numSubdivisions * 2);
	for (int i = 0; i <= numSubdivisions * 2; ++i) {
		float s = (i * glm::two_pi<float>()) / float(numSubdivisions * 2);
		for (int j = 0; j <= numSubdivisions; ++j) {
			float t = (j * glm::pi<float>()) / float(numSubdivisions);

			float x = cosf(s) * sinf(t);
			float y = sinf(s) * sinf(t);
			float z = cosf(t);

			// Compute Vertex
			Vertex vertex;
			vertex.position = glm::vec3(x, y, z);
			vertex.normal = glm::vec3(x, y, z);
			vertex.textureCoordinate = glm::vec2(s / glm::two_pi<float>(), t / glm::pi<float>());
			pMesh->mVertices.push_back(vertex);

			// Add Indices
			if (i > 0 && j > 0) {
				// Face 1
				pMesh->mIndices.push_back((i - 1) * (numSubdivisions + 1) + (j - 1));
				pMesh->mIndices.push_back((i - 1) * (numSubdivisions + 1) + j);
				pMesh->mIndices.push_back(i * (numSubdivisions + 1) + j);

				// Face 2
				pMesh->mIndices.push_back((i - 1) * (numSubdivisions + 1) + (j - 1));
				pMesh->mIndices.push_back(i * (numSubdivisions + 1) + j);
				pMesh->mIndices.push_back(i * (numSubdivisions + 1) + (j - 1));
			}
		}
	}

	// Calculate Vertex Tangents 
	CalculateTangents(pMesh);

	// Set min and max vertex position values for the mesh
	SetMinMax(pMesh);

	return pMesh;
}

Mesh* ShapeGenerator::GenerateQuad()
{
	Mesh* pMesh = new Mesh();
	pMesh->mMeshName = "Quad";

	// Compute Vertices for first triangle
	Vertex vertices[4];
	vertices[0].position = glm::vec3(-1.0f, -1.0f, 0.0f);
	vertices[0].textureCoordinate = glm::vec2(0.0f, 1.0f);
	vertices[0].normal = glm::vec3(0.0f, 0.0f, 1.0f);
	pMesh->mVertices.push_back(vertices[0]);

	vertices[1].position = glm::vec3(-1.0f, 1.0f, 0.0f);
	vertices[1].textureCoordinate = glm::vec2(0.0f, 0.0f);
	vertices[1].normal = glm::vec3(0.0f, 0.0f, 1.0f);
	pMesh->mVertices.push_back(vertices[1]);

	vertices[2].position = glm::vec3(1.0f, 1.0f, 0.0f);
	vertices[2].textureCoordinate = glm::vec2(1.0f, 0.0f);
	vertices[2].normal = glm::vec3(0.0f, 0.0f, 1.0f);
	pMesh->mVertices.push_back(vertices[2]);

	vertices[3].position = glm::vec3(1.0f, -1.0f, 0.0f);
	vertices[3].textureCoordinate = glm::vec2(1.0f, 1.0f);
	vertices[3].normal = glm::vec3(0.0f, 0.0f, 1.0f);
	pMesh->mVertices.push_back(vertices[3]);

	// Add Indices
	pMesh->mIndices.push_back(0);
	pMesh->mIndices.push_back(1);
	pMesh->mIndices.push_back(2);

	pMesh->mIndices.push_back(0);
	pMesh->mIndices.push_back(2);
	pMesh->mIndices.push_back(3);

	// Calculate Vertex Tangents 
	CalculateTangents(pMesh);

	// Set min and max vertex position values for the mesh
	SetMinMax(pMesh);

	return pMesh;
}

Mesh* ShapeGenerator::GenerateCube()
{
	Mesh* pMesh = new Mesh();
	pMesh->mMeshName = "Cube";

	// Generate the 6 faces of the cube
	glm::mat4 translation(1.0f);
	glm::mat4 rotation(1.0f);
	GenerateCubeFace(pMesh, glm::translate(translation, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(rotation, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f)));
	GenerateCubeFace(pMesh, glm::translate(translation, glm::vec3(0.0f, -1.0f, 0.0f)) * glm::rotate(rotation, glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)));
	GenerateCubeFace(pMesh, glm::translate(translation, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::rotate(rotation, glm::pi<float>() / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f)));
	GenerateCubeFace(pMesh, glm::translate(translation, glm::vec3(0.0f, 0.0f, -1.0f)) * glm::rotate(rotation, -glm::pi<float>() / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f)));
	GenerateCubeFace(pMesh, glm::translate(translation, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(rotation, -glm::pi<float>() / 2.0f, glm::vec3(0.0f, 0.0f, 1.0f)));
	GenerateCubeFace(pMesh, glm::translate(translation, glm::vec3(-1.0f, 0.0f, 0.0f)) * glm::rotate(rotation, glm::pi<float>() / 2.0f, glm::vec3(0.0f, 0.0f, 1.0f)));

	// Add Indices
	for (int i = 0; i < pMesh->mVertices.size(); ++i)
		pMesh->mIndices.push_back(i);

	// Calculate Vertex Tangents 
	CalculateTangents(pMesh);

	// Set min and max vertex position values for the mesh
	SetMinMax(pMesh);

	return pMesh;
}

Mesh* ShapeGenerator::GenerateCubeInstanced()
{
	Mesh* pMesh = new Mesh();
	pMesh->mMeshName = "InsatncedCube";

	float vertices[] = {
		// Position			   TexCoords
		 -0.5f, -0.5f, -0.5f,	0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,	1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,	1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,	1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,	0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,	1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,	1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,	1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,	0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,	1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,	0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,	0.0f, 1.0f
	};

	// Add Indices
	/*for (int i = 0; i < pMesh->mVertices.size(); ++i)
		pMesh->mIndices.push_back(i);*/

	// Calculate Vertex Tangents 
	//CalculateTangents(pMesh);

	// Set min and max vertex position values for the mesh
	//SetMinMax(pMesh);

	//glGenVertexArrays(1, &pMesh->mVAO);
	//glGenBuffers(1, &pMesh->mVBO);
	// fill buffer
	glBindVertexArray(pMesh->mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, pMesh->mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// link vertex attributes
	{
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

		//Instanced VBO

		glGenBuffers(1, &pMesh->mInsatnceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, pMesh->mInsatnceVBO);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 20 * sizeof(float), (void*)(0 * sizeof(float)));
		glVertexAttribDivisor(2, 1);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 20 * sizeof(float), (void*)(4 * sizeof(float)));
		glVertexAttribDivisor(3, 1);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 20 * sizeof(float), (void*)(8 * sizeof(float)));
		glVertexAttribDivisor(4, 1);

		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 20 * sizeof(float), (void*)(12 * sizeof(float)));
		glVertexAttribDivisor(5, 1);

		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 20 * sizeof(float), (void*)(16 * sizeof(float)));
		glVertexAttribDivisor(6, 1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);

	return pMesh;
}

Mesh* ShapeGenerator::GenerateTetrahedron()
{
	Mesh* pMesh = new Mesh();
	pMesh->mMeshName = "Tetrahedron";

	std::vector<glm::vec3> vertexPositions;
	vertexPositions.push_back(glm::vec3(sqrt(8.0f / 9.0f), 0.0f, -1.0f / 3.0f));
	vertexPositions.push_back(glm::vec3(-1.0f * sqrt(2.0f / 9.0f), sqrt(2.0f / 3.0f), -1.0f / 3.0f));
	vertexPositions.push_back(glm::vec3(-1.0f * sqrt(2.0f / 9.0f), -1.0f * sqrt(2.0f / 3.0f), -1.0f / 3.0f));
	vertexPositions.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

	// For each face
	for (int i = 0; i < 4; ++i) {
		// Compute Vertices
		Vertex vertices[3];
		for (int j = 0; j < 3; ++j) {
			int positionIndex = i + 1 + j;
			if (positionIndex >= 4) {
				positionIndex = positionIndex - 4;
			}
			vertices[j].position = vertexPositions[positionIndex];
			vertices[j].textureCoordinate = glm::vec2(0.0f, 0.0f);
		}
		
		// Compute Normal
		glm::vec3 normal = glm::cross(vertices[0].position - vertices[2].position, vertices[1].position - vertices[2].position);
		float dot = glm::dot(normal, vertices[0].position);
		if (dot < 0.0f)
			normal *= -1.0f;

		vertices[0].normal = normal;
		vertices[1].normal = normal;
		vertices[2].normal = normal;

		if (dot < 0.0f) {
			pMesh->mVertices.push_back(vertices[2]);
			pMesh->mVertices.push_back(vertices[1]);
			pMesh->mVertices.push_back(vertices[0]);
		}
		else {
			pMesh->mVertices.push_back(vertices[0]);
			pMesh->mVertices.push_back(vertices[1]);
			pMesh->mVertices.push_back(vertices[2]);
		}
	}

	// Add Indices
	for (int i = 0; i < pMesh->mVertices.size(); ++i)
		pMesh->mIndices.push_back(i);

	// Calculate Vertex Tangents 
	CalculateTangents(pMesh);

	// Set min and max vertex position values for the mesh
	SetMinMax(pMesh);

	return pMesh;
}

Mesh* ShapeGenerator::GenerateOctahedron()
{
	Mesh* pMesh = new Mesh();
	pMesh->mMeshName = "Octahedron";
	
	glm::mat4 rotation(1.0f);
	float angle = 0.0f;
	// For each face
	for (int i = 0; i < 8; ++i) {
		// Compute Vertices
		Vertex vertices[3];
		vertices[0].position = glm::rotate(rotation, angle, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		vertices[0].textureCoordinate = glm::vec2(0.0f, 0.0f);

		vertices[1].position = glm::rotate(rotation, angle, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		vertices[1].textureCoordinate = glm::vec2(0.0f, 0.0f);

		vertices[2].position = glm::vec3(0.0f, pow(-1.0f, i / 4), 0.0f);
		vertices[2].textureCoordinate = glm::vec2(0.0f, 0.0f);

		// Compute Normal
		glm::vec3 normal = glm::cross(vertices[0].position - vertices[2].position, vertices[1].position - vertices[2].position);
		float dot = glm::dot(normal, vertices[0].position);
		if (dot < 0.0f)
			normal *= -1.0f;

		vertices[0].normal = normal;
		vertices[1].normal = normal;
		vertices[2].normal = normal;

		if (dot < 0.0f) {
			pMesh->mVertices.push_back(vertices[2]);
			pMesh->mVertices.push_back(vertices[1]);
			pMesh->mVertices.push_back(vertices[0]);
		}
		else {
			pMesh->mVertices.push_back(vertices[0]);
			pMesh->mVertices.push_back(vertices[1]);
			pMesh->mVertices.push_back(vertices[2]);
		}

		angle += glm::pi<float>() / 2.0f;
	}

	// Add Indices
	for (int i = 0; i < pMesh->mVertices.size(); ++i)
		pMesh->mIndices.push_back(i);

	// Calculate Vertex Tangents 
	CalculateTangents(pMesh);

	// Set min and max vertex position values for the mesh
	SetMinMax(pMesh);

	return pMesh;
}

Mesh* ShapeGenerator::GenerateIcosahedron()
{
	Mesh* pMesh = new Mesh();
	pMesh->mMeshName = "Icosahedron";

	float goldenRatio = (1.0f + sqrt(5.0f)) / 2.0f;

	// Add the twenty faces of the Icosahedron
	GenerateIcosahedronFace(pMesh, glm::vec3(1.0f, -goldenRatio, 0.0f), glm::vec3(0.0f, -1.0f, goldenRatio), glm::vec3(-1.0f, -goldenRatio, 0.0f));
	GenerateIcosahedronFace(pMesh, glm::vec3(goldenRatio, 0.0f, 1.0f), glm::vec3(1.0f, -goldenRatio, 0.0f), glm::vec3(0.0f, -1.0f, goldenRatio));
	GenerateIcosahedronFace(pMesh, glm::vec3(0.0f, 1.0f, goldenRatio), glm::vec3(goldenRatio, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, goldenRatio));
	GenerateIcosahedronFace(pMesh, glm::vec3(0.0f, 1.0f, goldenRatio), glm::vec3(-goldenRatio, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, goldenRatio));

	GenerateIcosahedronFace(pMesh, glm::vec3(-goldenRatio, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, goldenRatio), glm::vec3(-1.0f, -goldenRatio, 0.0f));
	GenerateIcosahedronFace(pMesh, glm::vec3(0.0f, 1.0f, goldenRatio), glm::vec3(goldenRatio, 0.0f, 1.0f), glm::vec3(1.0f, goldenRatio, 0.0f));
	GenerateIcosahedronFace(pMesh, glm::vec3(0.0f, 1.0f, goldenRatio), glm::vec3(1.0f, goldenRatio, 0.0f), glm::vec3(-1.0f, goldenRatio, 0.0f));
	GenerateIcosahedronFace(pMesh, glm::vec3(0.0f, 1.0f, goldenRatio), glm::vec3(-goldenRatio, 0.0f, 1.0f), glm::vec3(-1.0f, goldenRatio, 0.0f));

	GenerateIcosahedronFace(pMesh, glm::vec3(goldenRatio, 0.0f, 1.0f), glm::vec3(1.0f, -goldenRatio, 0.0f), glm::vec3(goldenRatio, 0.0f, -1.0f));
	GenerateIcosahedronFace(pMesh, glm::vec3(goldenRatio, 0.0f, 1.0f), glm::vec3(1.0f, goldenRatio, 0.0f), glm::vec3(goldenRatio, 0.0f, -1.0f));
	GenerateIcosahedronFace(pMesh, glm::vec3(-goldenRatio, 0.0f, 1.0f), glm::vec3(-1.0f, goldenRatio, 0.0f), glm::vec3(-goldenRatio, 0.0f, -1.0f));
	GenerateIcosahedronFace(pMesh, glm::vec3(-goldenRatio, 0.0f, 1.0f), glm::vec3(-goldenRatio, 0.0f, -1.0f), glm::vec3(-1.0f, -goldenRatio, 0.0f));

	GenerateIcosahedronFace(pMesh, glm::vec3(1.0f, -goldenRatio, 0.0f), glm::vec3(0.0f, -1.0f, -goldenRatio), glm::vec3(-1.0f, -goldenRatio, 0.0f));
	GenerateIcosahedronFace(pMesh, glm::vec3(0.0f, -1.0f, -goldenRatio), glm::vec3(-goldenRatio, 0.0f, -1.0f), glm::vec3(-1.0f, -goldenRatio, 0.0f));
	GenerateIcosahedronFace(pMesh, glm::vec3(0.0f, 1.0f, -goldenRatio), glm::vec3(0.0f, -1.0f, -goldenRatio), glm::vec3(-goldenRatio, 0.0f, -1.0f));
	GenerateIcosahedronFace(pMesh, glm::vec3(0.0f, 1.0f, -goldenRatio), glm::vec3(goldenRatio, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, -goldenRatio));

	GenerateIcosahedronFace(pMesh, glm::vec3(1.0f, -goldenRatio, 0.0f), glm::vec3(goldenRatio, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, -goldenRatio));
	GenerateIcosahedronFace(pMesh, glm::vec3(0.0f, 1.0f, -goldenRatio), glm::vec3(-1.0f, goldenRatio, 0.0f), glm::vec3(-goldenRatio, 0.0f, -1.0f));
	GenerateIcosahedronFace(pMesh, glm::vec3(1.0f, goldenRatio, 0.0f), glm::vec3(0.0f, 1.0f, -goldenRatio), glm::vec3(-1.0f, goldenRatio, 0.0f));
	GenerateIcosahedronFace(pMesh, glm::vec3(1.0f, goldenRatio, 0.0f), glm::vec3(0.0f, 1.0f, -goldenRatio), glm::vec3(goldenRatio, 0.0f, -1.0f));

	// Add Indices
	for (int i = 0; i < pMesh->mVertices.size(); ++i)
		pMesh->mIndices.push_back(i);

	// Calculate Vertex Tangents 
	CalculateTangents(pMesh);

	// Set min and max vertex position values for the mesh
	SetMinMax(pMesh);

	return pMesh;
}

GEAR_API Mesh * ShapeGenerator::GenerateCylinder()
{
	Mesh* pMesh = new Mesh();
	pMesh->mMeshName = "Cylinder";
	int n = 32;
	float d = 2.0f*glm::pi<float>() / float(n);
	for (int i = 0; i <= n; i++) {
		float s = i * 2.0f*glm::pi<float>() / float(n);
		for (int j = 0; j <= 1; j++) {
			float t = j;
			float x = cos(s);
			float y = t * 2.0f - 1.0f;
			float z = sin(s); 
			Vertex vertex;
			vertex.position = glm::vec3(x, y, z);
			vertex.normal = glm::vec3(x, y, z);
			vertex.textureCoordinate = glm::vec2(s / glm::two_pi<float>(), t / glm::pi<float>());
			pMesh->mVertices.push_back(vertex);

			// Add Indices
			if (i > 0 && j > 0) {
				// Face 1
				pMesh->mIndices.push_back((i - 1) * (n + 1) + (j - 1));
				pMesh->mIndices.push_back((i - 1) * (n + 1) + j);
				pMesh->mIndices.push_back(i * (n + 1) + j);

				// Face 2
				pMesh->mIndices.push_back((i - 1) * (n + 1) + (j - 1));
				pMesh->mIndices.push_back(i * (n + 1) + j);
				pMesh->mIndices.push_back(i * (n + 1) + (j - 1));
			}
		}
	}
	return pMesh;
}

GEAR_API Mesh * ShapeGenerator::GenerateCapsule()
{
	Mesh* pMesh = new Mesh();
	pMesh->mMeshName = "Capsule";

	// Generate Sphere Mesh
	int numSubdivisions = 16;
	float d = glm::two_pi<float>() / float(numSubdivisions * 2);
	for (int i =  -numSubdivisions; i <= numSubdivisions; ++i) {
		float s = (i * glm::two_pi<float>()) / float(numSubdivisions * 2);
		for (int j = 0; j <= numSubdivisions; ++j) {
			float t = (j * glm::pi<float>()) / float(numSubdivisions);

			float x = cosf(s) * sinf(t);
			float y = sinf(s) * sinf(t);
			float z = cosf(t);

			// Compute Vertex
			Vertex vertex;
			vertex.position = glm::vec3(x, y, z);
			vertex.normal = glm::vec3(x, y, z);
			vertex.textureCoordinate = glm::vec2(s / glm::two_pi<float>(), t / glm::pi<float>());
			pMesh->mVertices.push_back(vertex);

			// Add Indices
			if (i > 0 && j > 0) {
				// Face 1
				pMesh->mIndices.push_back((i - 1) * (numSubdivisions + 1) + (j - 1));
				pMesh->mIndices.push_back((i - 1) * (numSubdivisions + 1) + j);
				pMesh->mIndices.push_back(i * (numSubdivisions + 1) + j);

				// Face 2
				pMesh->mIndices.push_back((i - 1) * (numSubdivisions + 1) + (j - 1));
				pMesh->mIndices.push_back(i * (numSubdivisions + 1) + j);
				pMesh->mIndices.push_back(i * (numSubdivisions + 1) + (j - 1));
			}
		}
	}

	// Calculate Vertex Tangents 
	CalculateTangents(pMesh);

	// Set min and max vertex position values for the mesh
	SetMinMax(pMesh);

	return pMesh;
}

void ShapeGenerator::GenerateCubeFace(Mesh* a_pMesh, glm::mat4 a_transformation)
{
	// Compute Vertices for first triangle
	Vertex vertices1[3];
	vertices1[0].position = a_transformation * glm::vec4(-1.0f, 0.0f, -1.0f, 1.0f);
	vertices1[0].textureCoordinate = glm::vec2(0.0f, 0.0f);

	vertices1[1].position = a_transformation * glm::vec4(1.0f, 0.0f, -1.0f, 1.0f);
	vertices1[1].textureCoordinate = glm::vec2(1.0f, 0.0f);

	vertices1[2].position = a_transformation * glm::vec4(-1.0f, 0.0f, 1.0f, 1.0f);
	vertices1[2].textureCoordinate = glm::vec2(0.0f, 1.0f);

	// Compute Vertices for second triangle
	Vertex vertices2[3];
	vertices2[0].position = a_transformation * glm::vec4(1.0f, 0.0f, -1.0f, 1.0f);
	vertices2[0].textureCoordinate = glm::vec2(1.0f, 0.0f);

	vertices2[1].position = a_transformation * glm::vec4(-1.0f, 0.0f, 1.0f, 1.0f);
	vertices2[1].textureCoordinate = glm::vec2(0.0f, 1.0f);

	vertices2[2].position = a_transformation * glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
	vertices2[2].textureCoordinate = glm::vec2(1.0f, 1.0f);

	// Compute Normal
	glm::vec3 normal = glm::cross(vertices1[0].position - vertices1[2].position, vertices1[1].position - vertices1[2].position);
	float dot = glm::dot(normal, vertices1[0].position);
	if (dot < 0.0f)
		normal *= -1.0f;

	vertices1[0].normal = normal;
	vertices1[1].normal = normal;
	vertices1[2].normal = normal;
	vertices2[0].normal = normal;
	vertices2[1].normal = normal;
	vertices2[2].normal = normal;

	a_pMesh->mVertices.push_back(vertices1[2]);
	a_pMesh->mVertices.push_back(vertices1[1]);
	a_pMesh->mVertices.push_back(vertices1[0]);
	a_pMesh->mVertices.push_back(vertices2[0]);
	a_pMesh->mVertices.push_back(vertices2[1]);
	a_pMesh->mVertices.push_back(vertices2[2]);
}

void ShapeGenerator::GenerateIcosahedronFace(Mesh* a_pMesh, glm::vec3 a_vertPos1, glm::vec3 a_vertPos2, glm::vec3 a_vertPos3)
{
	// Compute Vertices
	Vertex vertices[3];
	vertices[0].position = a_vertPos1;
	vertices[0].textureCoordinate = glm::vec2(0.0f, 0.0f);

	vertices[1].position = a_vertPos2;
	vertices[1].textureCoordinate = glm::vec2(0.0f, 0.0f);

	vertices[2].position = a_vertPos3;
	vertices[2].textureCoordinate = glm::vec2(0.0f, 0.0f);

	// Compute Normal
	glm::vec3 normal = glm::cross(vertices[0].position - vertices[2].position, vertices[1].position - vertices[2].position);
	float dot = glm::dot(normal, vertices[0].position);
	if (dot < 0.0f)
		normal *= -1.0f;

	vertices[0].normal = normal;
	vertices[1].normal = normal;
	vertices[2].normal = normal;

	if (dot < 0.0f) {
		a_pMesh->mVertices.push_back(vertices[2]);
		a_pMesh->mVertices.push_back(vertices[1]);
		a_pMesh->mVertices.push_back(vertices[0]);
	}
	else {
		a_pMesh->mVertices.push_back(vertices[0]);
		a_pMesh->mVertices.push_back(vertices[1]);
		a_pMesh->mVertices.push_back(vertices[2]);
	}
}

void ShapeGenerator::SetMinMax(Mesh* a_pMesh)
{
	// For each vertex, update the min/max vertex position values in each direction
	for (int i = 0; i < a_pMesh->mVertices.size(); ++i) {
		glm::vec3 vertexPosition = a_pMesh->mVertices[i].position;

		// Update Min/Max vertex values in each direction

		// Check X
		if (vertexPosition.x < a_pMesh->mMinMaxX.x)
			a_pMesh->mMinMaxX.x = vertexPosition.x;
		if (vertexPosition.x > a_pMesh->mMinMaxX.y)
			a_pMesh->mMinMaxX.y = vertexPosition.x;

		// Check Y
		if (vertexPosition.y < a_pMesh->mMinMaxY.x)
			a_pMesh->mMinMaxY.x = vertexPosition.y;
		if (vertexPosition.y > a_pMesh->mMinMaxY.y)
			a_pMesh->mMinMaxY.y = vertexPosition.y;

		// Check Z
		if (vertexPosition.z < a_pMesh->mMinMaxZ.x)
			a_pMesh->mMinMaxZ.x = vertexPosition.z;
		if (vertexPosition.z > a_pMesh->mMinMaxZ.y)
			a_pMesh->mMinMaxZ.y = vertexPosition.z;
	}
}

void ShapeGenerator::CalculateTangents(Mesh* a_pMesh)
{
	for (int i = 0; i < a_pMesh->mIndices.size(); i += 3) {
		Vertex& vertex0 = a_pMesh->mVertices[a_pMesh->mIndices[i]];
		Vertex& vertex1 = a_pMesh->mVertices[a_pMesh->mIndices[i + 1]];
		Vertex& vertex2 = a_pMesh->mVertices[a_pMesh->mIndices[i + 2]];

		glm::vec3 edge0 = vertex1.position - vertex0.position;
		glm::vec3 edge1 = vertex2.position - vertex0.position;

		glm::vec2 deltaUV0 = vertex1.textureCoordinate - vertex0.textureCoordinate;
		glm::vec2 deltaUV1 = vertex2.textureCoordinate - vertex0.textureCoordinate;

		float det = deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y;

		glm::vec3 tangent = (deltaUV1.y * edge0 - deltaUV0.y * edge1) / det;

		vertex0.tangent += tangent;
		vertex1.tangent += tangent;
		vertex2.tangent += tangent;
	}

	for (int i = 0; i < a_pMesh->mVertices.size(); ++i)
		a_pMesh->mVertices[i].tangent = glm::normalize(a_pMesh->mVertices[i].tangent);
}