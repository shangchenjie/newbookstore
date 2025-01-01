#ifndef FINANCE_H
#define FINANCE_H

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>

class FinanceManager {
private:
    std::vector<double> priceChanges;
    double totalProfit;
    std::string fileName;

    void loadFromFile();
    void saveToFile();
public:
    FinanceManager(const std::string& file);
    ~FinanceManager();

    void recordPriceChange(double change);
    double getTotalProfit() const;
    void printTransactions(int count = -1) const;
    void printTotalProfit() const;
};

extern FinanceManager financeMgr;

#endif // FINANCE_H