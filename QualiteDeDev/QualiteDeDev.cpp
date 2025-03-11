#include <iostream>
#include <cassert>
#include <vector>
#include <string>

// Premier composant: Gestionnaire de base de données simplifié
class DatabaseManager {
private:
    std::vector<std::string> records;

public:
    bool addRecord(const std::string& record) {
        records.push_back(record);
        return true;
    }

    bool deleteRecord(size_t index) {
        if (index >= records.size()) {
            return false;
        }
        records.erase(records.begin() + index);
        return true;
    }

    std::string getRecord(size_t index) const {
        if (index >= records.size()) {
            return "";
        }
        return records[index];
    }

    size_t getRecordCount() const {
        return records.size();
    }
};

// Second composant: Service utilisateur qui utilise le gestionnaire de BDD
class UserService {
private:
    DatabaseManager& dbManager;

public:
    UserService(DatabaseManager& db) : dbManager(db) {}

    bool createUser(const std::string& userData) {
        return dbManager.addRecord(userData);
    }

    bool removeUser(size_t userId) {
        return dbManager.deleteRecord(userId);
    }

    std::string getUserInfo(size_t userId) const {
        return dbManager.getRecord(userId);
    }

    size_t getTotalUsers() const {
        return dbManager.getRecordCount();
    }
};

// Classe de test d'intégration
class IntegrationTest {
public:
    void runTests() {
        testUserCreationAndRetrieval();
        testUserDeletion();
        testMultipleOperations();

        std::cout << "Tous les tests d'intégration ont réussi!" << std::endl;
    }

private:
    void testUserCreationAndRetrieval() {
        std::cout << "Test: Création et récupération d'utilisateur..." << std::endl;

        DatabaseManager db;
        UserService userService(db);

        // Test d'intégration: vérifier que l'utilisateur est correctement créé et récupéré
        assert(userService.createUser("Alice,30,alice@example.com"));
        assert(userService.getTotalUsers() == 1);
        assert(userService.getUserInfo(0) == "Alice,30,alice@example.com");

        std::cout << "Test réussi!" << std::endl;
    }

    void testUserDeletion() {
        std::cout << "Test: Suppression d'utilisateur..." << std::endl;

        DatabaseManager db;
        UserService userService(db);

        // Ajout de deux utilisateurs
        assert(userService.createUser("Bob,25,bob@example.com"));
        assert(userService.createUser("Charlie,40,charlie@example.com"));
        assert(userService.getTotalUsers() == 2);

        // Test d'intégration: vérifier que la suppression fonctionne correctement
        assert(userService.removeUser(0));
        assert(userService.getTotalUsers() == 1);
        assert(userService.getUserInfo(0) == "Charlie,40,charlie@example.com");

        std::cout << "Test réussi!" << std::endl;
    }

    void testMultipleOperations() {
        std::cout << "Test: Multiples opérations..." << std::endl;

        DatabaseManager db;
        UserService userService(db);

        // Test d'intégration: vérifier une séquence d'opérations
        assert(userService.createUser("David,35,david@example.com"));
        assert(userService.createUser("Eve,28,eve@example.com"));
        assert(userService.createUser("Frank,45,frank@example.com"));

        assert(userService.getTotalUsers() == 3);
        assert(userService.removeUser(1));
        assert(userService.getTotalUsers() == 2);
        assert(userService.getUserInfo(1) == "Frank,45,frank@example.com");

        assert(userService.createUser("Grace,33,grace@example.com"));
        assert(userService.getTotalUsers() == 3);

        std::cout << "Test réussi!" << std::endl;
    }
};

// Fonction principale
int main() {
    try {
        IntegrationTest integrationTest;
        integrationTest.runTests();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception pendant les tests: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}