#include "application.h"
#include <windows.h>
#include "imgmgr.h"

static IMGArchive *pIMG = nullptr;
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
                if (pIMG)
                {
                    delete pIMG;
                }

                pIMG = new IMGArchive(ofn.lpstrFile);
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
        ImGui::MenuItem("Check for update");
        ImGui::MenuItem("About");
        ImGui::EndMenu();
    }
}

int main(int, char**)
{
    Ui::Specification spec;
    spec.Name = "IMG Editor";
    spec.MenuBarFunc = MenubarFunc;
    spec.Size = { 350, 500 };

    spec.LayerFunc = []()
    {
        static ImGuiTextFilter filter;
        filter.Draw("Search", ImGui::GetWindowContentRegionWidth());
        if (strlen(filter.InputBuf) == 0)
        {
            ImDrawList* drawlist = ImGui::GetWindowDrawList();

            ImVec2 min = ImGui::GetItemRectMin();
            min.x += ImGui::GetStyle().FramePadding.x;
            min.y += ImGui::GetStyle().FramePadding.y;
            drawlist->AddText(min, ImGui::GetColorU32(ImGuiCol_TextDisabled), "Search");
        }

        if (ImGui::BeginTable("ListedItems", 2, ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Offset", ImGuiTableColumnFlags_WidthFixed, 75);
            ImGui::TableHeadersRow();
            
            if (pIMG)
            {   
                for (DirEntry &entry : pIMG->Entries)
                {
                    if (filter.PassFilter(entry.name))
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::Selectable(entry.name);
                        ImGui::TableNextColumn();
                        ImGui::Text("%d", entry.offset);
                    }
                }
            }
            ImGui::EndTable();
        }
    };

    Ui::Application App = Ui::Application(spec);
    App.Run();

    return 0;
}