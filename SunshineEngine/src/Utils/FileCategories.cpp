#include "Utils/FileCategories.h"

// --- FILE FILTER STRINGS ---

static const char MODEL_FILTER[] =
    "3D Models (*.fbx;*.obj;*.abc;*.usd)\0*.fbx;*.obj;*.abc;*.usd\0"
    "FBX (*.fbx)\0*.fbx\0"
    "OBJ (*.obj)\0*.obj\0"
    "ABC (*.abc)\0*.abc\0"
    "USD (*.usd)\0*.usd\0"
    "All Files (*.*)\0*.*\0\0";

static const char TEXTURE_FILTER[] =
    "Textures (*.jpg;*.jpeg;*.png;*.tga;*.bmp;*.psd;*.dds;*.tiff;*.gif;*.hdr)\0*.jpg;*.jpeg;*.png;*.tga;*.bmp;*.psd;*.dds;*.tiff;*.gif;*.hdr\0"
    "JPG (*.jpg)\0*.jpg\0"
    "JPEG (*.jpeg)\0*.jpeg\0"
    "PNG (*.png)\0*.png\0"
    "TGA (*.tga)\0*.tga\0"
    "BMP (*.bmp)\0*.bmp\0"
    "PSD (*.psd)\0*.psd\0"
    "DDS (*.dds)\0*.dds\0"
    "TIFF (*.tiff)\0*.tiff\0"
    "GIF (*.gif)\0*.gif\0"
    "HDR (*.hdr)\0*.hdr\0"
    "All Files (*.*)\0*.*\0\0";

static const char AUDIO_FILTER[] =
    "Audio (*.wav;*.mp3;*.ogg)\0*.wav;*.mp3;*.ogg\0"
    "WAV (*.wav)\0*.wav\0"
    "MP3 (*.mp3)\0*.mp3\0"
    "OGG (*.ogg)\0*.ogg\0"
    "All Files (*.*)\0*.*\0\0";

static const char FONT_FILTER[] =
    "Fonts (*.ttf;*.otf)\0*.ttf;*.otf\0"
    "TTF (*.ttf)\0*.ttf\0"
    "OTF (*.otf)\0*.otf\0"
    "All Files (*.*)\0*.*\0\0";

static const char SCRIPT_FILTER[] =
    "Scripts (*.lua)\0*.lua\0"
    "All Files (*.*)\0*.*\0\0";

static const char SHADER_FILTER[] =
    "Shaders (*.hlsl;*.glsl;*.vert;*.frag;*.shader)\0*.hlsl;*.glsl;*.vert;*.frag;*.shader\0"
    "HLSL (*.hlsl)\0*.hlsl\0"
    "GLSL (*.glsl)\0*.glsl\0"
    "VERT (*.vert)\0*.vert\0"
    "FRAG (*.frag)\0*.frag\0"
    "SHADER (*.shader)\0*.shader\0"
    "All Files (*.*)\0*.*\0\0";

static const char ALL_FILTER[] =
    "All Supported Files (*.fbx;*.obj;*.abc;*.usd;"
    "*.jpg;*.jpeg;*.png;*.tga;*.bmp;*.psd;*.dds;*.tiff;*.gif;*.hdr;"
    "*.wav;*.mp3;*.ogg;"
    "*.ttf;*.otf;"
    "*.lua;"
    "*.hlsl;*.glsl;*.vert;*.frag;*.shader)\0"
    "*.fbx;*.obj;*.abc;*.usd;"
    "*.jpg;*.jpeg;*.png;*.tga;*.bmp;*.psd;*.dds;*.tiff;*.gif;*.hdr;"
    "*.wav;*.mp3;*.ogg;"
    "*.ttf;*.otf;"
    "*.lua;"
    "*.hlsl;*.glsl;*.vert;*.frag;*.shader\0"
    "3D Models (*.fbx;*.obj;*.abc;*.usd)\0*.fbx;*.obj;*.abc;*.usd\0"
    "Textures (*.jpg;*.jpeg;*.png;*.tga;*.bmp;*.psd;*.dds;*.tiff;*.gif;*.hdr)\0*.jpg;*.jpeg;*.png;*.tga;*.bmp;*.psd;*.dds;*.tiff;*.gif;*.hdr\0"
    "Audio (*.wav;*.mp3;*.ogg)\0*.wav;*.mp3;*.ogg\0"
    "Fonts (*.ttf;*.otf)\0*.ttf;*.otf\0"
    "Scripts (*.lua)\0*.lua\0"
    "Shaders (*.hlsl;*.glsl;*.vert;*.frag;*.shader)\0*.hlsl;*.glsl;*.vert;*.frag;*.shader\0"
    "All Files (*.*)\0*.*\0\0";


// --- CATEGORY LIST ---

const std::vector<FileCategory>& FileCategories::Get()
{
    static std::vector<FileCategory> categories = {
        { "3D Models", { ".fbx", ".obj", ".abc", ".usd" }, MODEL_FILTER },
        { "Textures",  { ".jpg", ".jpeg", ".png", ".tga", ".bmp", ".psd", ".dds", ".tiff", ".gif", ".hdr" }, TEXTURE_FILTER },
        { "Audio",     { ".wav", ".mp3", ".ogg" }, AUDIO_FILTER },
        { "Fonts",     { ".ttf", ".otf" }, FONT_FILTER },
        { "Scripts",   { ".lua" }, SCRIPT_FILTER },
        { "Shaders",   { ".hlsl", ".glsl", ".vert", ".frag", ".shader" }, SHADER_FILTER }
    };

    return categories;
}

const char* FileCategories::GetAllFilter()
{
    return ALL_FILTER;
}
