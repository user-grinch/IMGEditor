#include "pch.h"
#include "imgmgr.h"

IMGArchive::IMGArchive(std::string Path, bool CreateNew)
{
    if (CreateNew)
    {
        this->FileName = Path;
        this->CreateNew = true;
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
                        fread(&entry.offset, sizeof(entry.offset), 1, fp);
                        fread(&entry.size, sizeof(entry.size), 1, fp);
                        fread(&entry.unused, sizeof(entry.unused), 1, fp);
                        fread(entry.name, sizeof(entry.name), 1, fp);

                        // extract file type
                        if (strstr(entry.name, ".dff")) entry.fileType = "Model";
                        if (strstr(entry.name, ".txd")) entry.fileType = "Texture";
                        if (strstr(entry.name, ".col")) entry.fileType = "Collision";
                        if (strstr(entry.name, ".ifp")) entry.fileType = "Animation";
                        if (strstr(entry.name, ".ipl")) entry.fileType = "Item placement";
                        if (strstr(entry.name, ".ide")) entry.fileType = "Item defination";

                        Entries.push_back(std::move(entry));
                    }
                }
            }
            fclose(fp);
        }
    }
} 

void IMGArchive::ExportEntry(EntryInfo *pEntry, std::string filePath)
{
    FILE *pOut = fopen(filePath.c_str(), "wb+");
    FILE *pImg = fopen(Path.c_str(), "rb");

    if (pOut && pImg)
    {
        fseek(pImg, pEntry->offset*2048, 0);
        size_t size = pEntry->size*2048;
        char *buf = new char[size + 1];
        if (buf)
        {
            fread(buf, size, 1, pImg);
            fwrite(buf, size, 1, pOut);
            buf[pEntry->size] ='\0';
            
            delete[] buf;
        }
        fclose(pImg);
        fclose(pOut);
    }
}