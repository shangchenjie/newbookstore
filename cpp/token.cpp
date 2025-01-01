#include "../src/token.h"
#include <vector>
#include <algorithm>
#include <cctype>

std::vector<std::string> Token::parseCommand(const std::string& input) {
    std::vector<std::string> tokens;
    std::string token;
    bool inQuotes = false;
    for (char ch : input) {
        if (ch == '"') {
            inQuotes = !inQuotes;
        } else if (ch == ' ' && !inQuotes) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token += ch;
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}
bool Token::isValidCommand(const std::string& command) {
    const std::vector<std::string> validCommands = {
        "quit", "exit", "su", "logout", "register", "passwd", "useradd", "delete",
        "show", "buy", "select", "modify", "import", "log", "report"
    };
    return std::find(validCommands.begin(), validCommands.end(), command) != validCommands.end();
}

bool Token::isValidUserID(const std::string& userID) {
    return !userID.empty() && userID.find(' ') == std::string::npos;
}

bool Token::isValidPassword(const std::string& password) {
    return !password.empty() && password.find(' ') == std::string::npos;
}

bool Token::isValidUsername(const std::string& username) {
    return !username.empty();
}

bool Token::isValidPrivilege(const std::string& privilege) {
    for (char ch : privilege) {
        if (!isdigit(ch)) {
            return false;
        }
    }
    return true;
}

bool Token::isValidISBN(const std::string& isbn) {
    return !isbn.empty() && isbn.find(' ') == std::string::npos;
}

bool Token::isValidBookName(const std::string& bookName) {
    return !bookName.empty();
}

bool Token::isValidAuthor(const std::string& author) {
    return !author.empty();
}

bool Token::isValidKeyword(const std::string& keyword) {
    return !keyword.empty();
}

bool Token::isValidPrice(const std::string& price) {
    bool hasDecimal = false;
    for (char ch : price) {
        if (!isdigit(ch) && ch != '.') {
            return false;
        }
        if (ch == '.') {
            if (hasDecimal) {
                return false;
            }
            hasDecimal = true;
        }
    }
    return true;
}

bool Token::isValidQuantity(const std::string& quantity) {
    for (char ch : quantity) {
        if (!isdigit(ch)) {
            return false;
        }
    }
    return true;
}

bool Token::isValidTotalCost(const std::string& totalCost) {
    bool hasDecimal = false;
    for (char ch : totalCost) {
        if (!isdigit(ch) && ch != '.') {
            return false;
        }
        if (ch == '.') {
            if (hasDecimal) {
                return false;
            }
            hasDecimal = true;
        }
    }
    return true;
}