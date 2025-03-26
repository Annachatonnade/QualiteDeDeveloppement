// FileSystem.h - Module de gestion des fichiers
#pragma once
#include <string>
#include <fstream>

class FileSystem {
public:
    virtual bool saveFile(const std::string& path, const std::string& content) {
        std::ofstream file(path);
        if (!file.is_open()) return false;

        file << content << std::endl;
        return true;
    }

    virtual std::string loadFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + path);
        }

        std::string content;
        std::string line;
        while (std::getline(file, line)) {
            content += line + "\n";
        }

        return content;
    }

    virtual bool fileExists(const std::string& path) {
        std::ifstream file(path);
        return file.good();
    }

    virtual ~FileSystem() {}
};
