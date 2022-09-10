#pragma once
#include <string>
#include <vector>
#include <functional>

enum class eImgVer
{
    One,        // GTA III, VC, BULLY
    Two,        // GTA SA
    Fastman92,  // GTA SA with FLA
};

/*
    Contains information about each entry in the archive
    TODO: Add 3, VC & Fastman92's img support
*/
struct EntryInfo
{
    // archive data
    uint32_t Offset     = 0;        // in sectors (each sector is 2048 bytes)
    uint16_t Size       = 0;        // in sectors (each sector is 2048 bytes)
    uint16_t unused     = 0;
    char FileName[24];              // file name in the archive

    // editor data
    std::string Type    = "Unknown";
    std::string Path    = "";       // Path used for importing
    bool bImported      = false;    // Was the item imported    
    bool bRename        = false;    // Is rename in progress
    bool bSelected      = false;    // Is item currently selected
};

struct ProgressInfo
{
    float Percentage = 0.0f;
    bool bCancel = false;
    bool bInUse = false;
};

class IMGMgr;
extern class IParser;

struct ArchiveInfo
{
    IMGMgr* pArc;
    std::string path;
};

/*
    Wrapper class for archives
    Contains helper functions for processing the img
*/
class IMGMgr
{
public:
    std::string Path;
    std::string FileName;
    std::vector<EntryInfo> EntryList;
    std::vector<std::string> LogList;
    ProgressInfo ProgressBar;

    bool bOpen = true;
    bool bCreateNew;

    IMGMgr(std::string Path, bool CreateNew = false);

    // Adds a new message to log
    void AddLogMessage(std::string &&message);

    // Export entity
    void ExportEntry(EntryInfo *pEntry, std::string filePath, bool log = true);
    
    // Exports the entire archive, should run in a separate thread
    static void ExportAll(ArchiveInfo *pInfo);

    // Exports the selected entries in the archive
    static void ExportSelected(ArchiveInfo *pInfo);

    // Get file type
    static std::string GetFileType(const char* name);

    // Returns the appropriate parser for IMG Type
    static IParser* GetParser(eImgVer version);

    // Import entity
    void ImportEntry(const std::string& path, bool replace = false);

    // Imports multiple file entries at once
    void ImportEntries(const std::string& filePaths, bool replace = false);

    // Returns true if archive is supported(v2)
    static bool IsSupported(const std::string &Path);

    // Rebuilds the IMG archive, save changes
    static void Rebuild(ArchiveInfo *pInfo);
};