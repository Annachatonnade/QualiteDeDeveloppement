#ifdef _WIN32
#include <Windows.h>
#endif 

#include "BankingSystem.h"
#include "TestFramework.h"
#include <iostream>
#include <cassert>


// Tests unitaires pour la classe Account
void runUnitTests(TestFramework& framework) {
    TestFramework::TestSuite& unitTestSuite = framework.createTestSuite("Tests Unitaires");

    // Test de création de compte
    unitTestSuite.addTest("Account - Création avec solde initial", []() {
        Account account("ACC123", 100.0);
        return account.getBalance() == 100.0 && account.getId() == "ACC123";
        });

    // Test de dépôt
    unitTestSuite.addTest("Account - Dépôt de montant valide", []() {
        Account account("ACC123");
        account.deposit(50.0);
        return account.getBalance() == 50.0;
        });

    // Test de dépôt avec montant négatif
    unitTestSuite.addTest("Account - Dépôt de montant négatif", []() {
        Account account("ACC123");
        try {
            account.deposit(-50.0);
            return false; // Devrait lever une exception
        }
        catch (const std::invalid_argument&) {
            return account.getBalance() == 0.0;
        }
        });

    // Test de retrait avec solde suffisant
    unitTestSuite.addTest("Account - Retrait avec solde suffisant", []() {
        Account account("ACC123", 100.0);
        bool result = account.withdraw(50.0);
        return result && account.getBalance() == 50.0;
        });

    // Test de retrait avec solde insuffisant
    unitTestSuite.addTest("Account - Retrait avec solde insuffisant", []() {
        Account account("ACC123", 30.0);
        bool result = account.withdraw(50.0);
        return !result && account.getBalance() == 30.0;
        });

    // Test de retrait négatif
    unitTestSuite.addTest("Account - Retrait de montant négatif", []() {
        Account account("ACC123", 100.0);
        try {
            account.withdraw(-50.0);
            return false; // Devrait lever une exception
        }
        catch (const std::invalid_argument&) {
            return account.getBalance() == 100.0;
        }
        });

    // Tests unitaires pour TransactionManager
    unitTestSuite.addTest("TransactionManager - Générer ID de transaction", []() {
        TransactionManager manager;
        std::string id1 = manager.generateTransactionId();
        std::string id2 = manager.generateTransactionId();
        return id1 != id2;
        });

    unitTestSuite.addTest("TransactionManager - Enregistrement des transactions", []() {
        TransactionManager manager;
        manager.recordTransaction("ACC1", TransactionManager::DEPOSIT, 100.0, true);
        auto transactions = manager.getTransactionHistory();
        return transactions.size() == 1 &&
            transactions[0].sourceAccountId == "ACC1" &&
            transactions[0].type == TransactionManager::DEPOSIT &&
            transactions[0].amount == 100.0 &&
            transactions[0].successful == true;
        });
}

// Tests d'intégration pour tester l'interaction entre les composants
void runIntegrationTests(TestFramework& framework) {
    TestFramework::TestSuite& integrationTestSuite = framework.createTestSuite("Tests d'Intégration");

    // Test d'intégration du système complet - Création de compte et dépôt
    integrationTestSuite.addTest("Intégration - Création de compte et dépôt", []() {
        BankingSystem system;
        std::string accountId = system.createAccount();
        bool depositResult = system.deposit(accountId, 200.0);
        auto account = system.getAccount(accountId);

        // Vérification que le dépôt a fonctionné
        if (!depositResult || account->getBalance() != 200.0) {
            return false;
        }

        // Vérification que la transaction a été correctement enregistrée
        auto transactions = system.getAccountTransactions(accountId);
        return transactions.size() == 1 &&
            transactions[0].sourceAccountId == accountId &&
            transactions[0].type == TransactionManager::DEPOSIT &&
            transactions[0].amount == 200.0 &&
            transactions[0].successful == true;
        });

    // Test d'intégration - Dépôt et retrait
    integrationTestSuite.addTest("Intégration - Dépôt et retrait", []() {
        BankingSystem system;
        std::string accountId = system.createAccount();

        system.deposit(accountId, 500.0);
        bool withdrawResult = system.withdraw(accountId, 300.0);

        auto account = system.getAccount(accountId);

        // Vérification du solde après opérations
        if (!withdrawResult || account->getBalance() != 200.0) {
            return false;
        }

        // Vérification des transactions
        auto transactions = system.getAccountTransactions(accountId);
        return transactions.size() == 2 &&
            transactions[1].type == TransactionManager::WITHDRAWAL &&
            transactions[1].amount == 300.0 &&
            transactions[1].successful == true;
        });

    // Test d'intégration - Transfert entre comptes
    integrationTestSuite.addTest("Intégration - Transfert entre comptes", []() {
        BankingSystem system;

        // Création de deux comptes
        std::string sourceId = system.createAccount(1000.0);
        std::string targetId = system.createAccount();

        // Transfert de fonds
        bool transferResult = system.transfer(sourceId, targetId, 500.0);

        // Vérification des soldes après transfert
        auto sourceAccount = system.getAccount(sourceId);
        auto targetAccount = system.getAccount(targetId);

        if (!transferResult ||
            sourceAccount->getBalance() != 500.0 ||
            targetAccount->getBalance() != 500.0) {
            return false;
        }

        // Vérification des transactions
        auto transactions = system.getTransactionHistory();
        return transactions.size() == 1 &&
            transactions[0].type == TransactionManager::TRANSFER &&
            transactions[0].sourceAccountId == sourceId &&
            transactions[0].targetAccountId == targetId &&
            transactions[0].amount == 500.0 &&
            transactions[0].successful == true;
        });

    // Test d'intégration - Transfert avec solde insuffisant
    integrationTestSuite.addTest("Intégration - Transfert avec solde insuffisant", []() {
        BankingSystem system;

        // Création de deux comptes
        std::string sourceId = system.createAccount(100.0);
        std::string targetId = system.createAccount();

        // Tentative de transfert avec solde insuffisant
        bool transferResult = system.transfer(sourceId, targetId, 500.0);

        // Vérification que le transfert a échoué et que les soldes n'ont pas changé
        auto sourceAccount = system.getAccount(sourceId);
        auto targetAccount = system.getAccount(targetId);

        if (transferResult ||
            sourceAccount->getBalance() != 100.0 ||
            targetAccount->getBalance() != 0.0) {
            return false;
        }

        // Vérification de l'enregistrement de l'échec
        auto transactions = system.getTransactionHistory();
        return transactions.size() == 1 &&
            transactions[0].type == TransactionManager::TRANSFER &&
            transactions[0].successful == false;
        });

    // Test d'intégration - Historique des transactions
    integrationTestSuite.addTest("Intégration - Historique des transactions", []() {
        BankingSystem system;
        std::string accountId = system.createAccount(1000.0);

        // Plusieurs opérations
        system.deposit(accountId, 500.0);
        system.withdraw(accountId, 200.0);
        system.withdraw(accountId, 2000.0); // Échec intentionnel

        // Vérification de l'historique
        auto transactions = system.getAccountTransactions(accountId);

        // Doit avoir 3 transactions
        return transactions.size() == 3 &&
            // Dépôt réussi
            transactions[0].type == TransactionManager::DEPOSIT &&
            transactions[0].successful == true &&
            // Retrait réussi
            transactions[1].type == TransactionManager::WITHDRAWAL &&
            transactions[1].successful == true &&
            // Retrait échoué
            transactions[2].type == TransactionManager::WITHDRAWAL &&
            transactions[2].successful == false;
        });
}

// Fonction principale de démonstration
void demonstrateTestBenefits() {
    std::cout << "\n=====================================================" << std::endl;
    std::cout << "DÉMONSTRATION DE L'UTILITÉ DES TESTS D'INTÉGRATION" << std::endl;
    std::cout << "=====================================================" << std::endl;

    std::cout << "\nCette démonstration montre comment les tests unitaires et d'intégration\n"
        << "travaillent ensemble pour assurer la qualité du logiciel.\n" << std::endl;

    std::cout << "Les tests unitaires vérifient le fonctionnement de composants individuels," << std::endl;
    std::cout << "tandis que les tests d'intégration valident leur interaction correcte." << std::endl;

    std::cout << "\n-----------------------------------------------------" << std::endl;

    // Créer et exécuter les tests
    TestFramework framework;
    runUnitTests(framework);
    runIntegrationTests(framework);
    framework.runTests();

    // Démontrer un scénario d'utilisation
    std::cout << "\n=====================================================" << std::endl;
    std::cout << "SCÉNARIO D'UTILISATION RÉELLE" << std::endl;
    std::cout << "=====================================================" << std::endl;

    BankingSystem bank;

    std::cout << "\n1. Création de comptes" << std::endl;
    std::string aliceAccount = bank.createAccount(1000);
    std::string bobAccount = bank.createAccount();

    std::cout << "   - Compte d'Alice créé avec ID: " << aliceAccount << " (Solde initial: 1000€)" << std::endl;
    std::cout << "   - Compte de Bob créé avec ID: " << bobAccount << " (Solde initial: 0€)" << std::endl;

    std::cout << "\n2. Bob fait un dépôt" << std::endl;
    bank.deposit(bobAccount, 500);
    std::cout << "   - Dépôt de 500€ effectué sur le compte de Bob" << std::endl;
    std::cout << "   - Nouveau solde de Bob: " << bank.getAccount(bobAccount)->getBalance() << "€" << std::endl;

    std::cout << "\n3. Alice transfère de l'argent à Bob" << std::endl;
    bank.transfer(aliceAccount, bobAccount, 300);
    std::cout << "   - Alice transfère 300€ à Bob" << std::endl;
    std::cout << "   - Nouveau solde d'Alice: " << bank.getAccount(aliceAccount)->getBalance() << "€" << std::endl;
    std::cout << "   - Nouveau solde de Bob: " << bank.getAccount(bobAccount)->getBalance() << "€" << std::endl;

    std::cout << "\n4. Bob tente un retrait supérieur à son solde" << std::endl;
    bool withdrawResult = bank.withdraw(bobAccount, 1000);
    std::cout << "   - Tentative de retrait de 1000€ par Bob: " << (withdrawResult ? "Réussite" : "Échec") << std::endl;
    std::cout << "   - Solde de Bob après tentative: " << bank.getAccount(bobAccount)->getBalance() << "€" << std::endl;

    std::cout << "\n5. Affichage de l'historique des transactions" << std::endl;
    std::cout << "   Historique des transactions de Bob:" << std::endl;
    auto bobTxns = bank.getAccountTransactions(bobAccount);
    for (const auto& txn : bobTxns) {
        std::cout << "   - Type: ";
        switch (txn.type) {
        case TransactionManager::DEPOSIT: std::cout << "Dépôt"; break;
        case TransactionManager::WITHDRAWAL: std::cout << "Retrait"; break;
        case TransactionManager::TRANSFER: std::cout << "Transfert"; break;
        }
        std::cout << ", Montant: " << txn.amount << "€";
        std::cout << ", Statut: " << (txn.successful ? "Réussi" : "Échoué");

        if (txn.type == TransactionManager::TRANSFER && !txn.sourceAccountId.empty()) {
            if (txn.sourceAccountId == bobAccount) {
                std::cout << ", Vers: " << txn.targetAccountId;
            }
            else {
                std::cout << ", De: " << txn.sourceAccountId;
            }
        }

        std::cout << std::endl;
    }

    std::cout << "\n=====================================================" << std::endl;
    std::cout << "CONCLUSION" << std::endl;
    std::cout << "=====================================================" << std::endl;

    std::cout << "\nPourquoi les tests d'intégration sont-ils indispensables?" << std::endl;
    std::cout << "----------------------------------------------------------------\n";
    std::cout << "1. Ils détectent les problèmes d'interaction entre composants\n";
    std::cout << "   - Par exemple, dans notre système, la cohérence entre les comptes\n";
    std::cout << "     et l'enregistrement des transactions a été validée\n\n";

    std::cout << "2. Ils valident des flux complets dans l'application\n";
    std::cout << "   - Le processus de transfert qui implique deux comptes et\n";
    std::cout << "     l'enregistrement d'une transaction ne peut être testé unitairement\n\n";

    std::cout << "3. Ils reproduisent des scénarios réels d'utilisation\n";
    std::cout << "   - Les tests d'intégration simulent l'utilisation du système\n";
    std::cout << "     comme le ferait un utilisateur final\n\n";

    std::cout << "4. Ils servent de documentation vivante\n";
    std::cout << "   - Les tests d'intégration illustrent comment les différents\n";
    std::cout << "     composants doivent interagir ensemble\n";
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif // _WIN32
    demonstrateTestBenefits();
    return 0;
}
