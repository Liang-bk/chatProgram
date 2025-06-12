//
// Created by 86183 on 2025/6/12.
//

#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H
#pragma once
#include "constant.h"

struct SectionInfo {
    SectionInfo() {}
    ~SectionInfo() {}
    // 拷贝构造
    SectionInfo(const SectionInfo &other) {
        section_datas_ = other.section_datas_;
    }
    // 赋值构造
    SectionInfo &operator=(const SectionInfo &other) {
        if (this == &other) {
            return *this;
        }
        section_datas_ = other.section_datas_;
        return *this;
    }
    std::string operator[] (const std::string& key) {
        if (section_datas_.find(key) == section_datas_.end()) {
            return "";
        }
        return section_datas_[key];
    }
    std::map<std::string, std::string> section_datas_;
};

class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();
    SectionInfo operator[] (const std::string& key);
    ConfigManager(const ConfigManager &other);
    ConfigManager &operator=(const ConfigManager &other);
private:
    std::map<std::string, SectionInfo> config_map_;
};



#endif //CONFIG_MANAGER_H
