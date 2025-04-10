//==============================================================================
// miMarkup : Markup Source
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/miMarkup
//==============================================================================
#include "MarkupPCH.h"
#include "miMarkup.h"
#include "Markup.h"

static std::string input;
static std::string output;
static miMarkup* markup = nullptr;
//------------------------------------------------------------------------------
void Markup::Initialize()
{
    input.clear();
    output.clear();
    markup = nullptr;
}
//------------------------------------------------------------------------------
void Markup::Shutdown()
{
    input.clear();
    input.shrink_to_fit();
    output.clear();
    output.shrink_to_fit();
    delete markup;
    markup = nullptr;
}
//------------------------------------------------------------------------------
static bool Node(miMarkup& markup)
{
    bool updated = false;

    ImGui::Selectable("{");
    ImGui::Indent();
    for (auto& child : markup)
    {
        // "Name" : "Value"
        if (child.name.size() && child.value.size())
        {
            char text[128];
            snprintf(text, 128, "%s : %s", child.name.c_str(), child.value.c_str());
            ImGui::Selectable(text);
            continue;
        }

        // "Value"
        if (child.value.size())
        {
            ImGui::Selectable(child.value.c_str());
            continue;
        }

        // "Object", {...}
        if (child.name.size())
        {
            ImGui::Selectable(child.name.c_str());
            if (child.size())
            {
                updated |= Node(child);
            }
            continue;
        }

        // "Array" : [...]
        if (child.size())
        {
            updated |= Node(child);
        }
    }
    ImGui::Unindent();
    ImGui::Selectable("}");

    return updated;
}
//------------------------------------------------------------------------------
bool Markup::Update(const UpdateData& updateData, bool& show)
{
    if (show == false)
        return false;

    bool updated = false;
    ImGui::SetNextWindowSize(ImVec2(1280.0f, 872.0f), ImGuiCond_Appearing);
    if (ImGui::Begin("Markup", &show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking))
    {
        float height = ImGui::GetTextLineHeightWithSpacing() * 16;
        if (ImGui::InputTextMultiline("    INPUT", input, ImVec2(ImGui::GetWindowWidth() - 16.0f, height)))
        {
            delete markup;
            markup = miMarkupJSONLoad(input);
            if (markup)
            {
                output = miMarkupJSONSave(*markup);
            }
        }
        ImGui::Separator();
        if (ImGui::BeginChild("Node", ImVec2(0, height)))
        {
            if (markup)
            {
                updated |= Node(*markup);
            }
            ImGui::EndChild();
        }
        ImGui::Separator();
        ImGui::InputTextMultiline("    OUTPUT", output, ImVec2(ImGui::GetWindowWidth() - 16.0f, height), ImGuiInputTextFlags_ReadOnly);
    }
    ImGui::End();

    return updated;
}
//------------------------------------------------------------------------------
