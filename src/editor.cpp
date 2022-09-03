#include "pch.h"
#include "editor.h"
#include "widget.h"
#include "windialogs.h"
#define MENU_VERSION 1.0

void Editor::AboutPopUp()
{
    ImGui::Columns(2, nullptr, false);
    ImGui::Text("Author: Grinch_");
    ImGui::Text("Version: %.2f", MENU_VERSION);
    ImGui::Spacing();
    float h = ImGui::GetFrameHeight();
    float w = ImGui::GetWindowContentRegionWidth()/2 - ImGui::GetStyle().ItemSpacing.x;
    if (ImGui::Button("GitHub", ImVec2(w, h)))
    {
        ShellExecute(nullptr, "open", "https://github.com/user-grinch/IMGEditor/", nullptr, nullptr, SW_SHOWNORMAL);
    }
    ImGui::NextColumn();
    ImGui::Text("ImGui: %s", ImGui::GetVersion());
    ImGui::Text("Build: %s", __DATE__ );
    ImGui::Spacing();
    if (ImGui::Button("Patreon", ImVec2(w, h)))
    {
        ShellExecute(nullptr, "open", "https://www.patreon.com/grinch_", nullptr, nullptr, SW_SHOWNORMAL);
    }
    ImGui::Columns(1);
    ImGui::Spacing();
    ImGui::Text("Copyright Grinch_ 2022. All rights reserved.");
}

void Editor::ProcessMenuBar()
{
    if (ImGui::BeginMenu("File"))
    {
        ImGui::MenuItem("New");
        if (ImGui::MenuItem("Open..."))
        {
            std::string path = WinDialogs::OpenFile();
            if (path != "")
            {
                if (pIMG)
                {
                    delete pIMG;
                }
                pIMG = new IMGArchive(std::move(path));
            }
        }
        ImGui::MenuItem("Save");
        ImGui::MenuItem("Save as...");
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Edit"))
    {
        ImGui::MenuItem("Import");
        ImGui::MenuItem("Export all");
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Help"))
    {
        if (ImGui::MenuItem("Check for update"))
        {
            ShellExecute(nullptr, "open", "https://github.com/user-grinch/IMGEditor/releases", nullptr, nullptr, SW_SHOWNORMAL);
        }
        if (ImGui::MenuItem("About"))
        {
            if (pApp)
            {
                pApp->SetPopup(AboutPopUp);
            }
        }
        ImGui::EndMenu();
    }
}

void Editor::ProcessContextMenu()
{
    if (!pContextEntry)
    {
        return;
    }

    ImVec2 pos = ImGui::GetMousePos();
    ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.5f);
    ImGui::SetNextWindowFocus();
    if (ImGui::Begin("##Context", NULL, flags))
    {
        if (ImGui::MenuItem("Delete"))
        {
        }

        if (ImGui::MenuItem("Extract"))
        {
        }

        if (ImGui::MenuItem("Rename"))
        {
            for (DirEntry &e : pIMG->Entries)
            {
                e.rename  = false;
            }
            pContextEntry->rename = true;
        }
        ImGui::End();
    }

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsWindowFocused())
    {
        pContextEntry = nullptr;
    }
    ImGui::PopStyleVar();
}

void Editor::ProcessWindow()
{
    // Search bar
    std::string text = std::format("Total Items: {}", pIMG ? pIMG->Entries.size() : 0);
    Widget::Filter("Search", filter, text.c_str());

    if (ImGui::BeginTable("ListedItems", 2, ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        // Freeze the header row
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 50);
        ImGui::TableHeadersRow();
        if (pIMG)
        {   
            for (DirEntry &entry : pIMG->Entries)
            {
                if (filter.PassFilter(entry.name))
                {
                    ImGui::TableNextRow();
                    static bool prevSelected = false;
                    if (prevSelected)
                    {
                        ImGui::PopStyleColor(2);
                        prevSelected = false;
                    }
                    ImGui::TableNextColumn();

                    // Renaming system
                    if (entry.rename)
                    {
                        ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
                        ImGui::InputText("##Rename", entry.name, sizeof(entry.name));
                        if (ImGui::IsKeyPressed(VK_RETURN))
                        {
                            entry.rename = false;
                        }
                    }
                    else
                    {
                        if (ImGui::Selectable(entry.name))
                        {
                            // selection & renanme disable
                            for (DirEntry &e : pIMG->Entries)
                            {
                                // allow multiselect when ctrl is pressed
                                if (!(ImGui::IsKeyDown(VK_LCONTROL) || ImGui::IsKeyDown(VK_RCONTROL)))
                                {
                                    e.selected  = false;
                                }
                                e.rename  = false;
                            }

                            entry.selected = true;
                        }
                        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
                        {
                            pContextEntry = pContextEntry ? nullptr : &entry;
                        }
                    }

                    if (entry.selected || ImGui::IsItemHovered())
                    {
                        ImGui::PushStyleColor(ImGuiCol_TableRowBg, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
                        ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
                        prevSelected = true;
                    }

                    ImGui::TableNextColumn();
                    ImGui::Text("%d kb", entry.size*2);
                }
            }
            ProcessContextMenu();
        }
        ImGui::EndTable();
    }
}

void Editor::Run()
{
    Ui::Specification spec;
    spec.Name = "IMG Editor";
    spec.MenuBarFunc = Editor::ProcessMenuBar;
    spec.Size = { 350, 500 };

    spec.LayerFunc = Editor::ProcessWindow;

    pApp = new Ui::Application(spec);
    pApp->Run();
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    Editor::Run();
    return 0;
}