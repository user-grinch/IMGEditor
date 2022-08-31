#include "application.h"

int main(int, char**)
{
    Ui::Specification spec;
    spec.Name = "IMG Editor";
    spec.MenuBarFunc = []()
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::MenuItem("New");
            ImGui::MenuItem("Open...");
            ImGui::MenuItem("Save");
            ImGui::MenuItem("Save as...");
            ImGui::MenuItem("Close");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("About"))
        {
            ImGui::MenuItem("Credits");
            ImGui::MenuItem("Check for update");
            ImGui::EndMenu();
        }
    };
    spec.LayerFunc = []()
    {
        ImGui::Text("THIS IS A SIMPLE TEST");
        ImGui::Spacing();
        ImGui::Text("TEST2");
    };

    Ui::Application App = Ui::Application(spec);
    App.Run();

    return 0;
}