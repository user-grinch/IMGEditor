#include "pch.h"
#include "windialogs.h"
#include <shobjidl.h> 

std::string WinDialogs::OpenFile()
{
    OPENFILENAME ofn;   
    TCHAR szFile[260] = { 0 }; 

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetActiveWindow();
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

std::string WinDialogs::ImportFiles()
{
    OPENFILENAME ofn = { sizeof ofn };
    char file[2048];
    file[0] = '\0';
    ofn.lpstrFile = file;
    ofn.nMaxFile = 2048;
    ofn.hwndOwner = GetActiveWindow();
    ofn.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER;

    if (GetOpenFileName(&ofn))
    {
        return std::string(ofn.lpstrFile, 2048);
    }

    return "";
}

std::string WinDialogs::SaveFile(std::string fileName)
{
    fileName.resize(255);
    OPENFILENAME ofn;   
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetActiveWindow();
    ofn.lpstrFile = fileName.data();
    ofn.nMaxFile = static_cast<DWORD>(fileName.capacity());
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

std::string WinDialogs::SaveFolder()
{
    IFileOpenDialog *pFileOpen;
    // Create the FileOpenDialog object.
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, 
            IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

    if (SUCCEEDED(hr))
    {
        pFileOpen->SetOptions(FOS_PICKFOLDERS);
        
        // Show the Open dialog box.
        hr = pFileOpen->Show(GetActiveWindow());

        // Get the file name from the dialog box.
        if (SUCCEEDED(hr))
        {
            IShellItem *pItem;
            hr = pFileOpen->GetResult(&pItem);
            if (SUCCEEDED(hr))
            {
                PWSTR pszFilePath;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                // Display the file name to the user.
                if (SUCCEEDED(hr))
                {
                    std::wstring wstr(pszFilePath);
                    std::string str(wstr.begin(), wstr.end());
                    CoTaskMemFree(pszFilePath);
                    return str;
                }
                pItem->Release();
            }
        }
        pFileOpen->Release();
    }
    CoUninitialize();
    return "";
}