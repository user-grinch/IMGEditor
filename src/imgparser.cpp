#include "pch.h"
#include "imgparser.h"
#include "imgarchive.h"

void Parser::Open(IMGArchive *pArc)
{
    std::string dirPath = pArc->Path;
    if (pArc->ImageVersion == eImgVer::One)
    {
        dirPath.replace(dirPath.end()-3, dirPath.end(), "dir");
    }

    if (std::filesystem::exists(dirPath))
    {
        pArc->FileName = std::move(std::filesystem::path(dirPath).filename().stem().string());
        FILE *fp = fopen(dirPath.c_str(), "rb");
        if (fp)
        {
            uint32_t TotalEntries = 0;

            // fetch size
            if (pArc->ImageVersion == eImgVer::One)
            {
                fseek(fp, 0, SEEK_END);
                TotalEntries = ftell(fp) / 32; // size of dir 
                fseek(fp, 0, SEEK_SET);
            }
            else
            {
                // We already know it's v2 archive
                fseek(fp, 4, 0);
                fread(&TotalEntries, sizeof(TotalEntries), 1, fp);
            }

            for (unsigned int i = 0; i < TotalEntries; ++i)
            {
                EntryInfo entry;
                fread(&entry.Offset, sizeof(entry.Offset), 1, fp);
                fread(&entry.Size, sizeof(entry.Size), 1, fp);
                fread(entry.FileName, sizeof(entry.FileName), 1, fp);

                entry.Type = IMGArchive::GetFileType(entry.FileName);
                pArc->EntryList.push_back(std::move(entry));
            }
        }
        fclose(fp);
    }
    pArc->AddLogMessage("Opened archive");
}

void Parser::Export(IMGArchive *pMgr, EntryInfo *pEntry, const std::string& filePath, bool logMsg)
{
    FILE *pOut = fopen(filePath.c_str(), "wb+");
    FILE *pImg = fopen(pMgr->Path.c_str(), "rb");

    if (pOut && pImg)
    {
        fseek(pImg, pEntry->Offset*2048, 0);
        size_t size = pEntry->Size*2048;
        char *buf = new char[size + 1];
        if (buf)
        {
            fread(buf, size, 1, pImg);
            fwrite(buf, size, 1, pOut);
            if (logMsg)
            {
                pMgr->AddLogMessage(std::format("Exported {}", pEntry->FileName));
            }
            buf[pEntry->Size] ='\0';
            
            delete[] buf;
        }
    }
    if (pImg)
    {
        fclose(pImg);
    }
    if (pOut)
    {
        fclose(pOut);
    }
}

void Parser::Import(IMGArchive *pArc, const std::string &path, bool replace)
{
    std::filesystem::path p {path};

    // skip folder paths
    if (p.extension() == "")
    {
        return;
    }

    std::string name = p.filename().string();

    if (name.size() > 23)
    {
        pArc->AddLogMessage(std::format("Skipping {}. Name too large.", name));
        return;
    }

    if (replace)
    {
        pArc->EntryList.erase (
            std::remove_if (
                pArc->EntryList.begin(), 
                pArc->EntryList.end(), 
                [&](EntryInfo const& obj) {
                    return obj.FileName == name;
                }
            ), 
            pArc->EntryList.end()
        );
    }

    EntryInfo info;
    strcpy(info.FileName, name.c_str());
    info.FileName[23] = '\0';
    info.Path = path;
    info.bImported = true;
    info.Type = IMGArchive::GetFileType(info.FileName);
    info.Size = static_cast<uint32_t>(std::filesystem::file_size(path))/2048; // bytes -> sector
    pArc->EntryList.push_back(std::move(info));
    pArc->AddLogMessage(std::format("Imported {}", name));
}

void Parser::Save(ArchiveInfo *pInfo)
{
    if (pInfo->pArc->Path == "")
    {
        return;
    }

    eImgVer ver = pInfo->pArc->ImageVersion;
    pInfo->pArc->ProgressBar.bInUse = true;
    std::string tempPath = pInfo->path + ".temp";
    FILE *fImg = fopen(tempPath.c_str(), "wb");
    FILE *fIn = fopen(pInfo->pArc->Path.c_str(), "rb");

    // used for v1 archives
    FILE *fDir = nullptr;
    std::string dirPath = "";
    std::string tempDirPath = "";

    if (ver == eImgVer::One)
    {
        dirPath = pInfo->path;
        dirPath.replace(dirPath.end()-3, dirPath.end(), "dir");
        tempDirPath = dirPath + ".temp";
        fDir = fopen(tempDirPath.c_str(), "wb");
    }   

    if (fImg && (fDir || ver == eImgVer::Two))
    {
        // v2 header
        if (ver == eImgVer::Two)
        {
            fwrite("VER2", 4, 1, fImg);
            uint32_t TotalEntries = static_cast<uint32_t>(pInfo->pArc->EntryList.size());
            fwrite(&TotalEntries, sizeof(TotalEntries), 1, fImg);
        }

        // dir entry
        uint32_t offset = ver == eImgVer::One ? 0 : 4096; // start offset
        size_t index = 0;
        size_t total = pInfo->pArc->EntryList.size();
        for (EntryInfo &e : pInfo->pArc->EntryList)
        {
            size_t size = 0;
            FILE *fFile = NULL;

            // fetch archive size
            if (e.bImported)
            {
                fFile = fopen(e.Path.c_str(), "rb");
                fseek(fFile, 0, SEEK_END);
                size = ftell(fFile);
                fseek(fFile, 0, SEEK_SET);
            }
            else
            {
                fseek(fIn, e.Offset*2048, 0);
                size = e.Size*2048;
            }

            char *buf = new char[size + 1];
            if (buf)
            {
                fread(buf, size, 1, e.bImported? fFile : fIn);
                e.Offset = offset/2048;

                long pos = 8 + static_cast<long>(index) * 32;

                if (ver == eImgVer::One)
                {
                    fwrite(&e.Offset, sizeof(e.Offset), 1, fDir);
                    fwrite(&e.Size, sizeof(e.Size), 1, fDir);
                    fwrite(e.FileName, sizeof(e.FileName), 1, fDir);
                }
                else
                {
                    fseek(fImg, pos, 0);
                    fwrite(&e.Offset, sizeof(e.Offset), 1, fImg);
                    fwrite(&e.Size, sizeof(e.Size), 1, fImg);
                    fwrite(e.FileName, sizeof(e.FileName), 1, fImg);
                    fseek(fImg, offset, 0);
                }

                fwrite(buf, size, 1, fImg);
                
                delete[] buf;
            }

            if (fFile)
            {
                fclose(fFile);
            }
            
            pInfo->pArc->ProgressBar.Percentage = (static_cast<float>(index)+1)/ static_cast<float>(total);

            if (pInfo->pArc->ProgressBar.bCancel)
            {
                pInfo->pArc->ProgressBar.bCancel = false;
                if (fIn)
                {
                    fclose(fIn);
                }
                if (fDir)
                {
                    fclose(fDir);
                    std::filesystem::remove(tempDirPath);
                }
                if (fImg)
                {
                    fclose(fImg);
                    std::filesystem::remove(tempPath);
                }
                pInfo->pArc->AddLogMessage("Rebuilding failed");
                pInfo->pArc->ProgressBar.bInUse = false;
                delete pInfo;
                return;
            }
            
            offset += e.Size*2048; // add the sector for this entry
            ++index;
        }        
    }

    if (fIn)
    {
        fclose(fIn);
    }

    if (fDir)
    {
        fclose(fDir);
        std::filesystem::remove(dirPath);
        std::filesystem::rename(tempDirPath, dirPath);
    }

    if (fImg)
    {
        fclose(fImg);
        std::filesystem::remove(pInfo->pArc->Path);
        std::filesystem::rename(tempPath, pInfo->path);
    }

    // update data
    pInfo->pArc->Path = pInfo->path;
    pInfo->pArc->FileName = std::move(std::filesystem::path(pInfo->path).filename().stem().string());
    pInfo->pArc->ProgressBar.bInUse = false;
    delete pInfo;
}