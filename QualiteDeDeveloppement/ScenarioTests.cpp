#include <cassert>
#include <iostream>
#include <string>
#include "Library.h"
#include "Document.h"
#include "FileSystem.h"
#include "MockFileSystem.h"

void runScenarioTests() {
    MockFileSystem mockFs;
    Library library(&mockFs, "library");

    // Scénario: Plusieurs documents avec interactions
    Document doc1(&mockFs);
    doc1.setTitle("Book1");
    doc1.setAuthor("Author1");
    doc1.setContent("Content1");

    Document doc2(&mockFs);
    doc2.setTitle("Book2");
    doc2.setAuthor("Author2");
    doc2.setContent("Content2");

    assert(library.addDocument(doc1));
    assert(library.addDocument(doc2));

    // Scénario de conflits
    Document doc3(&mockFs);
    doc3.setTitle("Book1");  // Même titre que doc1
    doc3.setAuthor("Author3");
    doc3.setContent("Content3");

    // Ça devrait échouer car le titre existe déjà
    assert(!library.addDocument(doc3));

    // Vérifier que le document d'origine est intact
    Document* retrievedDoc = library.getDocument("Book1");
    assert(retrievedDoc != nullptr);
    assert(retrievedDoc->getAuthor() == "Author1");

    std::cout << "Scenario tests passed!" << std::endl;
}
