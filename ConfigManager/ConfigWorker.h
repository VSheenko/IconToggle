#ifndef ICONTOGGLE_CONFIGWORKER_H
#define ICONTOGGLE_CONFIGWORKER_H

#include <windows.h>
#include <string>

class ConfigWorker {
private:
    void ReadKeys(unsigned short* keys);
    void ReadSettingProgramPath(std::string& settingProgramPath);

    void WriteKeys(const unsigned short* keys);
    void WriteSettingProgramPath(const std::string& settingProgramPath);

    static bool FileExist(const std::string& path);
    bool OpenFile(HANDLE* file, const std::string& path, const  DWORD& access);

    std::string configPath;


public:
    unsigned short keys[3];
    unsigned short autoHideTime;
    unsigned int   PID;
    char           settingProgramPath[];

public:
    explicit ConfigWorker(const std::string& configPath);
    static ConfigWorker* Create(const std::string& configPath);

    bool Serialization();
    bool Deserialization();
};




#endif //ICONTOGGLE_CONFIGWORKER_H
