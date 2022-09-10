#include "pch.h"
#include "imgmgr.h"
#include "imgparser.h"
#include <filesystem>

IMGMgr::IMGMgr(std::string Path, bool CreateNew)
{
    if (CreateNew)
    {
        this->FileName = Path;
        this->bCreateNew = true;
        AddLogMessage("Created archive");
    }
    else
    {
        this->Path = Path;
        IParser *parser = GetParser(eImgVer::One);
        if (parser)
        {
            parser->Open(this);
        }
    }
} 

std::string IMGMgr::GetFileType(const char* name)
{
    std::string type = "Unknown";
    if (strstr(name, ".dff")) type = "Model";
    if (strstr(name, ".txd")) type = "Texture";
    if (strstr(name, ".col")) type = "Collision";
    if (strstr(name, ".ifp")) type = "Animation";
    if (strstr(name, ".ipl")) type = "Item placement";
    if (strstr(name, ".ide")) type = "Item defination";

    return type;
}

void IMGMgr::ExportEntry(EntryInfo *pEntry, std::string filePath, bool log)
{
    IParser *parser = GetParser(eImgVer::One);
    if (parser)
    {
        parser->Export(this, pEntry, filePath, log);
    }
}

void IMGMgr::ExportAll(ArchiveInfo *pInfo)
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

void IMGMgr::ExportSelected(ArchiveInfo *pInfo)
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

void IMGMgr::ImportEntry(const std::string &path, bool replace)
{
    IParser *parser = GetParser(eImgVer::One);
    if (parser)
    {
        parser->Import(this, path, replace);
    }
}

void IMGMgr::ImportEntries(const std::string &path, bool replace)
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

void IMGMgr::AddLogMessage(std::string &&message)
{
    LogList.push_back(std::move(message));
}

bool IMGMgr::IsSupported(const std::string &Path)
{
    FILE *fp = fopen(Path.c_str(), "rb");
    if (fp)
    {
        char ver[4];
        fread(ver, sizeof(ver), 1, fp);
        return (ver[0] == 'V' && ver[1] == 'E' && ver[2] == 'R' && ver[3] == '2');
        fclose(fp);
    }
    return false;
}

void IMGMgr::Rebuild(ArchiveInfo *pInfo)
{
    IParser *parser = GetParser(eImgVer::One);
    if (parser)
    {
        parser->Save(pInfo);
    }
}

IParser* IMGMgr::GetParser(eImgVer version)
{
    switch(version)
    {
    case eImgVer::One:
        return ParserV2::Get();
        break;
    }

    return nullptr;
}