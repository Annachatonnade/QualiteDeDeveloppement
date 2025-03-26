#include <iostream>
#include <locale>
#include "FileSystem.h"
#include "Document.h"
#include "Library.h"
#include "MockFileSystem.h"
#include "IntegrationTests.h"
#ifdef _WIN32
#include <windows.h>

#endif


// Déclaration des fonctions définies dans d'autres fichiers
void testFileSystem();
void testDocument();
void runIntegrationTests();
void runScenarioTests();

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#else
    std::locale::global(std::locale("en_US.UTF-8"));
    std::cout.imbue(std::locale());
#endif

    // Configuration de l'encodage pour afficher correctement les caractères spéciaux
    std::locale::global(std::locale(""));
    std::cout.imbue(std::locale());

    std::cout << "=== Démonstration de l'importance des tests d'intégration en C++ ===" << std::endl;

    std::cout << "\n[1] Exécution des tests unitaires..." << std::endl;
    testFileSystem();
    testDocument();

    std::cout << "\n[2] Exécution des tests d'intégration..." << std::endl;
    runIntegrationTests();

    std::cout << "\n[3] Exécution des tests de scénarios..." << std::endl;
    runScenarioTests();

    std::cout << "\n=== Démonstration d'un cas problématique qui n'est pas détecté par les tests unitaires ===" << std::endl;

    // Créons un problème d'intégration: changer le format de sauvegarde dans FileSystem
    // sans adapter Document
    std::cout << "Imaginons que nous avons changé le format de sauvegarde sans adapter tous les modules:" << std::endl;

    MockFileSystem mockFs;
    Document doc(&mockFs);
    doc.setTitle("Problematic Document");
    doc.setAuthor("Integration Error");
    doc.setContent("This shows integration issues");

    // Sauvegarde avec un format différent (simulé)
    mockFs.saveFile("library/Problematic Document.doc",
        "FORMAT_V2\n"
        "title:Problematic Document\n"
        "author:Integration Error\n"
        "content:This shows integration issues");

    // Le chargement échouera car le format a changé
    std::cout << "Tentative de chargement avec un format incompatible: ";
    bool loadResult = doc.load("library/Problematic Document.doc");

    if (!loadResult) {
        std::cout << "ÉCHEC! Les tests d'intégration auraient détecté ce problème" << std::endl;
        std::cout << "C'est ce type d'erreur entre modules qui n'est pas identifié par les tests unitaires seuls!" << std::endl;
    }

    return 0;
}
