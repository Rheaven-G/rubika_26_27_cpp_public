#include "TextureMgr.h"
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_utils.hpp>

#include "Globals.h"
#include "Logger.h"

// ---- TextureData
TextureData::TextureData() 
    :name("NULL"), type(TextureType::Background), texPath("NULL"), Texture(sf::Texture()) {}
TextureData::TextureData(std::string name, std::filesystem::path texPath, sf::Texture texture, TextureType type) 
    : name(name), type(type), texPath(texPath), Texture(texture) {}
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
            "TextureMgr::GetTextureData -> No texture with name '{}' found. Does it exists ?", name));
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
    try { xTex.parse<0>(mTex.data()); } 
    catch (const char* msg) { Logger::Error(
            std::format(
                "TextureMgr::LoadTexture -> XML is broken or corrupted. rapidxml error '{}'.", msg)); }
    
    // ---- Check image validity + LOAD
    sf::Texture tTex = sf::Texture();
    if (!tTex.loadFromFile(path, true))
    {
        Logger::Error(
            std::format(
                "TextureMgr::LoadTexture -> Failed to load image texture from '{}'. Are you sure it's an image ?",
                path.string()));
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
    
    // ---- Creation of actual TextureData
    TextureData dTex = TextureData(nTex, path, tTex);
    // ---- Adding it to the hashmap of TextureData
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
    static ImGuiTreeNodeFlags tree_node_flags = 
        ImGuiTreeNodeFlags_SpanAllColumns | 
        ImGuiTreeNodeFlags_DefaultOpen | 
        ImGuiTreeNodeFlags_DrawLinesFull;
    
    if (ImGui::BeginTable("TextureData table", 5, table_flags))
    {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
        ImGui::TableSetupColumn("Type");
        ImGui::TableSetupColumn("Dimensions");
        ImGui::TableSetupColumn("Size");
        ImGui::TableSetupColumn("Format");
        ImGui::TableHeadersRow();
        ImGui::TableNextColumn();
        ImGui::TableNextRow();
            
        for (std::pair<const std::string, TextureData>& tex : m_mTextures)
        {
            // Name
            ImGui::TextUnformatted(tex.first.c_str());
            ImGui::TableNextColumn();
            // TexType
            char* typeRepr = new char('B');
            if (tex.second.type == TextureType::Animation) { *typeRepr = 'A';}
            ImGui::TextUnformatted(typeRepr);
            ImGui::TableNextColumn();
            // Dimensions
            sf::Vector2u tDim = tex.second.Texture.getSize();
            ImGui::TextUnformatted(std::format("{}x{}", tDim.x, tDim.y).c_str());
            ImGui::TableNextColumn();
            // Size
            ImGui::TextUnformatted(std::format("{} pixels", tDim.x * tDim.y).c_str());
            ImGui::TableNextColumn();
            // Format
            ImGui::TextUnformatted(tex.second.Texture.isSrgb() ? "sRGB" : "non-sRGB");
            ImGui::TableNextColumn();
            ImGui::TableNextRow();
        }
        
        // ---- Ends table
        ImGui::EndTable();
    }
}