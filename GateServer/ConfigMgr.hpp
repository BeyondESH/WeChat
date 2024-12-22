//
// Created by Beyond on 2024/11/20.
//

#ifndef CONFIGMGR_H
#define CONFIGMGR_H

#include "const.h"

class ConfigMgr {
public:
    static ConfigMgr &getInstance() {
        static ConfigMgr instance; //局部静态变量生命周期与线程同步
        return instance;
    }

    ConfigMgr(const ConfigMgr &config_mgr) {
        this->_config_map = config_mgr._config_map;
    }

    ConfigMgr &operator=(const ConfigMgr &config_mgr) {
        if (this == &config_mgr) {
            return *this;
        }
        this->_config_map = config_mgr._config_map;
        return *this;
    }

    std::map<std::string,std::string> operator[](const std::string &section) {
        if (!this->_config_map.contains(section)) {
            return {};
        }
        return this->_config_map[section];
    }

    ~ConfigMgr() {
        _config_map.clear();
    }

    void printConfig() const {
        for (auto &section_pair: _config_map) {
            std::cout << "[" << section_pair.first << "]" << std::endl;
            for (auto &pair: section_pair.second) {
                std::cout << pair.first << "=" << pair.second << std::endl;
            }
        }
    }

private:
    ConfigMgr() {
        std::filesystem::path current_path = std::filesystem::current_path();
        std::filesystem::path config_path = current_path.parent_path() / "config.ini";
        std::cout << "config path is:" << config_path << std::endl;
        boost::property_tree::ptree ptree;
        boost::property_tree::read_ini(config_path.string(), ptree);
        for (const auto &section_pair: ptree) {
            const std::string &section_name = section_pair.first;
            const boost::property_tree::ptree &section_tree = section_pair.second;
            for (const auto &pair: section_tree) {
                const std::string &key = pair.first;
                const std::string &value = pair.second.get_value<std::string>();
                _config_map[section_name][key] = value;
            }
        }
        printConfig();
    }

    std::map<std::string, std::map<std::string, std::string> > _config_map;
};


#endif //CONFIGMGR_H
