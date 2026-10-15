#pragma once

#include <SFML/Graphics/Texture.hpp>

#include <filesystem>
#include <unordered_map>
#include <string>

#include "DebugMgr.h"
#include "rapidxml/rapidxml.hpp"

struct StaticTileData
{
	int StartX = 0;
	int StartY = 0;
	int SizeX = 0;
	int SizeY = 0;
	bool IsReverted = false;
};
struct AnimationData : public StaticTileData
{
	int OffsetX = 0;
	int OffsetY = 0;
	int AnimationSpriteCount = 0;
	int SpriteOnLine = 0;
	float TimeBetweenAnimationInS = 0.f;
};

enum TextureType
{
	Animation,
	Background,
};
struct TextureData
{
public:
	TextureData();
	~TextureData();
	
	std::string name;
	std::filesystem::path texPath;
	sf::Texture Texture;
	std::unordered_map<std::string,  AnimationData> dAnimation;
	std::unordered_map<std::string, StaticTileData> dStaticTile;
	TextureType type;

	// friend class TextureMgr; // Forgotten change, before had private stuff and was a class
};

class TextureMgr final : public IDebugable
{
private:
	std::unordered_map<std::string, TextureData> m_mTextures;
public:
	TextureMgr();
	~TextureMgr();
	void Start();
	void Update(float fDeltaTime);
	void Destroy();
	void Init();
	void Shut();

	/**
	  * [] Loads the texture using the given path, including its metadata.
	  * [] Obviously, this function will also store the loaded data
	  * [] If errors occur during the loading process, this function must return false.
	  * [] Return true otherwise.
	  */
	bool LoadTexture(const std::filesystem::path& path);

	/**
	  * [] Returns the corresponding TextureData using the name of the texture. 
	  * [] Returns the default TextureData in case of an unknown name
	  */
	const TextureData& GetTextureData(const std::string& name) const;
	
	// ---- IDebugable
	void DrawDebug();
};
static bool ParseStaticTileMetadata(TextureData& dTex, rapidxml::xml_node<>* node);
static bool ParseAnimationMetadata (TextureData& dTex, rapidxml::xml_node<>* node);