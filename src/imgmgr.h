#pragma once
#include <string>
#include <vector>
#include <filesystem>

// Add support for 3 & VC later
struct EntryInfo
{
    uint32_t offset;
    uint16_t size;
    uint16_t unused;
    char name[24];

    // vars
    bool selected = false;
    bool rename = false;
    std::string fileType = "Unknown";
};

class IMGArchive
{
public:
    std::string Path;
    std::string FileName;
    std::vector<EntryInfo> Entries;
    uint32_t TotalEntries;
    bool Open = true;
    bool CreateNew;

    IMGArchive(std::string Path, bool CreateNew = false);

    void ExportEntry(EntryInfo *pEntry, std::string filePath);
    void ImportEntry();
};