#include "Utils/wcharUtils.h"

// Choose dst at least (50 + len(localPath))
void getGraphicsAssetPath(wchar_t* fullPath, size_t dst_capacity, const wchar_t* localPath) {
    wcsncpy_s(fullPath, dst_capacity, GRAPHICS_ASSETS_DIR, _TRUNCATE);
    wcscat_s(fullPath, dst_capacity, localPath);
    return;
}

/*
enum class AssetLocation {
    Graphics, Engine, Editor, Game
};

// Choose dst at least (50 + len(s))
void getAssetFullPath(wchar_t* fullPath, size_t dst_capacity, const wchar_t* localPath,
    AssetLocation loc, wchar_t* gamePath = nullptr) {
    switch (loc)
    {
    case AssetLocation::Graphics:
        wcscpy(fullPath, GRAPHICS_ASSETS_DIR);
        break;
    case AssetLocation::Engine:
        wcscpy(fullPath, ENGINE_ASSETS_DIR);
        break;
    case AssetLocation::Editor:
        wcscpy(fullPath, EDITOR_ASSETS_DIR);
        break;
    case AssetLocation::Game:
        wcscpy(fullPath, gamePath);
        break;
    default:
        break;
    }
    wcscat(fullPath, localPath);
}
*/