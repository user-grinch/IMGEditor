#include "pch.h"
#include "imgarchive.h"
#include "imgparser.h"
#include <filesystem>

IMGArchive::IMGArchive(std::string Path, bool CreateNew)
{
    if (CreateNew)
    {
        this->FileName = Path;
        this->bCreateNew = true;
        AddLogMessage("Created archive");
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
        UpdateSelectList("");
    }
} 

void IMGArchive::UpdateSelectList(const char *text)
{
    SelectedList.clear();
    for (EntryInfo &e : EntryList)
    {
        if (strstr(e.FileName, text))
        {
            SelectedList.push_back(&e);
        }
    }
}

std::string IMGArchive::GetFileType(const char* name)
{
    if (strstr(name, ".dff")) return "Model";
    if (strstr(name, ".txd")) return "Texture";
    if (strstr(name, ".col")) return "Collision";
    if (strstr(name, ".ifp")) return "Animation";
    if (strstr(name, ".ipl")) return "Item placement";
    if (strstr(name, ".ide")) return "Item defination";
    if (strstr(name, ".dat")) return "Data";

    return std::filesystem::path(name).extension().string() + " file";
}

void IMGArchive::ExportEntry(EntryInfo *pEntry, std::string filePath, bool log)
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
        std::string path = std::format("{}\\{}", pInfo->path, pInfo->pArc->EntryList[i].FileName);
        pInfo->pArc->ExportEntry(&pInfo->pArc->EntryList[i], path, false);
        pInfo->pArc->ProgressBar.Percentage = (static_cast<float>(i)+1)/ static_cast<float>(total);

        if (pInfo->pArc->ProgressBar.bCancel)
        {
            pInfo->pArc->ProgressBar.bCancel = false;
            break;
        }
    }
    pInfo->pArc->AddLogMessage("Exported archive");
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
            std::string path = std::format("{}\\{}", pInfo->path, pInfo->pArc->EntryList[i].FileName);
            pInfo->pArc->ExportEntry(&pInfo->pArc->EntryList[i], path, false);
            pInfo->pArc->ProgressBar.Percentage = (static_cast<float>(i)+1)/ static_cast<float>(total);

            if (pInfo->pArc->ProgressBar.bCancel)
            {
                pInfo->pArc->ProgressBar.bCancel = false;
                break;
            }
        }
    }
    pInfo->pArc->AddLogMessage("Exported entries");
    pInfo->pArc->ProgressBar.bInUse = false;
    delete pInfo;
}

void IMGArchive::ImportEntry(const std::string &path, bool replace)
{
    if (Parser)
    {
        Parser->Import(this, path, replace);
    }
}

void IMGArchive::ImportEntries(const std::string &path, bool replace)
{
    std::string temp = "";
    for (char c : path)
    {
        if (c != '\0')
        {
            temp += c;
        }
        else
        {
            if (temp == "")
            {
                break;
            }

            temp += "\0";
            ImportEntry(temp, replace);
            temp = "";
        }
    }
}

void IMGArchive::AddLogMessage(std::string &&message)
{
    LogList.push_back(std::move(message));
}

eImgVer IMGArchive::GetVersion(const std::string &Path)
{
    eImgVer imgVer = eImgVer::Unknown;

    // It's easier to detect v2, so let's do it first
    FILE *fp = fopen(Path.c_str(), "rb");
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
    std::string dirPath = Path;
    dirPath.replace(dirPath.end()-3, dirPath.end(), "dir");

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