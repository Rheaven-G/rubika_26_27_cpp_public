#pragma once

#include <SFML/Graphics/Texture.hpp>

#include <filesystem>
#include <unordered_map>
#include <string>

#include "DebugMgr.h"

struct AnimationData
{
	int StartX = 0;
	int StartY = 0;
	int SizeX = 0;
	int SizeY = 0;
	int OffsetX = 0;
	int OffsetY = 0;
	int AnimationSpriteCount = 0;
	int SpriteOnLine = 0;
	bool IsRevertedX = false;
	bool IsRevertedY = false;
	bool IsBackward  = false;
	float TimeBetweenAnimationInS = 0.f;
};

struct StaticTileData
{
	int StartX = 0;
	int StartY = 0;
	int SizeX = 0;
	int SizeY = 0;
	bool IsRevertedX = false;
	bool IsRevertedY = false;
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
	TextureData(std::string name, std::filesystem::path texPath, sf::Texture texture, TextureType type = 
	TextureType::Background);
	~TextureData();
	
	std::string name;
	TextureType type;
	std::filesystem::path texPath;
	AnimationData dAnimation;
	StaticTileData dStaticTile;
	sf::Texture Texture;
	// Container of AnimationData AnimationsData;
	// Container of StaticTileData StaticTilesData;

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