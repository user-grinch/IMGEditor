#pragma once
#include <string>
#include <vector>

// Add support for 3 & VC later
struct DirEntry
{
    uint32_t offset;
    uint16_t size;
    uint16_t unused;
    char name[24];

    // vars
    bool selected = false;
    bool rename = false;
};

class IMGArchive
{
public:
    std::string Path;
    std::vector<DirEntry> Entries;
    uint32_t TotalEntries;

    IMGArchive(std::string Path, bool CreateNew = false);
};