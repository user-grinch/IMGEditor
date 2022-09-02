#include "imgmgr.h"
#include <filesystem>

IMGArchive::IMGArchive(std::string Path, bool CreateNew)
{
    this->Path = Path;

    if (std::filesystem::exists(Path))
    {
        FILE *fp = fopen(Path.c_str(), "rb");
        char ver[4];
        fread(ver, sizeof(ver), 1, fp);
        if (ver[0] == 'V' && ver[1] == 'E' && ver[2] == 'R' && ver[3] == '2')
        {
            fread(&TotalEntries, sizeof(TotalEntries), 1, fp);

            for (unsigned int i = 0; i < TotalEntries; ++i)
            {
                DirEntry entry;
                fread(&entry.offset, sizeof(entry.offset), 1, fp);
                fread(&entry.size, sizeof(entry.size), 1, fp);
                fread(&entry.unused, sizeof(entry.unused), 1, fp);
                fread(entry.name, sizeof(entry.name), 1, fp);

                Entries.push_back(std::move(entry));
            }
        }
    }
} 