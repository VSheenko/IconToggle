#include "ConfigWorker.h"

bool ConfigWorker::Serialization() {

    return false;
}

bool ConfigWorker::Deserialization() {
    return false;
}

void ConfigWorker::ReadKeys(unsigned short *keys) {

}

void ConfigWorker::ReadSettingProgramPath(char* settingProgramPath) {

}

void ConfigWorker::WriteKeys(const unsigned short& keys) {

}

void ConfigWorker::WriteSettingProgramPath(const std::string &settingProgramPath) {

}

ConfigWorker* ConfigWorker::Create(const std::string &configPath) {
    if (!FileExist(configPath))
        return nullptr;

    return new ConfigWorker(configPath);
}

ConfigWorker::ConfigWorker(const std::string &configPath) : configPath(configPath) {}

bool ConfigWorker::FileExist(const std::string &path) {
    DWORD fileAttributes = GetFileAttributesA(path.c_str());
    return (fileAttributes != INVALID_FILE_ATTRIBUTES &&
            !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY));
}

bool ConfigWorker::OpenFile(HANDLE *file, const std::string &path, const DWORD& access) {
    *file = CreateFileA(
            path.c_str(),
            access,
            0,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr);
    return *file != INVALID_HANDLE_VALUE;
}
