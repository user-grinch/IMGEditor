#include "pch.h"
#include "windialogs.h"
#include <shobjidl.h>
#include <commdlg.h>
#include <string>
#include <iostream>
#include <locale>
#include <codecvt>

std::wstring WinDialogs::OpenFile()
{
    OPENFILENAMEW ofn;   
    wchar_t szFile[MAX_PATH] = { 0 }; 

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"IMG Archive\0*.IMG\0";  
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn)) 
    {
        return std::wstring(ofn.lpstrFile);
    }

    return L"";
}

std::wstring WinDialogs::ImportFiles()
{
    OPENFILENAMEW ofn = { sizeof ofn };
    ofn.nMaxFile = 1 << 20;
    ofn.lpstrFile = (LPWSTR) calloc(ofn.nMaxFile, sizeof(wchar_t)); 
    ofn.hwndOwner = NULL;
    ofn.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER;

    if (ofn.lpstrFile && GetOpenFileNameW(&ofn)) 
    {
        std::wstring paths = std::wstring(ofn.lpstrFile, ofn.nMaxFile);
        free(ofn.lpstrFile);
        return paths;
    }

    return L"";
}

std::wstring WinDialogs::ExportFile(const wchar_t* fileName)
{
    OPENFILENAMEW ofn;   
    wchar_t szFile[MAX_PATH] = { 0 }; 
    wcscpy_s(szFile, fileName); 

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"All\0*.*\0"; 
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileNameW(&ofn)) 
    {
        return std::wstring(ofn.lpstrFile);
    }

    return L"";
}

size_t WinDialogs::SaveArchive(std::wstring &path)
{
    path.resize(255);
    OPENFILENAMEW ofn;   
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = path.data();
    ofn.nMaxFile = static_cast<DWORD>(path.capacity());
    ofn.lpstrFilter = L"IMG Archive v1 (III, VC, BULLY)\0*.IMG\0IMG Archive v2 (SA)\0*.IMG\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileNameW(&ofn)) 
    {
        path = ofn.lpstrFile; 
        return static_cast<size_t>(ofn.nFilterIndex);
    }
    return 0; 
}

std::wstring WinDialogs::SaveFolder()
{
    IFileOpenDialog *pFileOpen;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, 
            IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

    if (SUCCEEDED(hr))
    {
        pFileOpen->SetOptions(FOS_PICKFOLDERS);
        
        
        hr = pFileOpen->Show(NULL);

        
        if (SUCCEEDED(hr))
        {
            IShellItem *pItem;
            hr = pFileOpen->GetResult(&pItem);
            if (SUCCEEDED(hr))
            {
                PWSTR pszFilePath;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                
                if (SUCCEEDED(hr))
                {
                    std::wstring wstr(pszFilePath);
                    CoTaskMemFree(pszFilePath);
                    pItem->Release();
                    pFileOpen->Release();
                    return wstr; 
                }
                pItem->Release();
            }
        }
        pFileOpen->Release();
    }
    CoUninitialize();
    return L""; 
}