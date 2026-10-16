#include "TextureMgr.h"

#include <iostream>
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_utils.hpp>
#include <Imgui/imgui-SFML.h>
#include <SFML/Graphics/RenderTexture.hpp>

#include "Globals.h"
#include "Logger.h"

// ---- TextureData
TextureData::TextureData() 
    :name("NULL"), type(TextureType::Background), texPath("NULL"), Texture(sf::Texture()) {}
TextureData::~TextureData() {}
const TextureData& TextureMgr::GetTextureData(const std::string& name) const
{
    if (!m_mTextures.empty())
    {
        if (m_mTextures.contains(name))
        {
            return m_mTextures.find(name)->second;
        }
    }
    Logger::Error(
        std::format(
            "TextureMgr::GetTextureData -> No texture with name '{}' found. Does it exists ? Returning blank TextureData.", name));
    return TextureData();
}

// ---- TextureMgr
TextureMgr::TextureMgr() {}
TextureMgr::~TextureMgr()
{
    Destroy();
}
void TextureMgr::Start() {}
void TextureMgr::Update(float fDeltaTime) {}
void TextureMgr::Destroy()
{
    m_mTextures.clear();
}
void TextureMgr::Init()
{
	gData.DebugMgr->RegisterDebugableWindow("Textures", this);
}
void TextureMgr::Shut()
{
	gData.DebugMgr->UnregisterDebugableWindow("Textures");
}
bool TextureMgr::LoadTexture(const std::filesystem::path& path)
{
    std::string xmlPath = path.string();
    xmlPath.replace(xmlPath.length()-3, 3, "xml"); 
    // TODO: Make ^^^ work with any texture extension (.png, .jpeg, .webp, etc)
    
    /*
     * FAILPROOF 
     */
    // ---- Existance check
    if (!std::filesystem::exists(path))
    {
        Logger::Error(
            std::format(
                "TextureMgr::LoadTexture -> no texture file at '{}'.", 
                path.string()));
        return false;
    }
    if (!std::filesystem::exists(xmlPath))
    {
        Logger::Error(
            std::format(
                "TextureMgr::LoadTexture -> no metadata file at '{}'.", 
                xmlPath));
        return false;
    }
    
    // ---- Check metadata validity + LOA
    rapidxml::file<> mTex/* = rapidxml::file<>*/(xmlPath.c_str());
    rapidxml::xml_document<> xTex;
    
    // ---- XML is correctly formatted
    try { xTex.parse<0>(mTex.data()); } 
    catch (const char* msg) { 
        Logger::Error(
            std::format(
                "TextureMgr::LoadTexture -> XML is broken or corrupted. rapidxml error '{}'.", msg)); 
        return false;
    }
    
    // ---- Metadata loading
    TextureData dTex = TextureData();
    const char* firstNodeType = xTex.document()->first_node()->name();
    if (strcmp(firstNodeType, "Animations") == 0)
    {
        ParseAnimationMetadata(dTex, xTex.document()->first_node()->first_node()); // We pass the first animation
    }
    else if (strcmp(firstNodeType, "Background") == 0)
    {
        ParseStaticTileMetadata(dTex, xTex.document()->first_node()->first_node()); // We pass the first animation
    }
    else // Unrecognised node.
    {
        Logger::Error(
            std::format(
                "TextureMgr::LoadTexture -- TextureType not recognised. Can treat <Animation> or <Background> types."));
        return false;
    } // Unrecognised node.
    
    
    // ---- Check image validity + LOAD
    sf::Texture tTex = sf::Texture();
    if (!tTex.loadFromFile(path))
    {
        Logger::Error(
            std::format(
                "TextureMgr::LoadTexture -> Failed to load image texture from '{}'. Are you sure it's an image ?",
                path.string()));
        return false;
    }
    /* 
     * END FAILPROOF 
     */
    
    
    //doc.parse<0>(myFile.data());
    //Logger::Info(myFile.data());
    // ---- Get Name of texture from file name
    std::string nTex = xmlPath
        .replace(0, xmlPath.find_last_of('/')+1, "")    //remove everything before name of file
        .replace(xmlPath.length()-4, 4, "");                //remove file extension
    // ---- Assignation of actual TextureData
    dTex.Texture = tTex;
    dTex.name = nTex;
    dTex.texPath = path;
    // ---- Adding it to the hashmap of TextureData in TextureMgr
    m_mTextures[nTex] = dTex;
    // ---- If we are there, we succeeded !!
    return true;
}
void TextureMgr::DrawDebug()
{ 
    // ---- FLAGS
    static ImGuiTableFlags table_flags = 
        ImGuiTableFlags_BordersV | 
        ImGuiTableFlags_BordersOuterH | 
        ImGuiTableFlags_Resizable | 
        ImGuiTableFlags_RowBg | 
        ImGuiTableFlags_NoBordersInBody;
    
    static bool unfoldAll { true };
    ImGui::Checkbox("Unfold All", &unfoldAll);
    
    // ---- Drawable Struct
    struct ListElement
    {
        const std::string Type;
        const std::string& Name;
        const sf::Texture* Texture;
        const sf::Vector2f Dimensions;
        
        const std::unordered_map<std::string, AnimationData>& AnimationData;
    
        void Preview() const
        {
            ImGui::SeparatorText(Name.c_str());
            ImGui::Image(*Texture, Dimensions);
            ImGui::SameLine();  
              ImGui::BeginTable(std::format("Properties##{}", Name).c_str(), 2, table_flags);
              ImGui::TableSetupColumn("Data");
              ImGui::TableSetupColumn("Value");
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
            
            ImGui::EndTable();
        }
        void NoPreview() const
        {
            if (ImGui::TreeNodeEx(Name.c_str()))
            {
                ImGui::Image(*Texture, Dimensions);
                ImGui::SameLine();  
                  ImGui::BeginTable(std::format("Properties##{}", Name).c_str(), 2, table_flags);
                  ImGui::TableSetupColumn("Data");
                  ImGui::TableSetupColumn("Value");
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
                
                ImGui::EndTable();
                ImGui::TreePop();
            }
        }
    };
    
    std::vector<ListElement> elements;
    elements.reserve(m_mTextures.size());
    for (const auto& it : m_mTextures)
    {
        const TextureData& td = it.second;
        ListElement le {
            td.type == TextureType::Animation ? "A" : "B",
            it.first,
            &td.Texture,
            {(float)td.Texture.getSize().x, (float)td.Texture.getSize().y},
            td.dAnimation,
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
}

bool ParseStaticTileMetadata(TextureData& dTex, rapidxml::xml_node<>* node)
{
    return true;
}
bool ParseAnimationMetadata (TextureData& dTex, rapidxml::xml_node<>* node)
{
    rapidxml::xml_node<>* current = node;
    do
    {
        // ---- Checks
        if (strcmp(current->name(), "Animation") != 0)
        {
            Logger::Error(
                std::format(
                    "ParseAnimationMetadata -- Wrong node format for 'Animations' texture type."));
            return false;
        }
        
        rapidxml::xml_attribute<>* att = current->first_attribute();
        if (att != nullptr)
        {   
            if (strcmp(att->name(), "Name") != 0)
            {
                Logger::Error(std::format(
            "ParseAnimationMetadata -- Wrong node attribute name. Looking for 'Name' and found {}", 
                    att->name()));
                return false;
            }   
            // Then we are lowkey good
            
            AnimationData dAnimation{};
            Logger::Info(std::format(
                "first_attribute() is not nullptr and is named '{}', with value '{}'", 
                current->first_attribute()->name(),
                current->first_attribute()->value()));
            
            const char* sProps[] {
                // ---- StaticTile Shared
                "X",                    // StartX
                "Y",                    // StartY
                "SizeX",                // SizeX
                "SizeY",                // SizeY
                "Reverted",             // IsReverted
                // ---- Animation Specific
                "OffsetX",              // OffsetX
                "OffsetY",              // OffsetY
                "SpriteNum",            // AnimationSpriteCount
                "SpriteOnLine",         // SpriteOnLine
                "TimeBetweenAnimation", // TimeBetweenAnimationInS
            };
            // TODO: Investigate how to do it with map and bind char[] from xml to var in struct dAnimation 
            // const std::unordered_map<const char[], void*> properties;
            // properties["X"] = &dAnimation.StartX;
            
            rapidxml::xml_node<>* property = current->first_node();
            // ---- StaticTile Shared
            // int  StartX
            if (property != nullptr)
            {
                if (strcmp(property->name(), sProps[0]) == 0) 
                {dAnimation.StartX = std::stoi(property->value());}
            } else { dAnimation.StartX = 0;}
            property = property->next_sibling();
            
            // int  StartY
            if (property != nullptr)
            {
                if (strcmp(property->name(), sProps[1]) == 0)
                { dAnimation.StartY = std::stoi(property->value());}
            } else { dAnimation.StartY = 0;}
            property = property->next_sibling();
            
            // int  SizeX
            if (property != nullptr)
            {
                if (strcmp(property->name(), sProps[2]) == 0)
                { dAnimation.SizeX = std::stoi(property->value());}
            }else { dAnimation.SizeX = 0;}
            property = property->next_sibling();
            
            // int  SizeY
            if (property != nullptr)
            {
                if (strcmp(property->name(), sProps[3]) == 0)
                { dAnimation.SizeY = std::stoi(property->value());; }
            } else { dAnimation.SizeY = 0;}
            property = property->next_sibling();
            
            // bool IsReverted
            if (property != nullptr)
            {
                if (strcmp(property->name(), sProps[4]) == 0)
                { dAnimation.IsReverted = std::stoi(property->value());}
            } else { dAnimation.IsReverted = 0;}
            property = property->next_sibling();
            
            // ---- Animation Specific
            // int   OffsetX
            if (property != nullptr)
            {
                if (strcmp(property->name(), sProps[5]) == 0)
                { dAnimation.OffsetX = std::stoi(property->value());}
            } else { dAnimation.OffsetX = 0;}
            property = property->next_sibling();
            
            // int   OffsetY
            if (property != nullptr)
            {
                if (strcmp(property->name(), sProps[6]) == 0)
                { dAnimation.OffsetY = std::stoi(property->value());}
            } else { dAnimation.OffsetY = 0;}
            property = property->next_sibling();
            
            // int   AnimationSpriteCount
            if (property != nullptr)
            {
                if (strcmp(property->name(), sProps[7]) == 0)
                { dAnimation.AnimationSpriteCount = std::stoi(property->value());}
            } else { dAnimation.AnimationSpriteCount = 0;}
            property = property->next_sibling();
            
            // int   SpriteOnLine
            if (property != nullptr)
            {
                if (strcmp(property->name(), sProps[8]) == 0)
                { dAnimation.SpriteOnLine = std::stoi(property->value());}
            } else { dAnimation.SpriteOnLine = 0;}
            property = property->next_sibling();
            
            // float TimeBetweenAnimationInS
            if (property != nullptr)
            {
                if (strcmp(property->name(), sProps[9]) == 0)
                { dAnimation.TimeBetweenAnimationInS = std::stof(property->value()); }
            } 
            else
            { dAnimation.TimeBetweenAnimationInS = 0;}
            
            // ---- Add everything to AnimationData
            dTex.dAnimation[att->value()] = dAnimation;
        }
        
        current = current->next_sibling();
    } while ( current != nullptr );
    return true;   
}