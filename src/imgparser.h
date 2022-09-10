#pragma once
#include "imgmgr.h"

class IParser
{
public:
    // Opens the archive & feeds data
    virtual void Open(IMGMgr *pMgr) = 0;

    // Exports an entity form the archive
    virtual void Export(IMGMgr *pMgr, EntryInfo *pEntry, const std::string& filePath, bool logMsg = true) = 0;

    // Imports an entity to the archive
    virtual void Import(IMGMgr *pArc, const std::string &path, bool replace) = 0;

    // Rebuilds the archive & saves changes
    virtual void Save(ArchiveInfo *pInfo) = 0;
};

/*
    Parser for IMG version 2 format
    Used for GTA SA
*/
class ParserV2 : public IParser
{
public:
    static ParserV2* Get()
    {
        static ParserV2 parser;
        return &parser;
    }

    void Open(IMGMgr *pMgr);
    void Export(IMGMgr *pMgr, EntryInfo *pEntry, const std::string& filePath, bool logMsg = true);
    void Import(IMGMgr *pArc, const std::string &path, bool replace);
    void Save(ArchiveInfo *pInfo);
};