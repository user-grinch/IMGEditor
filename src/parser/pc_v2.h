#pragma once
#include "pc_v1.h"

/*
    The IMG format v2.0
    Used by GTA SA
*/
class ParserPCv2 : public ParserPCv1
{
protected:
    ParserPCv2(){};
    ParserPCv2(ParserPCv2&){};

public:
    static ParserPCv2* Get()
    {
        static ParserPCv2 parser;
        return &parser;
    }

    std::string GetVersionText();
    void Open(IMGArchive *pMgr);
    bool IsValid(const std::wstring& path);
    void Save(ArchiveInfo *pInfo);
};