#include "pch.h"
#include "editor.h"
#include "widget.h"
#include "windialogs.h"
#define MENU_VERSION "1.0"

void Editor::AboutPopUp()
{
    ImGui::Columns(2, nullptr, false);
    ImGui::Text("Author: Grinch_");
    ImGui::Text("Version: " MENU_VERSION);
    ImGui::Spacing();

    if (ImGui::Button("GitHub", Widget::CalcSize(2)))
    {
        ShellExecute(nullptr, "open", "https://github.com/user-grinch/IMGEditor/", nullptr, nullptr, SW_SHOWNORMAL);
    }
    ImGui::NextColumn();
    ImGui::Text("ImGui: %s", ImGui::GetVersion());
    ImGui::Text("Build: %s", __DATE__ );
    ImGui::Spacing();
    if (ImGui::Button("Patreon", Widget::CalcSize(2)))
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
                Archives.push_back(std::move(IMGArchive(std::move(path))));
            }
        }
        ImGui::MenuItem("Save");
        ImGui::MenuItem("Save as...");
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Options"))
    {
        ImGui::MenuItem("Color");
        ImGui::MenuItem("Compact mode");
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Help"))
    {
        if (ImGui::MenuItem("Check for update"))
        {
            ShellExecute(nullptr, "open", "https://github.com/user-grinch/IMGEditor/releases", nullptr, nullptr, SW_SHOWNORMAL);
        }
        ImGui::MenuItem("Controls");
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

        if (ImGui::MenuItem("Export"))
        {
        }

        if (ImGui::MenuItem("Rename"))
        {
            // for (DirEntry &e : pIMG->Entries)
            // {
            //     e.rename  = false;
            // }
            // pContextEntry->rename = true;
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
    float windowWidth = ImGui::GetWindowContentRegionWidth();
    ImGuiStyle &style = ImGui::GetStyle();
    ImGui::Columns(2, NULL, false);
    ImGui::SetColumnWidth(0, windowWidth/2);

    if (ImGui::BeginTabBar("Archives", ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_Reorderable))
    {
        for (IMGArchive &archive : Archives)
        {  
            bool t = true;
            if (ImGui::BeginTabItem(archive.Path.filename().stem().string().c_str(), &t))
            {
                // Search bar
                ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - style.ItemSpacing.x - style.WindowPadding.x);
                Widget::Filter("##Search", filter, "Search");
                if (ImGui::BeginTable("ListedItems", 2, ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
                {
                    // Freeze the header row
                    ImGui::TableSetupScrollFreeze(0, 1);
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 50);
                    ImGui::TableHeadersRow();
                    for (DirEntry &entry : archive.Entries)
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
                                    for (DirEntry &e : archive.Entries)
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
                    ImGui::EndTable();
                }
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }
    ImGui::NextColumn();
    // ImGui::Text("Total lines: %d", archive.Entries.size(): 0);
    ImGui::Dummy(ImVec2(0, ImGui::GetStyle().ItemSpacing.y-2));
    ImVec2 sz = Widget::CalcSize(3, true, true);

    ImGui::Button("Import", sz);
    ImGui::SameLine();
    ImGui::Button("Export", sz);
    ImGui::SameLine();
    ImGui::Button("Export all", sz);

    ImGui::Button("Dump list", sz);
    ImGui::SameLine();
    ImGui::Button("Select all", sz);
    ImGui::SameLine();
    ImGui::Button("Merge", sz);

    ImGui::Columns(1);
}

void Editor::Run()
{
    Ui::Specification spec;
    spec.Name = "IMG Editor v" MENU_VERSION;
    spec.MenuBarFunc = Editor::ProcessMenuBar;
    spec.Size = { 600, 500 };

    spec.LayerFunc = Editor::ProcessWindow;

    pApp = new Ui::Application(spec);
    pApp->Run();
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    Editor::Run();
    return 0;
}