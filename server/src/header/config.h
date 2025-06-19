//
// Created by baikaishui on 2025/6/13.
//

#ifndef CONFIG_H
#define CONFIG_H
#pragma once
#include "header/singleton.h"

class Config : public Singleton<Config> {
    friend class Singleton<Config>;
public:
    ~Config();
    const Json::Value& getValue(const std::string& key);
    const Json::Value& operator[] (const std::string& key);
private:
    Config();
    Json::Value root;
};



#endif //CONFIG_H
