#include "pch.h"
#include "imgmgr.h"
#include <filesystem>

IMGArchive::IMGArchive(std::string Path, bool CreateNew)
{
    if (CreateNew)
    {
        this->FileName = Path;
        this->bCreateNew = true;
        AddLogMessage("Created archive");
    }
    else
    {
        if (std::filesystem::exists(Path))
        {
            this->Path = Path;
            this->FileName = std::move(std::filesystem::path(Path).filename().stem().string());
            FILE *fp = fopen(Path.c_str(), "rb");
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

                        entry.Type = GetFileType(entry.FileName);
                        EntryList.push_back(std::move(entry));
                    }
                }
            }
            fclose(fp);
        }
        AddLogMessage("Opened archive");
    }
} 

std::string IMGArchive::GetFileType(const char* name)
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

void IMGArchive::ExportEntry(EntryInfo *pEntry, std::string filePath, bool log)
{
    FILE *pOut = fopen(filePath.c_str(), "wb+");
    FILE *pImg = fopen(Path.c_str(), "rb");

    if (pOut && pImg)
    {
        fseek(pImg, pEntry->Offset*2048, 0);
        size_t size = pEntry->Size*2048;
        char *buf = new char[size + 1];
        if (buf)
        {
            fread(buf, size, 1, pImg);
            fwrite(buf, size, 1, pOut);
            if (log)
            {
                AddLogMessage(std::format("Exported {}", pEntry->FileName));
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

void IMGArchive::ImportEntry(const std::string &path)
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
        AddLogMessage(std::format("Skipping {}. Name too large.", name));
        return;
    }

    EntryInfo info;
    strcpy(info.FileName, name.c_str());
    info.FileName[23] = '\0';
    info.Path = path;
    info.bImported = true;
    info.Type = GetFileType(info.FileName);
    info.Size = static_cast<uint16_t>(std::filesystem::file_size(path))/2048; // bytes -> sector
    EntryList.push_back(std::move(info));
    AddLogMessage(std::format("Imported {}", name));
}

void IMGArchive::ImportEntries(const std::string &path)
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
            ImportEntry(temp);
            temp = "";
        }
    }
}

void IMGArchive::AddLogMessage(std::string &&message)
{
    LogList.push_back(std::move(message));
}

bool IMGArchive::IsSupported(const std::string &Path)
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

void IMGArchive::Rebuild()
{
    FILE *fOut = fopen("C:\\Users\\User\\Desktop\\test.img", "wb");
    FILE *fImg = fopen(Path.c_str(), "rb");

    if (fOut && fImg)
    {
        // header
        fwrite("VER2", 4, 1, fOut);
        uint32_t TotalEntries = static_cast<uint32_t>(EntryList.size());
        fwrite(&TotalEntries, sizeof(TotalEntries), 1, fOut);

        // directory entry
        uint32_t offset = 4096; // start offset
        size_t index = 0;
        for (EntryInfo &e : EntryList)
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

                fseek(fOut, 8+index*32, 0);
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
    }
}