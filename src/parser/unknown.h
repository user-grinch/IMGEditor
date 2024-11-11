#pragma once
#include "iparser.h"

/*
    Not used unknown format
*/
class UnknownFMT : public IParser
{
protected:
    UnknownFMT(){};
    UnknownFMT(UnknownFMT&){};

public:
    static UnknownFMT* Get()
    {
        static UnknownFMT parser;
        return &parser;
    }

    void Open(IMGArchive *pMgr){}
    void Export(IMGArchive *pMgr, EntryInfo *pEntry, const std::wstring& filePath, bool logMsg = true){}
    void Import(IMGArchive *pArc, const std::wstring &path, bool replace){}
    bool IsValid(const std::wstring& path){ return false; }
    std::string GetVersionText() { return "Unknown"; }
    void Save(ArchiveInfo *pInfo){}
};