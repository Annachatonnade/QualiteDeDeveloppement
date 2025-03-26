// MockFileSystem.h - Système de fichiers simulé pour les tests
#pragma once
#include <string>
#include <map>
#include "FileSystem.h"

class MockFileSystem : public FileSystem {
private:
    std::map<std::string, std::string> mockFiles;

public:
    bool saveFile(const std::string& path, const std::string& content) override {
        mockFiles[path] = content;
        return true;
    }

    std::string loadFile(const std::string& path) override {
        if (mockFiles.find(path) == mockFiles.end()) {
            throw std::runtime_error("Mock file not found: " + path);
        }
        return mockFiles[path] + "\n";
    }

    bool fileExists(const std::string& path) override {
        return mockFiles.find(path) != mockFiles.end();
    }
};
