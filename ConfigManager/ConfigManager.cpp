#include "ConfigManager.h"

std::unordered_map<std::string, std::shared_ptr<ConfigManager>> ConfigManager::instances;

bool ConfigManager::Serialization() {
    std::ofstream file(configPath, std::ios::binary);
    if (!file.is_open())
        return false;

    file.write(reinterpret_cast<char*>(&keys), sizeof(keys));
    file.write(reinterpret_cast<char*>(&autoHideTime), sizeof(autoHideTime));
    file.write(reinterpret_cast<char*>(&isLBM), sizeof(isLBM));
    file.write(reinterpret_cast<char*>(&isShortcut), sizeof(isShortcut));

    return true;
}

bool ConfigManager::Deserialization() {
    std::ifstream file(configPath, std::ios::binary);
    if (!file.is_open())
        return false;

    file.read(reinterpret_cast<char*>(&keys), sizeof(keys));
    file.read(reinterpret_cast<char*>(&autoHideTime), sizeof(autoHideTime));
    file.read(reinterpret_cast<char*>(&isLBM), sizeof(isLBM));
    file.read(reinterpret_cast<char*>(&isShortcut), sizeof(isShortcut));

    return true;
}


std::shared_ptr<ConfigManager> ConfigManager::Instance(const std::string &configPath) {
    if (!FileExist(configPath))
        throw std::runtime_error("Config file does not exist");

    if (instances.find(configPath) != instances.end())
        return instances[configPath];

    instances[configPath] = std::make_shared<ConfigManager>(configPath);
    return instances[configPath];
}

ConfigManager::ConfigManager(const std::string &configPath) : configPath(configPath) {}

bool ConfigManager::FileExist(const std::string &path) {
    DWORD fileAttributes = GetFileAttributesA(path.c_str());
    return (fileAttributes != INVALID_FILE_ATTRIBUTES &&
            !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY));
}
