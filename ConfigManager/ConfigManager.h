#ifndef ICONTOGGLE_CONFIGMANAGER_H
#define ICONTOGGLE_CONFIGMANAGER_H

#include <windows.h>
#include <string>
#include <fstream>
#include <memory>
#include <unordered_map>

class ConfigManager {
private:
    static std::unordered_map<std::string, std::shared_ptr<ConfigManager>> instances;
    std::string configPath;

    static VOID InitDefaultConfig(const std::string &configPath);

public:
    unsigned char keys[3];
    unsigned short autoHideTime;
    bool isLBM;
    bool isShortcut;

public:
    static bool FileExist(const std::string& path);
    explicit ConfigManager(const std::string& configPath);
    static std::shared_ptr<ConfigManager> Instance(const std::string& configPath);

    bool Serialization();
    bool Deserialization();
};




#endif //ICONTOGGLE_CONFIGMANAGER_H
