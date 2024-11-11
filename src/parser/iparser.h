
#pragma once
#include "../imgarchive.h"

class IParser
{
public:
    // Opens the archive & feeds data
    virtual void Open(IMGArchive *pMgr) = 0;

    // Exports an entity form the archive
    virtual void Export(IMGArchive *pMgr, EntryInfo *pEntry, const std::wstring& filePath, bool logMsg = true) = 0;

    // Imports an entity to the archive
    virtual void Import(IMGArchive *pArc, const std::wstring &path, bool replace) = 0;
    
    // Returns true if a valid archive format
    virtual bool IsValid() = 0;

    // Rebuilds the archive & saves changes
    virtual void Save(ArchiveInfo *pInfo) = 0;
};