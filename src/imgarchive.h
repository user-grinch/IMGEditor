#pragma once
#include <string>
#include <vector>
#include <functional>

enum class eImgVer
{
    One,        // GTA III, VC, BULLY
    Two,        // GTA SA
    Fastman92,  // GTA SA with FLA
    Unknown,
};

// Contains information about each entry in the archive
struct EntryInfo
{
    // archive data
    uint32_t Offset     = 0;        // in sectors (each sector is 2048 bytes)
    uint32_t Sector       = 0;        // in sectors (each sector is 2048 bytes)
    wchar_t FileName[24];              // file name in the archive

    // editor data
    std::wstring Type    = L"Unknown";
    std::wstring Path    = L"";       // Path used for importing
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

class IMGArchive;
class IParser;

// Used to pass info between threads
struct ArchiveInfo
{
    IMGArchive* pArc;
    std::wstring path;
    eImgVer outVer = eImgVer::Unknown;  // version for the output archive
    bool removeExisting = true;
};

/*
    Wrapper class for archives
    Contains helper functions for processing the img
*/
class IMGArchive
{
private:
    eImgVer ImageVersion = eImgVer::One;

public:
    std::wstring Path;
    std::wstring FileName;
    std::vector<EntryInfo> EntryList;
    std::vector<EntryInfo*> SelectedList;
    std::vector<std::wstring> LogList;
    ProgressInfo ProgressBar;
    IParser *Parser = nullptr;
    
    bool bOpen = true;
    bool bCreateNew;
    bool bUpdateSearch = false;

    IMGArchive(std::wstring Path, bool CreateNew = false);

    // Adds a new message to log
    void AddLogMessage(std::wstring &&message);

    // Export entity
    void ExportEntry(EntryInfo *pEntry, std::wstring filePath, bool log = true);
    
    // Exports the entire archive, should run in a separate thread
    static void ExportAll(ArchiveInfo *pInfo);

    // Exports the selected entries in the archive
    static void ExportSelected(ArchiveInfo *pInfo);

    // Get file type
    static std::wstring GetFileType(const wchar_t* name);

    // Returns the format text
    std::string GetFormatText();

    // Returns archive version 
    static eImgVer GetVersion(const std::wstring &archivePath);

    // Sets the version of the archive
    eImgVer GetVersion();

    // Sets the version of the archive
    void SetVersion(eImgVer ver);

    // Import entity
    void ImportEntry(const std::wstring& path, bool replace = false);

    // Imports multiple file entries at once
    static void ImportEntries(ArchiveInfo *pInfo);

    // Rebuilds the IMG archive, save changes
    static void Save(ArchiveInfo *pInfo);

    // Updated the archive search bar selected list
    void UpdateSelectList(const wchar_t* text);
};