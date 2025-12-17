#include <UI/FontStyles.h>

#include <imgui.h>
#include <EASTL/string.h>
#include <EASTL/algorithm.h>

#include <Utils/StringUtils.h>

namespace EditorUI {

    ImFont* FontStyles::s_fonts[(int)FontStyles::Style::Count] = { nullptr };

    static const char* REGULAR_NAME = "Roboto-Regular.ttf";
    static const char* BOLD_NAME    = "Roboto-Bold.ttf";

    bool FontStyles::Init(ImGuiIO& io)
    {
        eastl::string basePath = MakeEngineAssetPath_String("Fonts/").c_str();

        // keep a simple fallback: make sure default font exists
        if (io.Fonts->Fonts.empty())
            io.Fonts->AddFontDefault();

        // To-do: change to Roboto?
        // Change font to Arial to support Russian
        ImFont* fontArial = io.Fonts->AddFontFromFileTTF(
            MakeEngineAssetPath_String("Fonts/Arial.ttf").c_str(),
            13.0f);
        io.FontDefault = fontArial;

        // helper to build path
        auto buildPath = [&](const char* filename) {
            if (basePath.empty())
                return eastl::string(filename);

            eastl::string p = basePath;
            // ensure trailing separator
            if (p.back() != '/' && p.back() != '\\') p += '/';
            p += filename;
            return p;
        };

        // Try primary fonts
        s_fonts[(int)Style::Regular] = io.Fonts->AddFontFromFileTTF(buildPath(REGULAR_NAME).c_str(), 16.0f);
        s_fonts[(int)Style::Title]   = io.Fonts->AddFontFromFileTTF(buildPath(BOLD_NAME).c_str(),    22.0f);

        // If primary fonts aren't found fall back to default font
        if (!s_fonts[(int)Style::Regular]) s_fonts[(int)Style::Regular] = io.Fonts->Fonts.front();
        if (!s_fonts[(int)Style::Title])   s_fonts[(int)Style::Title]   = io.Fonts->Fonts.front();

        // Create headers by reusing regular/bold fonts at different sizes
        // Note: ImFont objects returned by AddFontFromFileTTF are distinct fonts.
        // Many backends prefer to register multiple sizes; for simplicity, we'll
        // load additional sizes from the same TTF (if available) or reuse a font.
        s_fonts[(int)Style::Header1] = io.Fonts->AddFontFromFileTTF(buildPath(BOLD_NAME).c_str(), 20.0f);
        if (!s_fonts[(int)Style::Header1]) s_fonts[(int)Style::Header1] = s_fonts[(int)Style::Title];

        s_fonts[(int)Style::Header2] = io.Fonts->AddFontFromFileTTF(buildPath(BOLD_NAME).c_str(), 18.0f);
        if (!s_fonts[(int)Style::Header2]) s_fonts[(int)Style::Header2] = s_fonts[(int)Style::Title];

        s_fonts[(int)Style::Header3] = io.Fonts->AddFontFromFileTTF(buildPath(REGULAR_NAME).c_str(), 15.0f);
        if (!s_fonts[(int)Style::Header3]) s_fonts[(int)Style::Header3] = s_fonts[(int)Style::Regular];

        return true;
    }

    ImFont* FontStyles::Get(Style s)
    {
        return s_fonts[(int)s];
    }

    void FontStyles::Push(Style s)
    {
        ImFont* f = Get(s);
        if (f) ImGui::PushFont(f);
    }

    void FontStyles::Pop()
    {
        ImGui::PopFont();
    }

    void FontStyles::Shutdown()
    {
        // ImGui owns fonts; we just clear our pointers
        for (int i = 0; i < (int)Style::Count; ++i)
            s_fonts[i] = nullptr;
    }

}
