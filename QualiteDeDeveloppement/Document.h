// Document.h - Module de gestion des documents
#pragma once
#include <string>
#include "FileSystem.h"

class Document {
private:
    std::string title;
    std::string content;
    std::string author;
    FileSystem* fileSystem;

public:
    // Ajout d'un constructeur par défaut
    Document() : fileSystem(nullptr) {}

    // Constructeur existant
    Document(FileSystem* fs) : fileSystem(fs) {}

    void setTitle(const std::string& title) { this->title = title; }
    void setContent(const std::string& content) { this->content = content; }
    void setAuthor(const std::string& author) { this->author = author; }

    std::string getTitle() const { return title; }
    std::string getContent() const { return content; }
    std::string getAuthor() const { return author; }

    // Ajoutez une fonction pour définir le système de fichiers si nécessaire
    void setFileSystem(FileSystem* fs) { fileSystem = fs; }

    bool save(const std::string& path) {
        if (!fileSystem) return false;  // Vérification pour éviter les erreurs

        std::string metadata = "TITLE:" + title + "\n";
        metadata += "AUTHOR:" + author + "\n";
        metadata += "CONTENT:\n" + content;

        return fileSystem->saveFile(path, metadata);
    }

    bool load(const std::string& path) {
        if (!fileSystem) return false;  // Vérification pour éviter les erreurs

        try {
            std::string fileContent = fileSystem->loadFile(path);

            // Exemple d'analyse simple du contenu
            size_t titlePos = fileContent.find("TITLE:");
            size_t authorPos = fileContent.find("AUTHOR:");
            size_t contentPos = fileContent.find("CONTENT:");

            if (titlePos == std::string::npos || authorPos == std::string::npos || contentPos == std::string::npos) {
                return false;
            }

            title = fileContent.substr(titlePos + 6, authorPos - titlePos - 7);
            author = fileContent.substr(authorPos + 7, contentPos - authorPos - 8);

            // Extraction du contenu et suppression des retours à la ligne finaux
            content = fileContent.substr(contentPos + 9);

            // Suppression des éventuels retours à la ligne en fin de contenu
            while (!content.empty() && (content.back() == '\n' || content.back() == '\r')) {
                content.pop_back();
            }

            return true;
        }
        catch (const std::exception&) {
            return false;
        }
    }

};
