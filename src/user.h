#ifndef USER_H
#define USER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstring>
#include "book.h"
#include "diary.h"

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

const int accountblock_size = 128;


struct User {
    char userID[31];
    char password[31];
    int privilege;
    char username[31];
};

struct accountHeadNode {
    int id;
    int prev_head;
    int nex_head;
    int size;
};
struct data {
    string userID;
    string ISBN;
};

const int accountsizeofP = sizeof(User);
const int accountsizeofH = sizeof(accountHeadNode);
extern User userbloc[] ;
extern accountHeadNode userlink[];
inline bool comp(const char* userIDx, const char* userIDy) {
    return strcmp(userIDx, userIDy) <= 0;
}

inline bool equal(const User& x, const User& y) {
    return strcmp(x.userID, y.userID) == 0 &&
           strcmp(x.password, y.password) == 0 &&
           x.privilege == y.privilege &&
           strcmp(x.username, y.username) == 0;
}

class accountNodeHead {
    fstream file;
    std::string file_name;

public:
    int new_id = 0;
    int cur_size = 0;
    int head = -1;

    accountNodeHead() = default;
    accountNodeHead(const std::string& file_name);
    ~accountNodeHead();

    int getSize();
    int getHead();
    accountHeadNode visitHead(int index);
    void writeaccountHead(int index, accountHeadNode ret);
    int addaccountHead(int index);
    void deleteaccountHead(int index);
};

class accountNodeBody {
    fstream file;
    std::string file_name;

public:
    accountNodeBody() = default;
    accountNodeBody(const std::string& file_name);
    ~accountNodeBody();
    void visitaccountNode(int index);
    void writeaccountNode(int index);
};

class UserManager {
private:
    //std::vector<User> userbloc;
    //std::vector<accountHeadNode> userlink;
    std::vector<data> log_;


    accountNodeHead accountHead;
    accountNodeBody accountBody;

public:
    UserManager() : accountHead("accounthead5"), accountBody("accountbody5") {
        accountinitialise();
    }
    ~UserManager();
    void deleteaccountNode(int index, const User& user);
    void addaccountNode(int index, const User& user);
    void add_account(const User& user);
    void delete_account(const char* userID);
    void suWithPassword(const char* userID,const char*password);
    void suWithoutPassword(const char* userID);
    void logout();
    User getcheck(const char* userID) ;
    bool check_privilege(const std::string& operation);
    void modifypassword(const User& old_account, const User& new_account);
    void selectbook(const std::string& ISBN);
    void changebook(Book& book);
    void accountinitialise();
    void flush();
    string getLastLogbook() const;
    User getLastLoguser() ;
    void ModifyLastBookInLogbook(const Book& newBookData);
};


#endif // USER_H
