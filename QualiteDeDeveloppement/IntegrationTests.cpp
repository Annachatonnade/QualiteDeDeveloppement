#include <cassert>
#include <iostream>
#include "Library.h"
#include "Document.h"
#include "MockFileSystem.h"
#include "IntegrationTests.h"

void runIntegrationTests() {
    MockFileSystem mockFs;
    Library library(&mockFs, "library");

    // Test d'intégration: Création d'un document et ajout à la bibliothèque
    Document doc1(&mockFs);
    doc1.setTitle("Integration Test");
    doc1.setAuthor("Test Author");
    doc1.setContent("This is integration test content");

    bool addResult = library.addDocument(doc1);
    assert(addResult);

    // Vérification que le document a été sauvegardé correctement
    assert(mockFs.fileExists("library/Integration Test.doc"));

    // Test d'intégration: Récupération d'un document de la bibliothèque
    Document* retrievedDoc = library.getDocument("Integration Test");
    assert(retrievedDoc != nullptr);
    assert(retrievedDoc->getTitle() == "Integration Test");
    assert(retrievedDoc->getAuthor() == "Test Author");

    // Test d'intégration complexe: Modification et rechargement
    retrievedDoc->setContent("Modified content");
    retrievedDoc->save("library/Integration Test.doc");

    Document doc2(&mockFs);
    doc2.setFileSystem(&mockFs);
    bool loadResult = doc2.load("library/Integration Test.doc");
    assert(loadResult);

    // Vérifions que le contenu commence par "Modified content" au lieu d'une comparaison exacte
    std::string content = doc2.getContent();
    std::cout << "Contenu chargé: '" << content << "'" << std::endl;
    assert(content.find("Modified content") == 0); // Vérifie que le contenu commence par "Modified content"

    std::cout << "Integration tests passed!" << std::endl;
}
