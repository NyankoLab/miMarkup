//==============================================================================
// miMarkup : Markup Source
//
// Copyright (c) 2025 TAiGA
// https://github.com/metarutaiga/miMarkup
//==============================================================================
#include "MarkupPCH.h"
#include "miMarkup.h"
#include "Markup.h"

static std::string input = R"({
  "first_name": "John",
  "last_name": "Smith",
  "is_alive": true,
  "age": 27,
  "address": {
    "street_address": "21 2nd Street",
    "city": "New York",
    "state": "NY",
    "postal_code": "10021-3100"
  },
  "phone_numbers": [
    {
      "type": "home",
      "number": "212 555-1234"
    },
    {
      "type": "office",
      "number": "646 555-4567"
    }
  ],
  "children": [
    "Catherine",
    "Thomas",
    "Trevor"
  ],
  "spouse": null
})";
static std::string output;
static miMarkup* markup = nullptr;
//------------------------------------------------------------------------------
void Markup::Initialize()
{
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
#define Q "##" xxStringify(__LINE__)
//------------------------------------------------------------------------------
bool Markup::Update(const UpdateData& updateData, bool& show)
{
    if (show == false)
        return false;

    bool updated = false;
    ImGui::SetNextWindowSize(ImVec2(1280.0f, 892.0f), ImGuiCond_Appearing);
    if (ImGui::Begin("Markup", &show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking))
    {
        static int inputType = 'JSON';
        static int outputType = 'XML ';
        bool build = false;

        float width = ImGui::GetWindowWidth() / 2.0f - 16.0f;
        float height = ImGui::GetTextLineHeightWithSpacing() * 24;

        ImGui::Columns(2);
        if (ImGui::RadioButton("JSON" Q, inputType == 'JSON')) { inputType = 'JSON'; build = true; } ImGui::SameLine();
        ImGui::NewLine();
        if (ImGui::InputTextMultiline("" Q, input, ImVec2(width, height)))
            build = true;
        ImGui::NextColumn();
        if (ImGui::RadioButton("JSON" Q, outputType == 'JSON')) { outputType = 'JSON'; build = true; } ImGui::SameLine();
        if (ImGui::RadioButton("TOML" Q, outputType == 'TOML')) { outputType = 'TOML'; build = true; } ImGui::SameLine();
        if (ImGui::RadioButton("XML" Q,  outputType == 'XML ')) { outputType = 'XML '; build = true; } ImGui::SameLine();
        if (ImGui::RadioButton("YAML" Q, outputType == 'YAML')) { outputType = 'YAML'; build = true; } ImGui::SameLine();
        ImGui::NewLine();
        ImGui::InputTextMultiline("" Q, output, ImVec2(width, height), ImGuiInputTextFlags_ReadOnly);
        ImGui::Columns(1);

        ImGui::Separator();
        if (ImGui::BeginChild("Node", ImVec2(0, height)))
        {
            if (markup)
            {
                updated |= Node(*markup);
            }
            ImGui::EndChild();
        }

        if (build)
        {
            delete markup;
            switch (inputType)
            {
            case 'JSON':
                markup = miMarkup::fromJSON(input);
                break;
            default:
                markup = nullptr;
                break;
            }
            if (markup)
            {
                switch (outputType)
                {
                case 'JSON':
                    output = (*markup).toJSON();
                    break;
                case 'TOML':
                    output = (*markup).toTOML();
                    break;
                case 'XML ':
                    output = (*markup).toXML();
                    break;
                case 'YAML':
                    output = (*markup).toYAML();
                    break;
                default:
                    output.clear();
                    break;
                }
            }
        }
    }
    ImGui::End();

    return updated;
}
//------------------------------------------------------------------------------
