#include "pch.h"
#include "windialogs.h"

std::string WinDialogs::OpenFile()
{
    OPENFILENAME ofn;   
    TCHAR szFile[260] = { 0 }; 

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = 0;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "IMG Archive\0*.IMG\0All\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn))
    {
        return std::string(ofn.lpstrFile);
    }

    return "";
}

std::string WinDialogs::SaveFile(std::string fileName)
{
    fileName.resize(255);
    OPENFILENAME ofn;   
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = 0;
    ofn.lpstrFile = fileName.data();
    ofn.nMaxFile = fileName.capacity();
    ofn.lpstrFilter = "IMG Archive\0*.IMG\0All\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn))
    {
        return std::string(ofn.lpstrFile);
    }

    return "";
}