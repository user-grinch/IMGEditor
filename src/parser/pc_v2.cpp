#include "pch.h"
#include "pc_v2.h"
#include "../utils.h"

void ParserPCv2::Open(IMGArchive* pArc)
{
    if (std::filesystem::exists(pArc->Path))
    {
        pArc->FileName = std::filesystem::path(pArc->Path).filename().stem().wstring();
        std::ifstream stream(pArc->Path, std::ios::binary);
        if (stream)
        {
            stream.seekg(4, std::ios::beg);
            size_t total = 0;
            stream.read(reinterpret_cast<char*>(&total), 4);
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

void ParserPCv2::Save(ArchiveInfo* pInfo)
{
    eImgVer ver = pInfo->pArc->GetVersion();
    eImgVer outVer = pInfo->outVer;
    pInfo->pArc->ProgressBar.bInUse = true;

    try {
        std::wstring tempPath = pInfo->path + L".temp";
        std::ofstream fImg(tempPath, std::ios::binary);
        std::ifstream fIn(pInfo->pArc->Path, std::ios::binary);

        if (!fImg.is_open()) {
            throw std::runtime_error("Archive open failed");
        }

        size_t offset = 0;
        size_t total = pInfo->pArc->EntryList.size();
        size_t index = 0;
        
        fImg.write("VER2", 4);
        fImg.write(reinterpret_cast<char*>(&total), sizeof(total));
        offset = 0x300000; // start offset, max 98k items

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

                long dirOffset = static_cast<long>(0x8 + 0x20 * index);
                fImg.seekp(dirOffset, std::ios::beg);
                fImg.write(reinterpret_cast<char*>(&e.Offset), sizeof(e.Offset));
                fImg.write(reinterpret_cast<char*>(&e.Sector), sizeof(e.Sector));
                fImg.write(nameBuf.data(), nameBuf.size());
                fImg.seekp(offset, std::ios::beg);
                fImg.write(buf.data(), size);

                offset += size; 
            }

            pInfo->pArc->ProgressBar.Percentage = static_cast<float>(index + 1) / total;
            ++index;

            if (pInfo->pArc->ProgressBar.bCancel) {
                throw std::runtime_error("Rebuild cancelled");
            }
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
            pInfo->pArc->SetVersion(outVer);
        }
    } catch (const std::exception& e) {
        pInfo->pArc->ProgressBar.bInUse = false;
        pInfo->pArc->AddLogMessage(L"Rebuilding failed: " + std::wstring(e.what(), e.what() + strlen(e.what())));
        std::filesystem::remove(pInfo->path + L".temp");
        pInfo->pArc->ProgressBar.bCancel = false;
    }

    delete pInfo;
}

std::string ParserPCv2::GetVersionText() {
    return "PC v2";
}

bool ParserPCv2::IsValid(const std::wstring& path) {
    std::ifstream file(path, std::ios::binary);
    if (file)
    {
        char ver[4];
        file.read(ver, sizeof(ver));
        if (ver[0] == 'V' && ver[1] == 'E' && ver[2] == 'R' && ver[3] == '2')
        {
            file.close();
            return true;
        }
        file.close();
    }
    return false;
}