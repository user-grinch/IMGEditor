#pragma once
#include "imgarchive.h"

class IParser
{
public:
    // Opens the archive & feeds data
    virtual void Open(IMGArchive *pMgr) = 0;

    // Exports an entity form the archive
    virtual void Export(IMGArchive *pMgr, EntryInfo *pEntry, const std::wstring& filePath, bool logMsg = true) = 0;

    // Imports an entity to the archive
    virtual void Import(IMGArchive *pArc, const std::wstring &path, bool replace) = 0;

    // Rebuilds the archive & saves changes
    virtual void Save(ArchiveInfo *pInfo) = 0;
};

/*
    The original IMG format
    Used by GTA III, VC, Bully, Manhunt, SA etc.
*/
class Parser : public IParser
{
private:
    Parser(){};
    Parser(Parser&){};

public:
    static Parser* Get()
    {
        static Parser parser;
        return &parser;
    }

    void Open(IMGArchive *pMgr);
    void Export(IMGArchive *pMgr, EntryInfo *pEntry, const std::wstring& filePath, bool logMsg = true);
    void Import(IMGArchive *pArc, const std::wstring &path, bool replace);
    void Save(ArchiveInfo *pInfo);
};