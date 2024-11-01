#include "pch.h"
#include "imgarchive.h"
#include "imgparser.h"
#include <filesystem>
#include "editor.h"

IMGArchive::IMGArchive(std::wstring Path, bool CreateNew)
{
    if (CreateNew)
    {
        this->FileName = Path;
        this->bCreateNew = true;
        AddLogMessage(L"Created archive");
        Parser = Parser::Get();
    }
    else
    {
        this->Path = Path;
        this->ImageVersion = GetVersion(Path);
        switch(this->ImageVersion)
        {
        case eImgVer::One:
        case eImgVer::Two:
            Parser = Parser::Get();
            break;
        default: 
            Parser = nullptr;
            break;
        }
    }

    if (Parser)
    {
        Parser->Open(this);
        UpdateSelectList(L"");
    }
} 

void IMGArchive::UpdateSelectList(const wchar_t *text)
{
    SelectedList.clear();
    for (EntryInfo &e : EntryList)
    {
        if (wcsstr(e.FileName, text))
        {
            SelectedList.push_back(&e);
        }
    }
}

std::wstring IMGArchive::GetFileType(const wchar_t* name)
{
    if (wcsstr(name, L".dff")) return L"Model";
    if (wcsstr(name, L".txd")) return L"Texture";
    if (wcsstr(name, L".col")) return L"Collision";
    if (wcsstr(name, L".ifp")) return L"Animation";
    if (wcsstr(name, L".ipl")) return L"Item placement";
    if (wcsstr(name, L".ide")) return L"Item defination";
    if (wcsstr(name, L".dat")) return L"Data";

    return std::filesystem::path(name).extension().wstring() + L" file";
}

void IMGArchive::ExportEntry(EntryInfo *pEntry, std::wstring filePath, bool log)
{
    if (Parser)
    {
        Parser->Export(this, pEntry, filePath, log);
    }
}

void IMGArchive::ExportAll(ArchiveInfo *pInfo)
{
    pInfo->pArc->ProgressBar.bInUse = true;
    size_t total =  pInfo->pArc->EntryList.size();
    
    for (size_t i = 0; i < total; ++i)
    {
        std::wstring path = std::format(L"{}\\{}", pInfo->path, pInfo->pArc->EntryList[i].FileName);
        pInfo->pArc->ExportEntry(&pInfo->pArc->EntryList[i], path, false);
        pInfo->pArc->ProgressBar.Percentage = (static_cast<float>(i)+1)/ static_cast<float>(total);

        if (pInfo->pArc->ProgressBar.bCancel)
        {
            pInfo->pArc->ProgressBar.bCancel = false;
            break;
        }
    }
    pInfo->pArc->AddLogMessage(L"Exported archive");
    pInfo->pArc->ProgressBar.bInUse = false;
    delete pInfo;
}

void IMGArchive::ExportSelected(ArchiveInfo *pInfo)
{
    pInfo->pArc->ProgressBar.bInUse = true;
    size_t total =  pInfo->pArc->EntryList.size();
    
    for (size_t i = 0; i < total; ++i)
    {
        if (pInfo->pArc->EntryList[i].bSelected)
        {
            std::wstring path = std::format(L"{}\\{}", pInfo->path, pInfo->pArc->EntryList[i].FileName);
            pInfo->pArc->ExportEntry(&pInfo->pArc->EntryList[i], path, false);
            pInfo->pArc->ProgressBar.Percentage = (static_cast<float>(i)+1)/ static_cast<float>(total);

            if (pInfo->pArc->ProgressBar.bCancel)
            {
                pInfo->pArc->ProgressBar.bCancel = false;
                break;
            }
        }
    }
    pInfo->pArc->AddLogMessage(L"Exported entries");
    pInfo->pArc->ProgressBar.bInUse = false;
    delete pInfo;
}

void IMGArchive::ImportEntry(const std::wstring &path, bool replace)
{
    if (Parser)
    {
        Parser->Import(this, path, replace);
    }
}

void IMGArchive::ImportEntries(ArchiveInfo *pInfo)
{
    std::vector<std::wstring> list;
    std::wstring temp = L"";
    std::wstring rootDir = L"";
    for (wchar_t c : pInfo->path)
    {
        if (c != '\0')
        {
            temp += c;
        }
        else
        {
            if (temp == L"")
            {
                break;
            }

            temp += L"\0";
            if (std::filesystem::is_directory(temp)) // skip folders
            {
                rootDir = std::move(temp) + L"\\";
            }
            else
            {
                list.push_back(std::move(rootDir + temp));
            }
            temp = L"";
        }
    }
    
    size_t total = list.size();
    for (size_t i = 0; i != total; ++i)
    {
        pInfo->pArc->ImportEntry(list[i], pInfo->removeExisting);
        pInfo->pArc->ProgressBar.Percentage = (static_cast<float>(i)+1)/ static_cast<float>(total);

        if (pInfo->pArc->ProgressBar.bCancel)
        {
            pInfo->pArc->ProgressBar.bCancel = false;
            pInfo->pArc->AddLogMessage(L"Rebuilding failed");
            pInfo->pArc->ProgressBar.bInUse = false;
        }
    }
    pInfo->pArc->UpdateSelectList(Editor::GetFilterText());
    pInfo->pArc->AddLogMessage(L"Imported entries");
    delete pInfo;
}

void IMGArchive::AddLogMessage(std::wstring &&message)
{
    LogList.push_back(std::move(message));
}

eImgVer IMGArchive::GetVersion(const std::wstring &Path)
{
    eImgVer imgVer = eImgVer::Unknown;

    // It's easier to detect v2, so let's do it first
    FILE *fp = _wfopen(Path.c_str(), L"rb");
    if (fp)
    {
        char ver[4];
        fread(ver, sizeof(ver), 1, fp);
        if (ver[0] == 'V' && ver[1] == 'E' && ver[2] == 'R' && ver[3] == '2')
        {
            imgVer = eImgVer::Two;
        }
        fclose(fp);
    }

    //  How to actually detect v1?
    std::filesystem::path fsPath = std::filesystem::path(Path);
    std::wstring dirPath = Path;
    dirPath.replace(dirPath.end()-3, dirPath.end(), L"dir");

    // if both .dir & .img exists with same name it's v1 YAY!
    if (std::filesystem::exists(fsPath) && std::filesystem::exists(dirPath))
    {
        imgVer = eImgVer::One;
    }

    return imgVer;
}

void IMGArchive::Save(ArchiveInfo *pInfo)
{
    if (pInfo->pArc->Parser)
    {
        pInfo->pArc->Parser->Save(pInfo);
    }
}