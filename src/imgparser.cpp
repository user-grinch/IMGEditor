#include "pch.h"
#include "imgparser.h"
#include "imgarchive.h"

void Parser::Open(IMGArchive *pArc)
{
    std::wstring dirPath = pArc->Path;
    if (pArc->ImageVersion == eImgVer::One)
    {
        dirPath.replace(dirPath.end()-3, dirPath.end(), L"dir");
    }

    if (std::filesystem::exists(dirPath))
    {
        pArc->FileName = std::move(std::filesystem::path(dirPath).filename().stem().wstring());
        FILE *fp = _wfopen(dirPath.c_str(), L"rb");
        if (fp)
        {
            uint32_t TotalEntries = 0;

            // fetch size
            if (pArc->ImageVersion == eImgVer::One)
            {
                _fseeki64(fp, 0, SEEK_END);
                TotalEntries = static_cast<uint32_t>(_ftelli64(fp) / 32); // size of dir 
                _fseeki64(fp, 0, SEEK_SET);
            }
            else
            {
                // We already know it's v2 archive
                _fseeki64(fp, 4, 0);
                fread(&TotalEntries, sizeof(TotalEntries), 1, fp);
            }

            for (unsigned int i = 0; i < TotalEntries; ++i)
            {
                EntryInfo entry;
                fread(&entry.Offset, sizeof(entry.Offset), 1, fp);
                fread(&entry.Size, sizeof(entry.Size), 1, fp);

                char buf[24];
                fread(buf, sizeof(buf), 1, fp);
                assert(sizeof(entry.FileName) != sizeof(buf));
                ConvertCharToWideChar(buf, entry.FileName, 24);

                entry.Type = IMGArchive::GetFileType(entry.FileName);
                pArc->EntryList.push_back(std::move(entry));
            }
        }
        fclose(fp);
    }
    pArc->AddLogMessage(L"Opened archive");
}

void Parser::Export(IMGArchive *pMgr, EntryInfo *pEntry, const std::wstring& filePath, bool logMsg)
{
    std::wstring path = filePath;
    size_t i = 1;
    FILE *pF = _wfopen(path.c_str(), L"r");
    while (pF != NULL) {
        ++i;
        std::wstring temp = std::format(L"({})", i);
        size_t pos = path.find_last_of(L"."); 
        path.insert(pos , temp);
        if (pF) {
            fclose(pF);
        }
        pF = _wfopen(path.c_str(), L"r");
    }
    if (pF) {
        fclose(pF);
    }

    FILE *pOut = _wfopen(path.c_str(), L"wb+");
    FILE *pImg = _wfopen(pMgr->Path.c_str(), L"rb");

    if (pOut && pImg)
    {
        size_t offset = pEntry->Offset*2048;
        size_t size = pEntry->Size*2048;

        _fseeki64(pImg, offset, 0);
        char *buf = new char[size + 1];
        if (buf)
        {
            fread(buf, size, 1, pImg);
            fwrite(buf, size, 1, pOut);
            if (logMsg)
            {
                pMgr->AddLogMessage(std::format(L"Exported {}", pEntry->FileName));
            }
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

void Parser::Import(IMGArchive *pArc, const std::wstring &path, bool replace)
{
    std::filesystem::path p {path};

    // skip folder paths
    if (p.extension() == "")
    {
        return;
    }

    std::wstring name = p.filename().wstring();

    if (name.size() > 23)
    {
        pArc->AddLogMessage(std::format(L"Skipping {}. Name too large.", name));
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

    EntryInfo info = {0};
    wcscpy(info.FileName, name.c_str());
    info.Path = path;
    info.bImported = true;
    info.Type = IMGArchive::GetFileType(info.FileName);
    info.Size = static_cast<uint32_t>(std::filesystem::file_size(path))/2048; // bytes -> sector
    pArc->EntryList.push_back(std::move(info));
}

void Parser::Save(ArchiveInfo *pInfo)
{
    eImgVer ver = pInfo->pArc->ImageVersion;
    eImgVer outVer = pInfo->outVer;
    pInfo->pArc->ProgressBar.bInUse = true;
    std::wstring tempPath = pInfo->path + L".temp";
    FILE *fImg = _wfopen(tempPath.c_str(), L"wb");
    FILE *fIn = _wfopen(pInfo->pArc->Path.c_str(), L"rb");

    // used for v1 archives
    FILE *fDir = nullptr;
    std::wstring dirPath = L"";
    std::wstring tempDirPath = L"";

    if (outVer == eImgVer::One)
    {
        dirPath = pInfo->path.c_str();
        dirPath.replace(dirPath.find(L"img"), sizeof(L"img") , L"dir");
        tempDirPath = dirPath + L".temp";
        fDir = _wfopen(tempDirPath.c_str(), L"wb");
    }   

    if (fImg)
    {
        size_t offset = 0;
        size_t index = 0;
        size_t total = pInfo->pArc->EntryList.size();

        // v2 header
        if (outVer == eImgVer::Two)
        {
            fwrite("VER2", 4, 1, fImg);
            fwrite(&total, sizeof(total), 1, fImg);
            offset = 0x300000; // start offset, max 98k items
        }
        
        for (EntryInfo &e : pInfo->pArc->EntryList)
        {
            size_t size = 0;
            FILE *fFile = NULL;

            // fetch archive size
            if (e.bImported)
            {
                fFile = _wfopen(e.Path.c_str(), L"rb");
                _fseeki64(fFile, 0, SEEK_END);
                size = _ftelli64(fFile);
                _fseeki64(fFile, 0, SEEK_SET);
            }
            else
            {
                _fseeki64(fIn, e.Offset*2048, SEEK_SET);
                size = e.Size*2048;
            }

            char *buf = new char[size + 1];
            if (buf)
            {
                fread(buf, size, 1, e.bImported? fFile : fIn);
                e.Offset = static_cast<uint32_t>(offset/2048);

                if (outVer == eImgVer::One)
                {
                    fwrite(&e.Offset, sizeof(e.Offset), 1, fDir);
                    fwrite(&e.Size, sizeof(e.Size), 1, fDir);
                    fwrite(e.FileName, sizeof(e.FileName), 1, fDir);
                }
                else
                {
                    long dirOffset = static_cast<long>(0x8 + 0x20 * index);
                    _fseeki64(fImg, dirOffset, SEEK_SET);
                    fwrite(&e.Offset, sizeof(e.Offset), 1, fImg);
                    fwrite(&e.Size, sizeof(e.Size), 1, fImg);
                    fwrite(e.FileName, sizeof(e.FileName), 1, fImg);
                    _fseeki64(fImg, offset, SEEK_SET);
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
                pInfo->pArc->AddLogMessage(L"Rebuilding failed");
                pInfo->pArc->ProgressBar.bInUse = false;
                delete pInfo;
                return;
            }
            
            offset += size; // add the sector for this entry
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

        if (pInfo->removeExisting)
        {
            std::filesystem::remove(pInfo->pArc->Path);
        }
        std::filesystem::rename(tempPath, pInfo->path);
    }

    // update data
    pInfo->pArc->Path = pInfo->path;
    pInfo->pArc->FileName = std::move(std::filesystem::path(pInfo->path).filename().stem().wstring());
    pInfo->pArc->ProgressBar.bInUse = false;
    if (outVer != eImgVer::Unknown)
    {
        pInfo->pArc->ImageVersion = outVer;
    }
    delete pInfo;
}