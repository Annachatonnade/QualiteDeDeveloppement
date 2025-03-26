#include <cassert>
#include <iostream>
#include "FileSystem.h"
#include "Document.h"

void testFileSystem() {
    FileSystem fs;
    std::string testContent = "Test content";
    std::string testPath = "test.txt";

    // Test écriture
    bool saveResult = fs.saveFile(testPath, testContent);
    assert(saveResult);

    // Test lecture
    std::string loadedContent = fs.loadFile(testPath);
    assert(loadedContent == testContent + "\n");

    std::cout << "FileSystem tests passed!" << std::endl;
}

void testDocument() {
    FileSystem fs;
    Document doc(&fs);

    doc.setTitle("Test Document");
    doc.setAuthor("Test Author");
    doc.setContent("This is test content");

    assert(doc.getTitle() == "Test Document");
    assert(doc.getAuthor() == "Test Author");
    assert(doc.getContent() == "This is test content");

    std::cout << "Document tests passed!" << std::endl;
}
