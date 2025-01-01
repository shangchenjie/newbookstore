#include "book.h"
#include "finance.h"
#include "../src/diary.h"
#include "../src/user.h"
#include <iostream>
#include <string>
#include "../src/token.h"
#include "set"

BookManager bookMgr;
UserManager userMgr;

std::vector<std::pair<std::string, std::string> > parseModifyArgs(const std::string &args) {
    std::vector<std::pair<std::string, std::string> > result;
    std::istringstream iss(args);
    std::string token;
    while (std::getline(iss, token, ' ')) {
        if (token.empty()) continue;
        size_t equalPos = token.find('=');
        if (equalPos != std::string::npos) {
            std::string key = token.substr(0, equalPos);
            std::string value = token.substr(equalPos + 1);
            if (!value.empty() && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }
            result.emplace_back(key, value);
        }
    }
    return result;
}

void processCommand(const std::string &input) {
    auto tokens = Token::parseCommand(input);
    if (tokens.empty()) {
        return;
    }

    std::string command = tokens[0];
    if (!Token::isValidCommand(command)) {
        std::cout << "Invalid\n";
        return;
    }

    if (command == "su") {
        if (tokens.size() < 2) {
            std::cout << "Invalid\n";
            return;
        }
        std::string userID = tokens[1];
        std::string password = tokens.size() > 2 ? tokens[2] : "";

        if (!Token::isValidUserID(userID)) {
            std::cout << "Invalid\n";
            return;
        }
        if (userMgr.check_privilege("su")) {
            User targetUser = userMgr.getcheck(userID.c_str());
            if (userMgr.getLastLogprivilege() >= targetUser.privilege) {
                userMgr.suWithoutPassword(targetUser);
            } else {
                if (!password.empty() && !Token::isValidPassword(password)) {
                    std::cout << "Invalid\n";
                    return;
                }
                userMgr.suWithPassword(targetUser, password.c_str());
            }
        }
    } else if (command == "useradd") {
        if (tokens.size() < 5) {
            std::cout << "Invalid\n";
            return;
        }
        std::string userID = tokens[1];
        std::string password = tokens[2];
        std::string privilege = tokens[3];
        std::string username = tokens[4];

        if (!Token::isValidUserID(userID)) {
            std::cout << "Invalid\n";
            return;
        }
        if (!Token::isValidPassword(password)) {
            std::cout << "Invalid\n";
            return;
        }
        if (!Token::isValidPrivilege(privilege)) {
            std::cout << "Invalid\n";
            return;
        }
        if (!Token::isValidUsername(username)) {
            std::cout << "Invalid\n";
            return;
        }
        User user;
        strcpy(user.userID, userID.c_str());
        strcpy(user.password, password.c_str());
        user.privilege = std::stoi(privilege);
        strcpy(user.username, username.c_str());

        if (userMgr.check_privilege("add_account")&&userMgr.getLastLogprivilege()>user.privilege) {
            userMgr.add_account(user);
        } else {
            std::cout << "Invalid\n";
        }
    } else if (command == "logout") {
        userMgr.logout();
    } else if (command == "register") {
        if (tokens.size() != 4) {
            std::cout << "Invalid\n";
            return;
        }
        std::string userID = tokens[1];
        std::string password = tokens[2];
        std::string username = tokens[3];

        if (!Token::isValidUserID(userID)) {
            std::cout << "Invalid\n";
            return;
        }
        if (!Token::isValidPassword(password)) {
            std::cout << "Invalid\n";
            return;
        }
        if (!Token::isValidUsername(username)) {
            std::cout << "Invalid\n";
            return;
        }

        User user;
        strcpy(user.userID, userID.c_str());
        strcpy(user.password, password.c_str());
        user.privilege = 1;
        strcpy(user.username, username.c_str());
        userMgr.add_account(user);
    } else if (command == "passwd") {
        if (tokens.size() < 2) {
            std::cout << "Invalid\n";
            return;
        }
        std::string userID = tokens[1];
        std::string currentPassword, newPassword;

        if (!Token::isValidUserID(userID)) {
            std::cout << "Invalid\n";
            return;
        }

        //User currentUser = userMgr.getcheck(userMgr.getLastLoguser().userID);
        //User targetUser = userMgr.getcheck(userID.c_str());

        if (userMgr.check_privilege("modifypassword")) {
            if (userMgr.getLastLogprivilege() >= 7) {
                if (tokens.size() < 3) {
                    std::cout << "Invalid\n";
                    return;
                }
                newPassword = tokens[2];
                if (!Token::isValidPassword(newPassword)) {
                    std::cout << "Invalid\n";
                    return;
                }
                userMgr.modifypassword(userID.c_str(), newPassword.c_str());
            } else {
                if (tokens.size() < 4) {
                    std::cout << "Invalid\n";
                    return;
                }
                currentPassword = tokens[2];
                newPassword = tokens[3];
                if (!Token::isValidPassword(currentPassword) || !Token::isValidPassword(newPassword)) {
                    std::cout << "Invalid\n";
                    return;
                }
                else {
                    userMgr.modifypasswordwithcurrent(userID.c_str(), currentPassword.c_str(),newPassword.c_str());
                }
            }
        } else {
            std::cout << "Invalid\n";
        }
    } else if (command == "delete") {
        if (tokens.size() < 2) {
            std::cout << "Invalid\n";
            return;
        }
        std::string userID = tokens[1];

        if (!Token::isValidUserID(userID)) {
            std::cout << "Invalid\n";
            return;
        }
        if (userMgr.check_privilege("delete_account")) {
            userMgr.delete_account(userID.c_str());
        } else {
            std::cout << "Invalid\n";
        }
    } else if (command == "buy") {
        if (tokens.size() < 3) {
            std::cout << "Invalid\n";
            return;
        }
        std::string ISBN = tokens[1];
        std::string quantityStr = tokens[2];

        if (!Token::isValidISBN(ISBN)) {
            std::cout << "Invalid\n";
            return;
        }
        if (!Token::isValidQuantity(quantityStr)) {
            std::cout << "Invalid\n";
            return;
        }

        int quantity = std::stoi(quantityStr);
        if (userMgr.check_privilege("sell_book") && ISBN[0] != '\0') {
            bookMgr.SellBook(ISBN, quantity);
        }else {
            std::cout << "Invalid\n";
        }
    } else if (command == "select") {
        if (tokens.size() < 2) {
            std::cout << "Invalid\n";
            return;
        }
        std::string ISBN = tokens[1];

        if (!Token::isValidISBN(ISBN)) {
            std::cout << "Invalid\n";
            return;
        }

        if (userMgr.check_privilege("select_book")) {
            userMgr.selectbook(ISBN);
        } else {
            std::cout << "Invalid\n";
        }
    } else if (command == "import") {
        if (tokens.size() < 3) {
            std::cout << "Invalid\n";
            return;
        }
        std::string quantityStr = tokens[1];
        std::string totalCostStr = tokens[2];

        if (!Token::isValidQuantity(quantityStr)) {
            std::cout << "Invalid\n";
            return;
        }
        if (!Token::isValidTotalCost(totalCostStr)) {
            std::cout << "Invalid\n";
            return;
        }

        int quantity = std::stoi(quantityStr);
        double totalCost = std::stod(totalCostStr);

        if (userMgr.check_privilege("import_book")) {
            bookMgr.importBook(quantity, totalCost);
        } else {
            std::cout << "Invalid\n";
        }
    } else if (command == "show") {
        if (tokens.size() == 1) {
            bookMgr.showall();
            return;
        }

        std::string subCommand = tokens[1];

        if (subCommand == "finance"&&userMgr.check_privilege("show_finance")) {

            if (tokens.size() >3) {
                std::cout << "Invalid\n";
                return;
            }
            if (tokens.size() ==2) {
                int count=-1;
                financeMgr.printTransactions(count);
                return;
            }
            std::string countStr = tokens[2];
            if (!Token::isValidQuantity(countStr)) {
                std::cout << "Invalid\n";
                return;
            }

            int count = std::stoi(countStr);
            financeMgr.printTransactions(count);
        }
        else if (subCommand.find("-") == 0&&userMgr.check_privilege("show_book")) {
            size_t equalSign = subCommand.find('=');
            if (equalSign == std::string::npos) {
                std::cout << "Invalid\n";
                return;
            }

            std::string field = subCommand.substr(0, equalSign);
            std::string value = subCommand.substr(equalSign + 1);

            if (value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }
            bookMgr.show(field, value);
        }
        else {
            std::cout << "Invalid\n";
        }
    } else if (command == "report") {
        if(tokens.size() < 1) {
            std::cout << "Invalid\n";
        }
        else if(tokens[1]=="finance"&&userMgr.check_privilege("show_finance")) {
            if (userMgr.check_privilege("report_finance")) {
                financeMgr.printTotalProfit();
            } else {
                std::cout << "Invalid\n";
            }
        }
        else if (tokens[1]=="employee"&&userMgr.check_privilege("show_finance")) {
            if (userMgr.check_privilege("report_employee")) {
                diaryMgr.printAlldiarys();
            } else {
                std::cout << "Invalid\n";
            }
        }
    } else if (command == "modify") {
        if (!userMgr.check_privilege("modify_book")) {
            std::cout << "Invalid\n";
            return;
        }
        Book newBookData;
        newBookData.ISBN[0] = '\0';
        newBookData.title[0] = '\0';
        newBookData.author[0] = '\0';
        newBookData.keywords[0] = '\0';
        newBookData.price = '\0';
        newBookData.stock = '\0';
        bool isValid = true;
        for (size_t i = 1; i < tokens.size(); ++i) {
            const std::string &token = tokens[i];
            if (token.find("-ISBN=") == 0) {
                std::string isbn = token.substr(6);
                if (!Token::isValidISBN(isbn)) {
                    isValid = false;
                } else {
                    strncpy(newBookData.ISBN, isbn.c_str(), sizeof(newBookData.ISBN) - 1);
                }
            } else if (token.find("-name=") == 0) {
                std::string name = token.substr(6);
                if (name=="" ) {
                    isValid = false;
                } else {
                    name = name.substr(0, name.size() );
                    if (name.empty()) {
                        isValid = false;
                    } else {
                        strncpy(newBookData.title, name.c_str(), sizeof(newBookData.title) - 1);
                    }
                }
            } else if (token.find("-author=") == 0) {
                std::string author = token.substr(8);
                if (author.empty() ) {
                    isValid = false;
                } else {
                    author = author.substr(0, author.size() );
                    if (author.empty()) {
                        isValid = false;
                    } else {
                        strncpy(newBookData.author, author.c_str(), sizeof(newBookData.author) - 1);
                    }
                }
            } else if (token.find("-keyword=") == 0) {
                std::string keyword = token.substr(9);
                if (keyword.empty()) {
                    isValid = false;
                } else {
                    std::vector<std::string> keywordsList;
                    std::stringstream ss(keyword);
                    std::string singleKeyword;
                    while (std::getline(ss, singleKeyword, '|')) {
                        keywordsList.push_back(singleKeyword);
                    }

                    std::set<std::string> uniqueKeywords(keywordsList.begin(), keywordsList.end());
                    if (uniqueKeywords.size() != keywordsList.size()) {
                        isValid = false;
                    } else {
                        strncpy(newBookData.keywords, keyword.c_str(), sizeof(newBookData.keywords) - 1);
                        newBookData.keywords[sizeof(newBookData.keywords) - 1] = '\0';
                    }
                }
            }else if (token.find("-price=") == 0) {
                std::string priceStr = token.substr(7);
                try {
                    newBookData.price = std::stod(priceStr);
                    if (newBookData.price < 0) {
                        isValid = false;
                    }
                } catch (const std::invalid_argument &) {
                    isValid = false;
                }
            } else {
                isValid = false;
            }
        }
        if (isValid) {
            userMgr.ModifyLastBookInLogbook(newBookData);
        } else {
            std::cout << "Invalid\n";
        }
    } else if (command == "log") {
        if (userMgr.check_privilege("log")) {
            diaryMgr.printAlldiarys();
        } else {
            std::cout << "Invalid\n";
        }
    } else if (command == "quit" || command == "exit") {
        return;
    } else {
        std::cout<<"Invalid\n";
    }
}

int main() {

    //freopen("testcase4.in", "r", stdin);
    //freopen("output.txt", "w", stdout);
    std::string input;
    while (true) {
        std::getline(std::cin, input);
        if (input.empty()||input == "quit" || input == "exit") {
            break;
        } else {
            processCommand(input);
        }
    }


    return 0;
}
