#pragma once

#include "system/system.h"

#include <vector>
#include <future>
#include <atomic>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glew/glew.h>
#include <sfml/Graphics.hpp>

#include "type/terrain/type.h"
#include "type/model/type.h"
#include "type/form/type.h"

#define TILES_U 4
#define TILES_V 4

class ModuleTerrain : public Module
{	
	// general
public:
	~ModuleTerrain();
private:
	int candidates;
	void Init();
	void Update();
	void Listeners();
	void Callbacks();

	// texture
	GLuint diffuse, normal, specular;
	GLuint Texture(std::string Path);
	void TextureShrink(sf::Image &Texture);
	inline sf::Color TextureAverage(sf::Color A, sf::Color B, sf::Color C, sf::Color D);

	// loading
	std::atomic<bool> running, loading; std::mutex access; std::future<void> task;
	Terrain current; std::vector<GLfloat> positions, normals, texcoords; std::vector<GLuint> elements; std::vector<std::tuple<btVector3, btVector3, btVector3>> triangles; bool null;
	void Loading();
	void Mesh(Terrain *Terrain);
	inline bool Inside(glm::ivec3 Position, glm::ivec3 Min, glm::ivec3 Max);
	inline glm::ivec3 Shift(int Dimension, glm::ivec3 Vector);
	void Buffer(uint64_t Id);

	// save
	bool Load(Terrain *Terrain);
	void Save(Terrain *Terrain);

	// generation
	void Generate(Terrain *Terrain);
	void GenerateGrass(Terrain *Terrain);
	void GenerateTrees(Terrain *Terrain);

	// noise
	float NoisePositive(float Zoom, glm::vec2 Sample);
	float NoiseSigmoid (float Zoom, glm::vec2 Sample, float Shift = 0.0f, float Sharp = 1.0f);
	float FractionalBrownianMotion(float Zoom, glm::vec3 Sample, int Octave, float Lacunarity, float Gain);

	// block
	uint8_t GetBlock(glm::ivec3 Block);
	void SetBlock(glm::ivec3 Block, uint8_t Type);
	uint64_t GetChunk(glm::ivec3 Chunk);
	glm::ivec3 PosChunk(glm::ivec3 Block);
	glm::ivec3 PosLocal(glm::ivec3 Block);

	// select
	bool show; uint64_t marker;
	std::tuple<glm::ivec3, glm::ivec3, uint8_t> Selection();
	float Intbound(float s, float ds);
	int Signum(float x);
	bool InReachDistance(glm::vec3 Target);
	uint64_t Marker();
	uint8_t type;

public:
	// callbacks
	static void jsChunk      (const v8::FunctionCallbackInfo<v8::Value> &args);
	static void jsBlock      (const v8::FunctionCallbackInfo<v8::Value> &args);
	static void jsPlacetype  (const v8::FunctionCallbackInfo<v8::Value> &args);
	static void jsPlacemarker(const v8::FunctionCallbackInfo<v8::Value> &args);
	static void jsWorld      (const v8::FunctionCallbackInfo<v8::Value> &args);
};
