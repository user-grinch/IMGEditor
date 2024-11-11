#include "pch.h"
#include "pc_v1.h"
#include "../utils.h"

void ParserPCv1::Open(IMGArchive* pArc)[[[[[]]]]]
{
    std::wstring dirPath = std::filesystem::path(pArc->Path).replace_extension(L".dir").wstring();
    if (std::filesystem::exists(dirPath))
    {
        pArc->FileName = std::filesystem::path(pArc->Path).filename().stem().wstring();
        std::ifstream stream(pArc->Path, std::ios::binary);
        if (stream)
        {
            const auto total = GetFileSz(pArc->Path) / STRUCT_SZ;
            pArc->EntryList.reserve(total);

            for (size_t i = 0; i < total; ++i)
            {
                EntryInfo entry;
                stream.read(reinterpret_cast<char*>(&entry.Offset), sizeof(entry.Offset));
                stream.read(reinterpret_cast<char*>(&entry.Sector), sizeof(entry.Sector));

                std::vector<char> buffer(24);
                stream.read(buffer.data(), buffer.size());
                Utils::ConvertUtf8ToWide(buffer.data(), entry.FileName, buffer.size());

                entry.Type = IMGArchive::GetFileType(entry.FileName);
                pArc->EntryList.emplace_back(std::move(entry));
            }

            pArc->AddLogMessage(L"Opened archive");
        }
        else
        {
            pArc->AddLogMessage(L"Open archive failed");
        }
    }
    else
    {
        pArc->AddLogMessage(L"Archive .dir error");
    }
}

void ParserPCv1::Export(IMGArchive* pMgr, EntryInfo* pEntry, const std::wstring& filePath, bool logMsg)
{
    // Find an available file name
    std::filesystem::path path = filePath;
    size_t i = 1;
    while (std::filesystem::exists(path))
    {
        path = filePath;
        path.replace_filename(std::format(L"{}({}){}", path.stem().wstring(), i++, path.extension().wstring()));
    }

    std::ifstream imgStream(pMgr->Path, std::ios::binary);
    std::ofstream outFile(path, std::ios::binary);
    if (outFile && imgStream)
    {
        const size_t size = pEntry->Sector * SECTOR_SZ;
        imgStream.seekg(pEntry->Offset * SECTOR_SZ);

        std::vector<char> buffer(size);
        if (imgStream.read(buffer.data(), size))
        {
            outFile.write(buffer.data(), size);
            if (logMsg)
            {
                pMgr->AddLogMessage(std::format(L"Exported {}", pEntry->FileName));
            }
        }
    }
    else
    {
        pMgr->AddLogMessage(L"Export failed");
    }
}

void ParserPCv1::Import(IMGArchive *pArc, const std::wstring &path, bool replace)
{
    std::filesystem::path p {path};

    // skip folder paths
    if (p.extension() == "")
    {
        return;
    }

    std::wstring name = p.filename().wstring();

    if (name.size() > 23)
    {
        pArc->AddLogMessage(std::format(L"Skipping {}. Name too large.", name));
        return;
    }

    if (replace)
    {
        auto it =  std::remove_if (pArc->EntryList.begin(), pArc->EntryList.end(), 
                [&](EntryInfo const& obj) {
                    return obj.FileName == name;
                });
        pArc->EntryList.erase (it, pArc->EntryList.end());
    }

    EntryInfo info = {0};
    wcscpy(info.FileName, name.c_str());
    info.Path = path;
    info.bImported = true;
    info.Type = IMGArchive::GetFileType(info.FileName);
    info.Sector = GetFileSz(path) / SECTOR_SZ;
    pArc->EntryList.push_back(std::move(info));
}

void ParserPCv1::Save(ArchiveInfo* pInfo)
{
    eImgVer ver = pInfo->pArc->ImageVersion;
    eImgVer outVer = pInfo->outVer;
    pInfo->pArc->ProgressBar.bInUse = true;

    try {
        std::wstring tempPath = pInfo->path + L".temp";
        std::ofstream fImg(tempPath, std::ios::binary);
        std::ifstream fIn(pInfo->pArc->Path, std::ios::binary);

        if (!fImg.is_open() || !fIn.is_open()) {
            throw std::runtime_error("Archive open failed");
        }

        std::filesystem::path dirPath = std::filesystem::path(pInfo->path).replace_extension(L".dir").wstring();
        std::wstring tempDirPath = dirPath.wstring() + L".temp";
        std::ofstream fDir(tempDirPath, std::ios::binary);

        if (!fDir.is_open()) {
            throw std::runtime_error("Archive .dir error");
        }

        size_t offset = 0;
        size_t total = pInfo->pArc->EntryList.size();
        size_t index = 0;

        for (EntryInfo& e : pInfo->pArc->EntryList) {
            size_t size = 0;
            std::ifstream fFile;

            if (e.bImported) {
                fFile.open(e.Path, std::ios::binary);
                if (!fFile.is_open()) {
                    throw std::runtime_error("Open import failed");
                }
                size = GetFileSz(e.Path);
                e.Sector = size / SECTOR_SZ;
            } else {
                fIn.seekg(e.Offset * SECTOR_SZ, std::ios::beg);
                size = e.Sector * SECTOR_SZ;
            }

            std::vector<char> buf(size);
            if (buf.size() > 0) {
                if (e.bImported) {
                    fFile.read(buf.data(), size);
                } else {
                    fIn.read(buf.data(), size);
                }

                e.Offset = static_cast<uint32_t>(offset / SECTOR_SZ);
                std::vector<char> nameBuf(24);
                Utils::ConvertWideToUtf8(e.FileName, nameBuf.data(), nameBuf.size());

                fDir.write(reinterpret_cast<char*>(&e.Offset), sizeof(e.Offset));
                fDir.write(reinterpret_cast<char*>(&e.Sector), sizeof(e.Sector));
                fDir.write(nameBuf.data(), nameBuf.size());
                fImg.write(buf.data(), size);

                offset += size; 
            }

            pInfo->pArc->ProgressBar.Percentage = static_cast<float>(index + 1) / total;
            ++index;

            if (pInfo->pArc->ProgressBar.bCancel) {
                throw std::runtime_error("Rebuild cancelled");
            }
        }

        if (fDir.is_open()) {
            fDir.close();
            std::filesystem::remove(pInfo->path);
            std::filesystem::rename(tempDirPath, pInfo->path);
        }

        if (fImg.is_open()) {
            fImg.close();
            if (pInfo->removeExisting) {
                std::filesystem::remove(pInfo->pArc->Path);
            }
            std::filesystem::rename(tempPath, pInfo->path);
        }

        pInfo->pArc->Path = pInfo->path;
        pInfo->pArc->FileName = std::filesystem::path(pInfo->path).filename().stem().wstring();
        pInfo->pArc->ProgressBar.bInUse = false;
        if (outVer != eImgVer::Unknown) {
            pInfo->pArc->ImageVersion = outVer;
        }
    } catch (const std::exception& e) {
        pInfo->pArc->ProgressBar.bInUse = false;
        pInfo->pArc->AddLogMessage(L"Rebuilding failed: " + std::wstring(e.what(), e.what() + strlen(e.what())));
        std::filesystem::remove(pInfo->path + L".temp");
        pInfo->pArc->ProgressBar.bCancel = false;
    }

    delete pInfo;
}

size_t ParserPCv1::GetFileSz(const std::wstring& path) {
    size_t sz = std::filesystem::file_size(path);
    return max(SECTOR_SZ, sz + (SECTOR_SZ - sz % SECTOR_SZ) % SECTOR_SZ);
}