#include "pch.h"
#include "imgmgr.h"

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
                    fread(&TotalEntries, sizeof(TotalEntries), 1, fp);

                    for (unsigned int i = 0; i < TotalEntries; ++i)
                    {
                        EntryInfo entry;
                        fread(&entry.Offset, sizeof(entry.Offset), 1, fp);
                        fread(&entry.Size, sizeof(entry.Size), 1, fp);
                        fread(&entry.unused, sizeof(entry.unused), 1, fp);
                        fread(entry.FileName, sizeof(entry.FileName), 1, fp);

                        // extract file type
                        if (strstr(entry.FileName, ".dff")) entry.Type = "Model";
                        if (strstr(entry.FileName, ".txd")) entry.Type = "Texture";
                        if (strstr(entry.FileName, ".col")) entry.Type = "Collision";
                        if (strstr(entry.FileName, ".ifp")) entry.Type = "Animation";
                        if (strstr(entry.FileName, ".ipl")) entry.Type = "Item placement";
                        if (strstr(entry.FileName, ".ide")) entry.Type = "Item defination";

                        EntryList.push_back(std::move(entry));
                    }
                }
            }
            fclose(fp);
        }
        AddLogMessage("Opened archive");
    }
} 

void IMGArchive::ExportEntry(EntryInfo *pEntry, std::string filePath)
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
            AddLogMessage(std::format("Exported {}", pEntry->FileName));
            buf[pEntry->Size] ='\0';
            
            delete[] buf;
        }
        fclose(pImg);
        fclose(pOut);
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