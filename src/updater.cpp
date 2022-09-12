#include "pch.h"
#include "updater.h"

bool Updater::IsUpdateAvailable()
{
    return Updater::curState == States::FOUND;
}

void Updater::ResetUpdaterState()
{
    Updater::curState = States::IDLE;
}

std::string Updater::GetUpdateVersion()
{
    return Updater::latestVer;
}

void Updater::CheckUpdate()
{
    if (Updater::curState == States::IDLE)
    {
        Updater::curState = States::CHECKING;
    }
}

void Updater::Process()
{
    if (Updater::curState != States::CHECKING)
    {
        return;
    }

    const char* link = "https://api.github.com/repos/user-grinch/Cheat-Menu/tags";
    char buf[MAX_PATH];
    GetModuleFileName(NULL, buf, MAX_PATH);
    std::string path = std::filesystem::path(buf).remove_filename().string() + "ver.json";
    HRESULT res = URLDownloadToFile(NULL, link, path.c_str(), 0, NULL);

    if (res == E_OUTOFMEMORY || res == INET_E_DOWNLOAD_FAILURE)
    {
        return;
    }

    // Extract the version number
    FILE *pFile= fopen(path.c_str(), "r");
    if (pFile != NULL)
    {
        char buf[64];
        float version = 0.0f;
        while (fgets(buf, 64, pFile) != NULL)
        {
            sscanf(buf, "[{\"name\": \"%f\",", &version);
            if (version != 0.0f)
            {
                std::stringstream ss;
                ss << std::fixed << std::setprecision(2) << version;
                latestVer = ss.str();
                break;
            }
        }
        fclose(pFile);
    }
    remove(path.c_str());

    if (latestVer > EDITOR_VERSION)
    {
        curState = States::FOUND;
    }
    else
    {
        Updater::curState = States::IDLE;
    }
}
