#pragma once
#include "iparser.h"

#define SECTOR_SZ 2048
#define STRUCT_SZ 32

/*
    The IMG format v1.0
    Used by GTA III, VC, Bully, Manhunt
*/
class ParserPCv1 : public IParser
{
protected:
    ParserPCv1(){};
    ParserPCv1(ParserPCv1&){};

public:
    static ParserPCv1* Get()
    {
        static ParserPCv1 parser;
        return &parser;
    }

    void Open(IMGArchive *pMgr);
    void Export(IMGArchive *pMgr, EntryInfo *pEntry, const std::wstring& filePath, bool logMsg = true);
    void Import(IMGArchive *pArc, const std::wstring &path, bool replace);
    bool IsValid(const std::wstring& path);
    std::string GetVersionText();

    void Save(ArchiveInfo *pInfo);

    size_t GetFileSz(const std::wstring& path);
};