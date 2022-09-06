#pragma once
#include <string>
#include <vector>
#include <filesystem>

/*
    Contains information about each entry in the archive
    TODO: Add 3, VC & Fastman92's img support
*/
struct EntryInfo
{
    // archive data
    uint32_t Offset;        // in sectors (each sector is 2048 bytes)
    uint16_t Size;          // in sectors (each sector is 2048 bytes)
    uint16_t unused;
    char FileName[24];      // file name in the archive

    // editor data
    std::string Type    = "Unknown";
    std::string Path    = "";       // Path used for importing
    bool bImported      = false;    // Was the item imported    
    bool bRename        = false;    // Is rename in progress
    bool bSelected      = false;    // Is item currently selected
};

/*
    Wrapper class for archives
    Contains helper functions for processing the img
*/
class IMGArchive
{
public:
    std::string Path;
    std::string FileName;
    uint32_t TotalEntries;
    std::vector<EntryInfo> EntryList;
    std::vector<std::string> LogList;

    bool bOpen = true;
    bool bCreateNew;

    IMGArchive(std::string Path, bool CreateNew = false);

    // Adds a new message to log
    void AddLogMessage(std::string &&message);

    // Export entity
    void ExportEntry(EntryInfo *pEntry, std::string filePath);

    // Import entity
    void ImportEntry();

    // Returns true if archive is supported(v2)
    static bool IsSupported(const std::string &Path);
};