#include "pch.h"
#include "windialogs.h"
#include <shobjidl.h> 

std::string WinDialogs::OpenFile()
{
    OPENFILENAME ofn;   
    TCHAR szFile[MAX_PATH] = { 0 }; 

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "IMG Archive\0*.IMG\0";
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
    ofn.hwndOwner = NULL;
    ofn.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER;

    if (GetOpenFileName(&ofn))
    {
        return std::string(ofn.lpstrFile, 2048);
    }

    return "";
}

std::string WinDialogs::ExportFile(const char* fileName)
{
    OPENFILENAME ofn;   
    TCHAR szFile[MAX_PATH] = { 0 }; 
    strcpy(szFile, fileName);

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "All\0*.*\0";
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

size_t WinDialogs::SaveArchive(std::string &path)
{
    path.resize(255);
    OPENFILENAME ofn;   
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = path.data();
    ofn.nMaxFile = static_cast<DWORD>(path.capacity());
    ofn.lpstrFilter = "IMG Archive v1 (III, VC, BULLY)\0*.IMG\0IMG Archive v2 (SA)\0*.IMG\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

   GetSaveFileName(&ofn);
    return static_cast<size_t>(ofn.nFilterIndex);
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
        hr = pFileOpen->Show(NULL);

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