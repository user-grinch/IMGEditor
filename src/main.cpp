#define VERSION 1.0
#include "application.h"
#include <windows.h>
#include <format>
#include "imgmgr.h"

static IMGArchive *g_pIMG = nullptr;
static Ui::Application *g_pApp = nullptr;

void MenubarFunc()
{
    if (ImGui::BeginMenu("File"))
    {
        ImGui::MenuItem("New");
        if (ImGui::MenuItem("Open..."))
        {
            OPENFILENAME ofn;   
            TCHAR szFile[260] = { 0 }; 

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = 0;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = "All\0*.*\0IMG Archive\0*.IMG\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileName(&ofn) == TRUE)
            {
                if (g_pIMG)
                {
                    delete g_pIMG;
                }

                g_pIMG = new IMGArchive(ofn.lpstrFile);
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
            if (g_pApp)
            {
                g_pApp->SetPopup([](){
                    ImGui::Columns(2, nullptr, false);
                    ImGui::Text("Author: Grinch_");
                    ImGui::Text("Version: %.2f", VERSION);
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
                });
            }
        }
        ImGui::EndMenu();
    }
}

void WindowFunc()
{
    // Search bar
    static ImGuiTextFilter filter;
    std::string text = g_pIMG ? std::format("Search (Total: {})", g_pIMG->Entries.size()) : "Search";
    filter.Draw(text.c_str(), ImGui::GetWindowContentRegionWidth());
    if (strlen(filter.InputBuf) == 0)
    {
        ImDrawList* drawlist = ImGui::GetWindowDrawList();

        ImVec2 min = ImGui::GetItemRectMin();
        min.x += ImGui::GetStyle().FramePadding.x;
        min.y += ImGui::GetStyle().FramePadding.y;
        drawlist->AddText(min, ImGui::GetColorU32(ImGuiCol_TextDisabled), text.c_str());
    }

    if (ImGui::BeginTable("ListedItems", 2, ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        // Freeze the header row
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 100);
        ImGui::TableHeadersRow();
        if (g_pIMG)
        {   
            static DirEntry *pContextEntry = nullptr;
            if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
            {
                pContextEntry =  nullptr;
            }
            for (DirEntry &entry : g_pIMG->Entries)
            {
                if (filter.PassFilter(entry.name))
                {
                    /*
                        TableNextRow seems to draw the table row
                        Kinda weird but have to Call PopStyleColor like this
                    */
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
                            for (DirEntry &e : g_pIMG->Entries)
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

                    ImGui::TableNextColumn();

                    std::string type = "Unknown";
                    if (strstr(entry.name, ".dff"))
                    {
                        type = "Model";
                    } 
                    if (strstr(entry.name, ".txd")) 
                    {
                        type = "Texture";
                    }
                    if (strstr(entry.name, ".ipl")) 
                    {
                        type = "Item placement";
                    }
                    if (strstr(entry.name, ".ide")) 
                    {
                        type = "Item defination";
                    }
                    if (strstr(entry.name, ".ifp"))
                    {
                        type = "Animation";
                    }
                    if (strstr(entry.name, ".col"))
                    {
                        type = "Collision";
                    }

                    ImGui::Text(type.c_str());

                    if (entry.selected)
                    {
                        ImGui::PushStyleColor(ImGuiCol_TableRowBg, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
                        ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
                        prevSelected = true;
                    }
                }
            }

            if (pContextEntry && ImGui::BeginPopupContextWindow())
            {
                if (ImGui::MenuItem("Delete"))
                {
                }

                if (ImGui::MenuItem("Extract"))
                {
                }

                if (ImGui::MenuItem("Rename"))
                {
                    for (DirEntry &e : g_pIMG->Entries)
                    {
                        e.rename  = false;
                    }
                    pContextEntry->rename = true;
                }

                if (ImGui::MenuItem("Close"))
                {
                    pContextEntry = nullptr;
                }
                ImGui::EndPopup();
            }
        }
        ImGui::EndTable();
    }
}

int main(int, char**)
{
    Ui::Specification spec;
    spec.Name = "IMG Editor";
    spec.MenuBarFunc = MenubarFunc;
    spec.Size = { 350, 500 };

    spec.LayerFunc = WindowFunc;

    g_pApp = new Ui::Application(spec);
    g_pApp->Run();

    return 0;
}