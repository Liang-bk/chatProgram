//
// Created by baikaishui on 2025/6/13.
//

#include "header/config.h"


Config::Config() {
    boost::filesystem::path current_path = boost::filesystem::current_path();
    boost::filesystem::path config_path = current_path / "config.json";
    std::cout << "config path is: " << config_path.string() << std::endl;
    std::ifstream config_file(config_path.string());
    if (!config_file.is_open()) {
        std::cerr << "Error: Couldn't open config file: " << config_path << std::endl;
        return;
    }
    Json::CharReaderBuilder reader_builder;
    std::string errs;
    if (!Json::parseFromStream(reader_builder, config_file, &root, &errs)) {
        std::cerr << "Error: Couldn't load config file: " << errs << std::endl;
    } else {
        std::cout << "Configuration loaded successfully from " << config_path.string() << std::endl;
    }
}

Config::~Config() {

}

const Json::Value &Config::getValue(const std::string &key) {
    if (!root.find(key)) {
        return Json::Value::null;
    }
    return root[key];
}

const Json::Value &Config::operator[](const std::string &key) {
    return getValue(key);
}

