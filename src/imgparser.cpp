#include "pch.h"
#include "imgparser.h"
#include "imgmgr.h"

void ParserV2::Open(IMGMgr *pMgr)
{
    if (std::filesystem::exists(pMgr->Path))
    {
        pMgr->FileName = std::move(std::filesystem::path(pMgr->Path).filename().stem().string());
        FILE *fp = fopen(pMgr->Path.c_str(), "rb");
        if (fp)
        {
            char ver[4];
            fread(ver, sizeof(ver), 1, fp);
            if (ver[0] == 'V' && ver[1] == 'E' && ver[2] == 'R' && ver[3] == '2')
            {
                uint32_t TotalEntries;
                fread(&TotalEntries, sizeof(TotalEntries), 1, fp);

                for (unsigned int i = 0; i < TotalEntries; ++i)
                {
                    EntryInfo entry;
                    fread(&entry.Offset, sizeof(entry.Offset), 1, fp);
                    fread(&entry.Size, sizeof(entry.Size), 1, fp);
                    fread(&entry.unused, sizeof(entry.unused), 1, fp);
                    fread(entry.FileName, sizeof(entry.FileName), 1, fp);

                    entry.Type = IMGMgr::GetFileType(entry.FileName);
                    pMgr->EntryList.push_back(std::move(entry));
                }
            }
        }
        fclose(fp);
    }
    pMgr->AddLogMessage("Opened archive");
}

void ParserV2::Export(IMGMgr *pMgr, EntryInfo *pEntry, const std::string& filePath, bool logMsg)
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

void ParserV2::Import(IMGMgr *pArc, const std::string &path, bool replace)
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
    info.Type = IMGMgr::GetFileType(info.FileName);
    info.Size = static_cast<uint16_t>(std::filesystem::file_size(path))/2048; // bytes -> sector
    pArc->EntryList.push_back(std::move(info));
    pArc->AddLogMessage(std::format("Imported {}", name));
}

void ParserV2::Save(ArchiveInfo *pInfo)
{
    if (pInfo->pArc->Path == "")
    {
        return;
    }

    pInfo->pArc->ProgressBar.bInUse = true;
    std::string tempPath = pInfo->path + ".temp";
    FILE *fOut = fopen(tempPath.c_str(), "wb");
    FILE *fImg = fopen(pInfo->pArc->Path.c_str(), "rb");

    if (fOut)
    {
        // header
        fwrite("VER2", 4, 1, fOut);
        uint32_t TotalEntries = static_cast<uint32_t>(pInfo->pArc->EntryList.size());
        fwrite(&TotalEntries, sizeof(TotalEntries), 1, fOut);

        // directory entry
        uint32_t offset = 4096; // start offset
        size_t index = 0;
        size_t total = pInfo->pArc->EntryList.size();
        for (EntryInfo &e : pInfo->pArc->EntryList)
        {
            size_t size = 0;
            FILE *fFile = NULL;
            if (e.bImported)
            {
                // read data from file
                fFile = fopen(e.Path.c_str(), "rb");
                fseek(fFile, 0, SEEK_END);
                size = ftell(fFile);
                fseek(fFile, 0, SEEK_SET);
            }
            else
            {
                // read data from img
                fseek(fImg, e.Offset*2048, 0);
                size = e.Size*2048;
            }

            char *buf = new char[size + 1];
            if (buf)
            {
                fread(buf, size, 1, e.bImported? fFile : fImg);
                e.Offset = offset/2048;

                long pos = 8 + static_cast<long>(index) * 32;
                fseek(fOut, pos, 0);
                fwrite(&e.Offset, sizeof(e.Offset), 1, fOut);
                fwrite(&e.Size, sizeof(e.Size), 1, fOut);
                fwrite(&e.unused, sizeof(e.unused), 1, fOut);
                fwrite(e.FileName, sizeof(e.FileName), 1, fOut);

                fseek(fOut, offset, 0);
                fwrite(buf, size, 1, fOut);
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
                if (fImg)
                {
                    fclose(fImg);
                }
                if (fOut)
                {
                    fclose(fOut);
                    remove(tempPath.c_str());
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
    if (fImg)
    {
        fclose(fImg);
    }
    if (fOut)
    {
        fclose(fOut);
        remove(pInfo->pArc->Path.c_str());
        rename(tempPath.c_str(), pInfo->path.c_str());
    }

    // update data
    pInfo->pArc->Path = pInfo->path;
    pInfo->pArc->FileName = std::move(std::filesystem::path(pInfo->path).filename().stem().string());
    pInfo->pArc->ProgressBar.bInUse = false;
    delete pInfo;
}