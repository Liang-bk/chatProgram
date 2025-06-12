//
// Created by 86183 on 2025/6/12.
//

#include "header/config_manager.h"
ConfigManager::ConfigManager() {

    boost::filesystem::path current_path = boost::filesystem::current_path();
    boost::filesystem::path config_path = current_path / "config.ini";
    std::cout << "config path is : " << config_path.string() << std::endl;

    boost::property_tree::ptree pt;
    // 将config_path路径的文件读到pt中
    boost::property_tree::read_ini(config_path.string(), pt);

    for (const auto & section_pair : pt) {
        const std::string& section_name = section_pair.first;
        const boost::property_tree::ptree& section_tree = section_pair.second;
        std::map<std::string, std::string> section_config;
        for (const auto& kv_pair : section_tree) {
            const std::string &key = kv_pair.first;
            const std::string &value = kv_pair.second.get_value<std::string>();

            config_map_[section_name][key] = value;
        }
    }
    // 打印配置
    for (const auto & section_entry : config_map_) {
        const std::string &section_name = section_entry.first;
        const SectionInfo& section_info = section_entry.second;
        std::cout << "[" << section_name << "]" << std::endl;
        for (const auto& kv_pair : section_info.section_datas_) {
            std::cout << kv_pair.first << " = " << kv_pair.second << std::endl;
        }
    }
}
ConfigManager::~ConfigManager() {

}


SectionInfo ConfigManager::operator[](const std::string &key) {
    if (config_map_.find(key) == config_map_.end()) {
        return SectionInfo();
    }
    return config_map_[key];
}

ConfigManager::ConfigManager(const ConfigManager &other) {
    config_map_ = other.config_map_;
}

ConfigManager& ConfigManager::operator=(const ConfigManager &other) {
    if (this == &other) {
        return *this;
    }
    config_map_ = other.config_map_;
    return *this;
}

