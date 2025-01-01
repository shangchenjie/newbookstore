
#ifndef TOKEN_H
#define TOKEN_H
#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

class Token {
public:
    static std::vector<std::string> parseCommand(const std::string& input);

    static bool isValidCommand(const std::string& command);

    static bool isValidUserID(const std::string& userID);

    static bool isValidPassword(const std::string& password);

    static bool isValidUsername(const std::string& username);

    static bool isValidPrivilege(const std::string& privilege);

    static bool isValidISBN(const std::string& isbn);

    static bool isValidBookName(const std::string& bookName);

    static bool isValidAuthor(const std::string& author);

    static bool isValidKeyword(const std::string& keyword);

    static bool isValidPrice(const std::string& price);

    static bool isValidQuantity(const std::string& quantity);

    static bool isValidTotalCost(const std::string& totalCost);
};

#endif // INPUT_HANDLER_H
#endif //TOKEN_H
