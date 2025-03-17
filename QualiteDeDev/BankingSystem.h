#ifndef BANKING_SYSTEM_H
#define BANKING_SYSTEM_H

#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <stdexcept>
#include <cassert>
#include <functional>

// Classe représentant un compte bancaire
class Account {
private:
    std::string accountId;
    double balance;

public:
    Account(const std::string& id, double initialBalance = 0.0)
        : accountId(id), balance(initialBalance) {
    }

    std::string getId() const { return accountId; }

    double getBalance() const { return balance; }

    void deposit(double amount) {
        if (amount <= 0) {
            throw std::invalid_argument("Le montant du dépôt doit être positif");
        }
        balance += amount;
    }

    bool withdraw(double amount) {
        if (amount <= 0) {
            throw std::invalid_argument("Le montant du retrait doit être positif");
        }

        if (balance >= amount) {
            balance -= amount;
            return true;
        }

        return false;
    }
};

// Classe pour la gestion des transactions
class TransactionManager {
public:
    enum TransactionType { DEPOSIT, WITHDRAWAL, TRANSFER };

    struct Transaction {
        std::string transactionId;
        std::string sourceAccountId;
        std::string targetAccountId; // Pour les transferts
        TransactionType type;
        double amount;
        bool successful;

        Transaction(const std::string& id, const std::string& source,
            TransactionType t, double a, bool success,
            const std::string& target = "")
            : transactionId(id), sourceAccountId(source), targetAccountId(target),
            type(t), amount(a), successful(success) {
        }
    };

private:
    std::vector<Transaction> transactions;
    int nextTransactionId = 1;

public:
    std::string generateTransactionId() {
        return "TXN" + std::to_string(nextTransactionId++);
    }

    void recordTransaction(const std::string& sourceAccountId,
        TransactionType type, double amount,
        bool successful,
        const std::string& targetAccountId = "") {

        std::string txnId = generateTransactionId();
        transactions.push_back(Transaction(txnId, sourceAccountId, type, amount,
            successful, targetAccountId));
    }

    std::vector<Transaction> getTransactionHistory() const {
        return transactions;
    }

    std::vector<Transaction> getAccountTransactions(const std::string& accountId) const {
        std::vector<Transaction> result;
        for (const auto& txn : transactions) {
            if (txn.sourceAccountId == accountId || txn.targetAccountId == accountId) {
                result.push_back(txn);
            }
        }
        return result;
    }
};

// Classe principale du système bancaire
class BankingSystem {
private:
    std::unordered_map<std::string, std::shared_ptr<Account>> accounts;
    TransactionManager transactionManager;
    int nextAccountId = 1;

public:
    std::string createAccount(double initialBalance = 0.0) {
        std::string accountId = "ACC" + std::to_string(nextAccountId++);
        accounts[accountId] = std::make_shared<Account>(accountId, initialBalance);
        return accountId;
    }

    std::shared_ptr<Account> getAccount(const std::string& accountId) {
        auto it = accounts.find(accountId);
        if (it == accounts.end()) {
            throw std::invalid_argument("Compte non trouvé: " + accountId);
        }
        return it->second;
    }

    bool deposit(const std::string& accountId, double amount) {
        try {
            auto account = getAccount(accountId);
            account->deposit(amount);
            transactionManager.recordTransaction(
                accountId, TransactionManager::DEPOSIT, amount, true
            );
            return true;
        }
        catch (const std::exception& e) {
            transactionManager.recordTransaction(
                accountId, TransactionManager::DEPOSIT, amount, false
            );
            std::cerr << "Erreur lors du dépôt: " << e.what() << std::endl;
            return false;
        }
    }

    bool withdraw(const std::string& accountId, double amount) {
        try {
            auto account = getAccount(accountId);
            bool success = account->withdraw(amount);
            transactionManager.recordTransaction(
                accountId, TransactionManager::WITHDRAWAL, amount, success
            );
            return success;
        }
        catch (const std::exception& e) {
            transactionManager.recordTransaction(
                accountId, TransactionManager::WITHDRAWAL, amount, false
            );
            std::cerr << "Erreur lors du retrait: " << e.what() << std::endl;
            return false;
        }
    }

    bool transfer(const std::string& sourceAccountId,
        const std::string& targetAccountId,
        double amount) {
        try {
            auto sourceAccount = getAccount(sourceAccountId);
            auto targetAccount = getAccount(targetAccountId);

            bool withdrawalSuccess = sourceAccount->withdraw(amount);
            if (!withdrawalSuccess) {
                transactionManager.recordTransaction(
                    sourceAccountId, TransactionManager::TRANSFER, amount, false, targetAccountId
                );
                return false;
            }

            targetAccount->deposit(amount);
            transactionManager.recordTransaction(
                sourceAccountId, TransactionManager::TRANSFER, amount, true, targetAccountId
            );
            return true;
        }
        catch (const std::exception& e) {
            transactionManager.recordTransaction(
                sourceAccountId, TransactionManager::TRANSFER, amount, false, targetAccountId
            );
            std::cerr << "Erreur lors du transfert: " << e.what() << std::endl;
            return false;
        }
    }

    std::vector<TransactionManager::Transaction> getTransactionHistory() const {
        return transactionManager.getTransactionHistory();
    }

    std::vector<TransactionManager::Transaction> getAccountTransactions(
        const std::string& accountId) const {

        return transactionManager.getAccountTransactions(accountId);
    }
};

#endif // BANKING_SYSTEM_H
