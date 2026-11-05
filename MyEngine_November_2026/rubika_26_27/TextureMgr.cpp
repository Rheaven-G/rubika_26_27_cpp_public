#include "TextureMgr.h"

#include <Globals.h>
#include <rapidxml/rapidxml_utils.hpp>

#ifdef _USE_IMGUI
#include <Imgui/imgui.h>
#include <Imgui/imgui-SFML.h>
#endif

#include <assert.h>
#include <filesystem>
#include <iostream>

void TextureMgr::Init()
{
	gData.DebugMgr->RegisterDebugableWindow("TextureMgr", this);
}

void TextureMgr::Shut()
{
	gData.DebugMgr->UnregisterDebugableWindow("TextureMgr");
}

bool TextureMgr::LoadTexture(const std::filesystem::path& path)
{
	if (!std::filesystem::exists(path.native()))
	{
		std::cerr << "Texture file doesn't exist " << path << std::endl;
		return false;
	}

	std::filesystem::path metadataPath = path;
	metadataPath = metadataPath.replace_extension(".xml");
	if (!std::filesystem::exists(metadataPath.native()))
	{
		std::cerr << "Texture metadata file doesn't exist " << metadataPath << std::endl;
		return false;
	}

	auto p = Textures.emplace(std::piecewise_construct,
		std::forward_as_tuple(path.string()),
		std::forward_as_tuple());

	if (!p.second)
	{
		std::cerr << "LoadTexture: Internal error. Cannot emplace in map" << std::endl;
		return false;
	}

	TextureData& textureData = p.first->second;
	if (!textureData.Texture.loadFromFile(path.string()))
	{
		return false;
	}

	if (!LoadTextureMetadata(metadataPath, textureData))
	{
		return false;
	}

	return true;
}

const TextureData& TextureMgr::GetTextureData(const std::string& name) const
{
	assert(Textures.find(name) != Textures.end());
	return Textures.at(name);
}

sf::Texture emptyTexture;

const sf::Texture& TextureMgr::GetEmptyTexture()
{
	return emptyTexture;
}

bool TextureMgr::LoadTextureMetadata(const std::filesystem::path& path, TextureData& textureData)
{
	rapidxml::file<> metadataFile(path.string().c_str());
	if (metadataFile.size() == 0)
	{
		std::cerr << "LoadTextureMetadata: Cannot open file " << path << std::endl;
		return false;
	}

	rapidxml::xml_document metadataXml;
	metadataXml.parse<0>(metadataFile.data());

	if (rapidxml::xml_node<>* node = metadataXml.first_node("Animations"))
	{
		if (!LoadAnimationMetadata(node, textureData))
		{
			return false;
		}
	}

	if (rapidxml::xml_node<>* node = metadataXml.first_node("Backgrounds"))
	{
		if (!LoadStaticTileMetadata(node, textureData))
		{
			return false;
		}
	}

	return true;
}

bool TextureMgr::LoadAnimationMetadata(rapidxml::xml_node<>* node, TextureData& textureData)
{
	if (!node)
	{
		return false;
	}

	rapidxml::xml_node<>* animationNode = node->first_node();
	while (animationNode)
	{
		rapidxml::xml_attribute<>* nameAttribute = animationNode->first_attribute("Name");
		if (nameAttribute)
		{
			auto p = textureData.AnimationsData.emplace(std::string(nameAttribute->value(), nameAttribute->value_size()), AnimationData());
			if (p.second)
			{
				AnimationData& data = p.first->second;
				if (rapidxml::xml_node<>* innerNode = animationNode->first_node("X"))
				{
					std::string value(innerNode->value(), innerNode->value_size());
					data.StartX = atoi(value.c_str());
				}

				if (rapidxml::xml_node<>* innerNode = animationNode->first_node("Y"))
				{
					std::string value(innerNode->value(), innerNode->value_size());
					data.StartY = atoi(value.c_str());
				}

				if (rapidxml::xml_node<>* innerNode = animationNode->first_node("SizeX"))
				{
					std::string value(innerNode->value(), innerNode->value_size());
					data.SizeX = atoi(value.c_str());
				}

				if (rapidxml::xml_node<>* innerNode = animationNode->first_node("SizeY"))
				{
					std::string value(innerNode->value(), innerNode->value_size());
					data.SizeY = atoi(value.c_str());
				}

				if (rapidxml::xml_node<>* innerNode = animationNode->first_node("OffsetX"))
				{
					std::string value(innerNode->value(), innerNode->value_size());
					data.OffsetX = atoi(value.c_str());
				}

				if (rapidxml::xml_node<>* innerNode = animationNode->first_node("OffsetY"))
				{
					std::string value(innerNode->value(), innerNode->value_size());
					data.OffsetY = atoi(value.c_str());
				}

				if (rapidxml::xml_node<>* innerNode = animationNode->first_node("SpriteNum"))
				{
					std::string value(innerNode->value(), innerNode->value_size());
					data.AnimationSpriteCount = atoi(value.c_str());
				}

				if (rapidxml::xml_node<>* innerNode = animationNode->first_node("SpritesOnLine"))
				{
					std::string value(innerNode->value(), innerNode->value_size());
					data.SpriteOnLine = atoi(value.c_str());
				}

				if (rapidxml::xml_node<>* innerNode = animationNode->first_node("Reverted"))
				{
					std::string value(innerNode->value(), innerNode->value_size());
					data.IsReverted = atoi(value.c_str());
				}

				if (rapidxml::xml_node<>* innerNode = animationNode->first_node("TimeBetweenAnimation"))
				{
					std::string value(innerNode->value(), innerNode->value_size());
					data.TimeBetweenAnimationInS = std::stof(value.c_str());
				}
			}
			else
			{
				std::cerr << "LoadAnimationMetadata: Cannot add animation " << nameAttribute->value() << ". Ignore it" << std::endl;
			}
		}
		else
		{
			std::cerr << "LoadAnimationMetadata: Find a animation node with no name. Ignore it" << std::endl;
		}

		animationNode = animationNode->next_sibling();
	}

	return true;
}

bool TextureMgr::LoadStaticTileMetadata(rapidxml::xml_node<>* node, TextureData& textureData)
{
	if (!node)
	{
		return false;
	}

	rapidxml::xml_node<>* tileNode = node->first_node();
	while (tileNode)
	{
		rapidxml::xml_attribute<>* nameAttribute = tileNode->first_attribute("Name");
		if (nameAttribute)
		{
			auto p = textureData.StaticTilesData.emplace(std::string(nameAttribute->value(), nameAttribute->value_size()), StaticTileData());
			if (p.second)
			{
				StaticTileData& data = p.first->second;
				if (rapidxml::xml_node<>* innerNode = tileNode->first_node("X"))
				{
					std::string value(innerNode->value(), innerNode->value_size());
					data.StartX = atoi(value.c_str());
				}

				if (rapidxml::xml_node<>* innerNode = tileNode->first_node("Y"))
				{
					std::string value(innerNode->value(), innerNode->value_size());
					data.StartY = atoi(value.c_str());
				}

				if (rapidxml::xml_node<>* innerNode = tileNode->first_node("SizeX"))
				{
					std::string value(innerNode->value(), innerNode->value_size());
					data.SizeX = atoi(value.c_str());
				}

				if (rapidxml::xml_node<>* innerNode = tileNode->first_node("SizeY"))
				{
					std::string value(innerNode->value(), innerNode->value_size());
					data.SizeY = atoi(value.c_str());
				}

				if (rapidxml::xml_node<>* innerNode = tileNode->first_node("RevertedX"))
				{
					std::string value(innerNode->value(), innerNode->value_size());
					data.IsRevertedX = atoi(value.c_str());
				}

				if (rapidxml::xml_node<>* innerNode = tileNode->first_node("RevertedY"))
				{
					std::string value(innerNode->value(), innerNode->value_size());
					data.IsRevertedY = atoi(value.c_str());
				}
			}
			else
			{
				std::cerr << "LoadStaticTileMetadata: Cannot add static Data " << nameAttribute->value() << ". Ignore it" << std::endl;
			}
		}
		else
		{
			std::cerr << "LoadStaticTileMetadata: Find a tile node with no name. Ignore it" << std::endl;
		}

		tileNode = tileNode->next_sibling();
	}

	return true;
}

void TextureMgr::DrawDebug()
{
#ifdef _USE_IMGUI
//
// 	const auto flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders;
//
// 	if (ImGui::BeginTable("##Textures", 4, flags))
// 	{
// 		ImGui::TableSetupColumn("Name");
// 		ImGui::TableSetupColumn("Width");
// 		ImGui::TableSetupColumn("Height");
// 		ImGui::TableSetupColumn("Image");
// 		ImGui::TableHeadersRow();
//
// 		for (const auto& [name, data] : Textures)
// 		{
// 			ImGui::TableNextColumn();
// 			ImGui::TextWrapped(name.c_str());
// 			ImGui::TableNextColumn();
// 			ImGui::TextWrapped("%d", data.Texture.getSize().x);
// 			ImGui::TableNextColumn();
// 			ImGui::TextWrapped("%d", data.Texture.getSize().y);
// 			ImGui::TableNextColumn();
//
// 			const sf::Vector2f size((float)data.Texture.getSize().x, (float)data.Texture.getSize().y);
// 			const float displayMaxSizeX = 200;
// 			const float displayMaxSizeY = 200;
//
// 			sf::Vector2f displaySize;
// 			if (size.x < displayMaxSizeX && size.y < displayMaxSizeY)
// 			{
// 				displaySize = size;
// 			}
// 			else if (size.x > size.y)
// 			{
// 				displaySize.x = displayMaxSizeX;
// 				displaySize.y = size.y / size.x * displayMaxSizeY;
// 			}
// 			else
// 			{
// 				displaySize.x = size.x / size.y * displayMaxSizeX;
// 				displaySize.y = displayMaxSizeY;
// 			}
//
// 			ImGui::Image(data.Texture, displaySize);
//
// 			ImGui::TableNextRow();
// 		}
// 		ImGui::EndTable();
// 	}
	
	static float maxTexWidthInGUI = 126.f;
// ---- FLAGS
    static ImGuiTableFlags table_flags = 
        ImGuiTableFlags_BordersV | 
        ImGuiTableFlags_BordersOuterH | 
        ImGuiTableFlags_Resizable | 
        ImGuiTableFlags_RowBg | 
        ImGuiTableFlags_NoBordersInBody;
	static ImGuiTableFlags table_flags_for_last_column = 
		ImGuiTableColumnFlags_WidthFixed;
    
    static bool unfoldAll { true };
    ImGui::Checkbox("Unfold All", &unfoldAll);
    
    // ---- Drawable Struct
    struct ListElement
    {
        const std::string Type;
        const std::string& Name;
        const sf::Texture* Texture;
        const sf::Vector2f Dimensions;
        
        const std::unordered_map<std::string, AnimationData>& AnimationDatas;
    	const std::unordered_map<std::string, StaticTileData>& StaticTilesDatas;
    
        void Preview() const
        {
            ImGui::SeparatorText(Name.c_str());
			ShowContent();
        }
        void NoPreview() const
        {
            if (ImGui::TreeNodeEx(Name.c_str()))
            {
            	ShowContent();
                ImGui::TreePop();
            }
        }
    	void ShowContent() const
        {
        	float imageRatio = (float)Texture->getSize().y / (float)Texture->getSize().x;
        	ImGui::Image(*Texture, sf::Vector2f(maxTexWidthInGUI, maxTexWidthInGUI*imageRatio));
        	ImGui::SameLine();  
        	ImGui::BeginTable(std::format("Properties##{}", Name).c_str(), 2, table_flags);
        	ImGui::TableSetupColumn("Data");
        	ImGui::TableSetupColumn("Value", table_flags_for_last_column, 50);
        	// Type line
        	ImGui::TableNextRow();
        	ImGui::TableNextColumn();
        	ImGui::TextUnformatted("Type");
        	ImGui::TableNextColumn();
        	ImGui::TextUnformatted(Type.c_str());
        	// Dimensions line
        	ImGui::TableNextRow();
        	ImGui::TableNextColumn();
        	ImGui::TextUnformatted("Dimensions");
        	ImGui::TableNextColumn();
        	ImGui::TextUnformatted(std::format("{}x{}", Dimensions.x, Dimensions.y).c_str());
        	// Data Section
        	if (Type == "SpriteTexture")
        	{
        		ImGui::TableNextRow();
        		ImGui::TableNextColumn();
        		if (ImGui::TreeNode("Animations"))
        		{
        			for (auto animData : AnimationDatas)
        			{
        				AnimationData& dAnim = animData.second;
        				ImGui::TableNextRow();
        				ImGui::TableNextColumn();
        				if (ImGui::TreeNodeEx(animData.first.c_str()))
        				{
        					ImGui::TableNextRow();
        					ImGui::TableNextColumn();
        					ImGui::TextUnformatted("Anim sprite count");
        					ImGui::TableNextColumn();
        					ImGui::TextUnformatted(std::format("{}", dAnim.SpriteOnLine).c_str());
        				
        					ImGui::TableNextRow();
        					ImGui::TableNextColumn();
        					ImGui::TextUnformatted("Sprites on line");
        					ImGui::TableNextColumn();
        					ImGui::TextUnformatted(std::format("{}",dAnim.SpriteOnLine).c_str());
        				
        					ImGui::TableNextRow();
        					ImGui::TableNextColumn();
        					ImGui::TextUnformatted("Reverted");
        					ImGui::TableNextColumn();
        					ImGui::TextUnformatted(dAnim.IsReverted ? "Yes": "No");
        				
        					ImGui::TableNextRow();
        					ImGui::TableNextColumn();
        					ImGui::TextUnformatted("Seconds Between sprites");
        					ImGui::TableNextColumn();
        					ImGui::TextUnformatted(std::format("{}",
								dAnim.TimeBetweenAnimationInS).c_str());
        				
        					ImGui::TableNextRow();
        					ImGui::TableNextColumn();
        					ImGui::TextUnformatted("Start");
        					ImGui::TableNextColumn();
        					ImGui::TextUnformatted(std::format("({},{})", 
								dAnim.StartX, dAnim.StartY).c_str());
        				
        					ImGui::TableNextRow();
        					ImGui::TableNextColumn();
        					ImGui::TextUnformatted("Size");
        					ImGui::TableNextColumn();
        					ImGui::TextUnformatted(std::format("({},{})", 
								dAnim.SizeX, dAnim.SizeY).c_str());
        				
        					ImGui::TableNextRow();
        					ImGui::TableNextColumn();
        					ImGui::TextUnformatted("Offset");
        					ImGui::TableNextColumn();
        					ImGui::TextUnformatted(std::format("({}, {})",
								dAnim.OffsetX, dAnim.OffsetY).c_str());
        				
        					ImGui::TreePop();
        				}
        				ImGui::TableNextColumn();
        			}
					ImGui::TreePop();
        		}
        	} else {
        		ImGui::TableNextRow();
        		ImGui::TableNextColumn();
        		if (ImGui::TreeNode("Tiles"))
        		{
        			for (auto tileData : StaticTilesDatas)
        			{	
        				StaticTileData& dTile = tileData.second;
        				ImGui::TableNextRow();
        				ImGui::TableNextColumn();
        				if (ImGui::TreeNodeEx(tileData.first.c_str()))
        				{
        					ImGui::TableNextRow();
        					ImGui::TableNextColumn();
        					ImGui::TextUnformatted("Reverted");
        					ImGui::TableNextColumn();
        					ImGui::TextUnformatted(std::format("X:{},Y:{}", 
								dTile.IsRevertedX ? "Yes":"No", dTile.IsRevertedY ? "Yes":"No").c_str());
        				
        					ImGui::TableNextRow();
        					ImGui::TableNextColumn();
        					ImGui::TextUnformatted("Start");
        					ImGui::TableNextColumn();
        					ImGui::TextUnformatted(std::format("({},{})", 
								dTile.StartX, dTile.StartY).c_str());
        				
        					ImGui::TableNextRow();
        					ImGui::TableNextColumn();
        					ImGui::TextUnformatted("Size");
        					ImGui::TableNextColumn();
        					ImGui::TextUnformatted(std::format("({},{})", 
								dTile.SizeX, dTile.SizeY).c_str());
        					ImGui::TreePop();
        				}
        				ImGui::TableNextColumn();
        			}
					ImGui::TreePop();
        		}
        	}
                
        	ImGui::EndTable();
        }
    };
    
    std::vector<ListElement> elements;
    elements.reserve(Textures.size());
    for (const auto& it : Textures)
    {
        const TextureData& td = it.second;
        ListElement le {
            !td.AnimationsData.empty()  ? "SpriteTexture" : "TileTexture",
            it.first,
            &td.Texture,
            {(float)td.Texture.getSize().x, (float)td.Texture.getSize().y},
            td.AnimationsData,
        	td.StaticTilesData,
        };
        elements.push_back(le);
    }
    for (const auto& ele : elements)
    {
        if (unfoldAll)
        {
            ele.Preview();
        } else
        {
            ele.NoPreview();
        }
    } 
#endif
}
