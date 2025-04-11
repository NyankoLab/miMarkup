//==============================================================================
// miMarkup : MarkupEntry Source
//
// Copyright (c) 2025 TAiGA
// https://github.com/metarutaiga/miMarkup
//==============================================================================
#include "MarkupPCH.h"
#include "Markup.h"

#if _CPPUNWIND == 0 && __cpp_exceptions == 0
#if defined(xxWINDOWS)
#include <windows.h>
#endif
#define try         if (1) {
#define catch(x)    } else { std::exception e; std::string msg;
#define throw       }
#endif
#include <ImGuiFileDialog/ImGuiFileDialog.cpp>

#define PLUGIN_NAME     "Markup"

//------------------------------------------------------------------------------
moduleAPI const char* Create(const CreateData& createData)
{
    Markup::Initialize();
    return PLUGIN_NAME;
}
//------------------------------------------------------------------------------
moduleAPI void Shutdown(const ShutdownData& shutdownData)
{
    Markup::Shutdown();
}
//------------------------------------------------------------------------------
moduleAPI void Message(const MessageData& messageData)
{
    if (messageData.length == 1)
    {
        switch (xxHash(messageData.data[0]))
        {
        case xxHash("INIT"):
            break;
        case xxHash("SHUTDOWN"):
            break;
        default:
            break;
        }
    }
}
//------------------------------------------------------------------------------
moduleAPI bool Update(const UpdateData& updateData)
{
    static bool showMarkup = false;

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Experimental"))
        {
            if (ImGui::GetCursorPos().y > ImGui::GetTextLineHeightWithSpacing())
                ImGui::Separator();
            ImGui::MenuItem("Markup", nullptr, &showMarkup);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    bool updated = false;
    updated |= Markup::Update(updateData, showMarkup);

    return updated;
}
//------------------------------------------------------------------------------
moduleAPI void Render(const RenderData& renderData)
{

}
//------------------------------------------------------------------------------
