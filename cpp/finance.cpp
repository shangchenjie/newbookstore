#include "finance.h"

FinanceManager financeMgr("finance.txt");

void FinanceManager::loadFromFile() {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        return;
    }
    priceChanges.clear();
    totalProfit = 0.00;
    double change;
    while (file >> change) {
        priceChanges.push_back(change);
        totalProfit += change;
    }
    file.close();
}

void FinanceManager::saveToFile() {
    std::ofstream file(fileName);
    if (!file.is_open()) {
        return;
    }

    for (const auto& change : priceChanges) {
        file << std::fixed << std::setprecision(2) << change << "\n";
    }

    file.close();
}

FinanceManager::FinanceManager(const std::string& file) : totalProfit(0.00), fileName(file) {
    loadFromFile();
}

FinanceManager::~FinanceManager() {
    saveToFile();
}

void FinanceManager::recordPriceChange(double change) {
    priceChanges.push_back(change);
    totalProfit += change;
}


double FinanceManager::getTotalProfit() const {
    return totalProfit;
}

void FinanceManager::printTransactions(int count) const {
    if (count == 0) {
        std::cout << "\n";
        return;
    }
    int s=priceChanges.size();
    if (count > s)) {
        std::cout<<"Invalid\n";
        return;

    }
    double income = 0.00;
    double outcome= 0.00;
    int startIndex = (count == -1) ? 0 : priceChanges.size() - count;

        for (size_t i = startIndex; i < priceChanges.size(); ++i) {
            if(priceChanges[i]>0) {
                income += priceChanges[i];
            }else {
                outcome -= priceChanges[i];
            }
        }
        std::cout << "+ " <<income <<" - "<<outcome<< std::endl;
}

void FinanceManager::printTotalProfit() const {
    std::cout << "Total Profit: " << std::fixed << std::setprecision(2) << totalProfit << "\n";
}
