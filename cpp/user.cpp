#include "../src/user.h"
#include "../src/diary.h"
#include <cassert>
#include <string>
#include <cstring> // 包含 strcmp 和 strlen

extern diaryManager diaryMgr;
User userbloc[128];
accountHeadNode userlink[100001];
extern void add_account(User);
extern BookManager bookMgr;
// accountNodeHead 类的实现
accountNodeHead::accountNodeHead(const std::string& file_name) {
    this->file_name = file_name;
    file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        file.open(file_name, std::ios::out);
        file.close();
        file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    }
}

accountNodeHead::~accountNodeHead() {
    file.close();
}

int accountNodeHead::getSize() {
    return cur_size;
}

int accountNodeHead::getHead() {
    return head;
}

accountHeadNode accountNodeHead::visitHead(int index) {
    accountHeadNode ret;
    file.seekg(index * accountsizeofH);
    file.read(reinterpret_cast<char*>(&ret), accountsizeofH);
    return ret;
}

void accountNodeHead::writeaccountHead(int index, accountHeadNode ret) {
    file.seekp(index * accountsizeofH);
    file.write(reinterpret_cast<char*>(&ret), accountsizeofH);
}

int accountNodeHead::addaccountHead(int index) {
    accountHeadNode new_head;
    new_head.id = new_id;
    new_head.prev_head = (index == -1) ? -1 : index;
    new_head.nex_head = (index == -1) ? -1 : userlink[index].nex_head;
    new_head.size = 0;

    if (head == -1) {
        head = new_id;
    }

    userlink[new_id] = new_head;
    if (index != -1) {
        userlink[index].nex_head = new_id;
        if (new_head.nex_head != -1) {
            userlink[new_head.nex_head].prev_head = new_id;
        }
    }
    writeaccountHead(new_id, new_head);
    new_id++;
    cur_size++;
    return new_id - 1;
}

void accountNodeHead::deleteaccountHead(int index) {
    accountHeadNode current = userlink[index];
    if (current.prev_head != -1) {
        userlink[current.prev_head].nex_head = current.nex_head;
    }
    if (current.nex_head != -1) {
        userlink[current.nex_head].prev_head = current.prev_head;
    }
    cur_size--;
}

// accountNodeBody 类的实现
accountNodeBody::accountNodeBody(const std::string& file_name) {
    this->file_name = file_name;
    file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        file.open(file_name, std::ios::out);
        file.close();
        file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    }
}

accountNodeBody::~accountNodeBody() {
    file.close();
}

void accountNodeBody::visitaccountNode(int index) {
    if (index != -1) {
        file.seekg(index * accountblock_size * accountsizeofP);
        file.read(reinterpret_cast<char*>(userbloc), userlink[index].size * accountsizeofP);
    }
}

void accountNodeBody::writeaccountNode(int index) {
    file.seekp(index * accountblock_size * accountsizeofP);
    file.write(reinterpret_cast<char*>(userbloc), userlink[index].size * accountsizeofP);
}

// UserManager 类的实现
UserManager::~UserManager() {
    flush();
}

void UserManager::addaccountNode(int index, const User& user) {
    accountBody.visitaccountNode(userlink[index].id);

    int left = 0, right = userlink[index].size - 1;
    int insertPos = userlink[index].size;
    while (left <= right) {
        int mid = (left + right) / 2;
        if (strcmp(userbloc[mid].userID, user.userID) <= 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
            insertPos = mid;
        }
    }

    for (int i = userlink[index].size; i > insertPos; --i) {
        userbloc[i] = userbloc[i - 1];
    }
    userbloc[insertPos] = user;
    userlink[index].size++;

    accountBody.writeaccountNode(userlink[index].id);
}

void UserManager::deleteaccountNode(int index, const User& user) {
    accountBody.visitaccountNode(userlink[index].id);

    int left = 0, right = userlink[index].size - 1;
    int deletePos = -1;
    while (left <= right) {
        int mid = (left + right) / 2;
        if (strcmp(userbloc[mid].userID, user.userID) == 0) {
            deletePos = mid;
            break;
        } else if (strcmp(userbloc[mid].userID, user.userID) < 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    if (deletePos != -1) {
        for (int i = deletePos; i < userlink[index].size - 1; ++i) {
            userbloc[i] = userbloc[i + 1];
        }
        userlink[index].size--;

        accountBody.writeaccountNode(userlink[index].id);
    }
}

void UserManager::add_account(const User& user) {
    User user_=getcheck(user.userID);
    if(user_.userID[0]!='\0') {
        std::cout<<"Invalid\n";
        if (!log_.empty()) {
            diaryMgr.adddiary(log_.back().userID, "Added user: " + std::string(user.userID)+"failed");
            return;
        }
    }else {
        int p = accountHead.getHead();
        int final;
        while (p != -1) {
            accountBody.visitaccountNode(userlink[p].id);
            if (userlink[p].size < accountblock_size && strcmp(userbloc[0].userID, user.userID) <= 0 && strcmp(userbloc[userlink[p].size - 1].userID, user.userID) >= 0) {
                addaccountNode(p, user);
                if (userlink[p].size == accountblock_size) {
                    int new_block_index = accountHead.addaccountHead(userlink[p].id);
                    userlink[p].size = accountblock_size / 2;
                    accountBody.writeaccountNode(userlink[p].id);

                    for (int i = accountblock_size / 2; i < accountblock_size; ++i) {
                        userbloc[i - accountblock_size / 2] = userbloc[i];
                    }
                    userlink[new_block_index].size = accountblock_size / 2;
                    accountBody.writeaccountNode(new_block_index);
                }
                return;
            }
            if (strcmp(userbloc[0].userID, user.userID) > 0) {
                p = userlink[p].prev_head;
                if (p == -1) {
                    p = 0;
                    addaccountNode(p, user);
                } else {
                    addaccountNode(p, user);
                }
                if (userlink[p].size == accountblock_size) {
                    int new_block_index = accountHead.addaccountHead(userlink[p].id);
                    userlink[p].size = accountblock_size / 2;
                    accountBody.writeaccountNode(userlink[p].id);

                    for (int i = accountblock_size / 2; i < accountblock_size; ++i) {
                        userbloc[i - accountblock_size / 2] = userbloc[i];
                    }
                    userlink[new_block_index].size = accountblock_size / 2;
                    accountBody.writeaccountNode(new_block_index);
                }
                return;
            }
            final = p;
            p = userlink[p].nex_head;
        }
        if (accountHead.getSize() != 0 && accountHead.getSize() != -1) {
            p = final;
            addaccountNode(p, user);
            if (userlink[p].size == accountblock_size) {
                int new_block_index = accountHead.addaccountHead(userlink[p].id);
                userlink[p].size = accountblock_size / 2;
                accountBody.writeaccountNode(userlink[p].id);

                for (int i = accountblock_size / 2; i < accountblock_size; ++i) {
                    userbloc[i - accountblock_size / 2] = userbloc[i];
                }
                userlink[new_block_index].size = accountblock_size / 2;
                accountBody.writeaccountNode(new_block_index);
            }
        } else {
            int new_block_index = accountHead.addaccountHead(-1);
            addaccountNode(new_block_index, user);
        }
        if (!log_.empty()) {
            diaryMgr.adddiary(log_.back().userID, "Added user: " + std::string(user.userID));
        }
    }
}

void UserManager::delete_account(const char* userID) {
    User user = getcheck(userID);
    if (user.userID[0] == '\0') {
        std::cout<<"Invalid\n";
        diaryMgr.adddiary(log_.back().userID, "Deleted user: " + std::string(user.userID) + "failed");
        return;
    }
    if(!log_.empty()) {
        for(size_t i=0;i<log_.size();++i) {
            if(strcmp(log_[i].userID.c_str(),userID)==0) {
                std::cout<<"Invalid\n";
                return;
            }
        }
    }
    int p = accountHead.getHead();
    while (p != -1) {
        accountBody.visitaccountNode(userlink[p].id);
        if (userlink[p].size > 0 && strcmp(userbloc[0].userID, user.userID) <= 0 && strcmp(userbloc[userlink[p].size - 1].userID, user.userID) >= 0) {
            deleteaccountNode(p, user);
            if (userlink[p].size == 0) {
                accountHead.deleteaccountHead(p);
            }
            return;
        }
        if (strcmp(userbloc[0].userID, user.userID) > 0) {
            return;
        }
        p = userlink[p].nex_head;
    }
    if (!log_.empty()) {
        diaryMgr.adddiary(log_.back().userID, "Deleted user: " + std::string(user.userID));
    }
}

User UserManager::getcheck(const char* userID) {
    User user = {};
    int p = accountHead.getHead();

    while (p != -1) {
        accountBody.visitaccountNode(userlink[p].id);

        if (userlink[p].size > 0 &&
            strcmp(userbloc[0].userID, userID) <= 0 &&
            strcmp(userbloc[userlink[p].size - 1].userID, userID) >= 0) {

            int left = 0, right = userlink[p].size - 1;
            while (left <= right) {
                int mid = left + (right - left) / 2;
                int cmpResult = strcmp(userbloc[mid].userID, userID);

                if (cmpResult == 0) {
                    user = userbloc[mid];
                    return user;
                } else if (cmpResult < 0) {
                    left = mid + 1;
                } else {
                    right = mid - 1;
                }
            }
        }

        p = userlink[p].nex_head;
    }

    return user;
}

void UserManager::suWithPassword(const char* userID,const char*password) {
    User user = getcheck(userID);
    if (strcmp(user.password, password) == 0) {
        data data1;
        data1.userID = std::string(user.userID);
        data1.ISBN = "";
        log_.push_back(data1);
        diaryMgr.adddiary(userID, "Logged in");
    } else {
        std::cout<<"Invalid\n";
        diaryMgr.adddiary(userID, "failed to Log in");
    }
}

void UserManager::suWithoutPassword(const char* userID) {
    User user = getcheck(userID);
    if (user.userID[0]!='\0') {
        data data2;
        data2.userID = std::string(user.userID);
        data2.ISBN = "";
        log_.push_back(data2);
        diaryMgr.adddiary(userID, "Logged in");
    } else {
        std::cout<<"Invalid\n";
        diaryMgr.adddiary(userID, "failed to Log in");
    }
}

void UserManager::logout() {
    if (!log_.empty()) {
        std::string userID = log_.back().userID;
        log_.erase(log_.end() - 1);
        diaryMgr.adddiary("user:" + userID, "Logged out");
    }else {
        std::cout<<"Invalid\n";
        diaryMgr.adddiary("" , "failed Logged out");
    }
}

bool UserManager::check_privilege(const std::string& operation) {
    User user;
    if (!log_.empty()) {
        user = getcheck(log_.back().userID.c_str());
    }else {
        user.privilege=0;
    }
    if (user.userID[0] == '\0' && operation != "su") {
        return false;
    }
    if (operation == "add_account" && user.privilege >= 3) {
        return true;
    } else if (operation == "delete_account" && user.privilege >= 7) {
        return true;
    } else if (operation == "modifypassword" && user.privilege >= 1) {
        return true;
    } else if (operation == "select_book" && user.privilege >= 3) {
        return true;
    } else if (operation == "sell_book" && user.privilege >= 0) {
        return true;
    } else if (operation == "add_book" && user.privilege >= 3) {
        return true;
    } else if (operation == "import_book" && user.privilege >= 3) {
        return true;
    } else if (operation == "modify_book" && user.privilege >= 3) {
        return true;
    } else if (operation == "show_book" && user.privilege >= 1) {
        return true;
    } else if (operation == "show_import" && user.privilege >= 7) {
        return true;
    } else if (operation == "show_sell" && user.privilege >= 7) {
        return true;
    } else if (operation == "show_finance" && user.privilege >= 7) {
        return true;
    }else if (operation == "log" && user.privilege >= 7) {
        return true;
    }else if (operation == "su" ) {
        return true;
    } else {
        return false;
    }
}

void UserManager::modifypassword(const User& old_account, const User& new_account) {
    int p = accountHead.getHead();
    while (p != -1) {
        accountBody.visitaccountNode(userlink[p].id);
        for (int i = 0; i < userlink[p].size; ++i) {
            if (strcmp(userbloc[i].userID, old_account.userID) == 0) {
                userbloc[i] = new_account;
                accountBody.writeaccountNode(userlink[p].id);
                if (!log_.empty()) {
                    diaryMgr.adddiary(log_.back().userID, "Modified account from " + std::string(old_account.userID) + " to " + std::string(new_account.userID));
                }
                return;
            }
        }
        p = userlink[p].nex_head;
    }
    diaryMgr.adddiary(log_.back().userID, "failed to Modified account from " + std::string(old_account.userID) + " to " + std::string(new_account.userID));
}

void UserManager::selectbook(const std::string& ISBN) {
    Book book = bookMgr.FindByISBN(ISBN);

    if (book.ISBN[0] == '\0') {
        Book newBook;
        strncpy(newBook.ISBN, ISBN.c_str(), sizeof(newBook.ISBN) - 1);
        newBook.ISBN[sizeof(newBook.ISBN) - 1] = '\0';
        newBook.title[0] = '\0';
        newBook.author[0] = '\0';
        newBook.keywords[0] = '\0';
        newBook.price = 0.0;
        newBook.stock = 0;
        bookMgr.Insert(newBook);
        book = newBook;
    }
    log_.back().ISBN=ISBN;
    if (!log_.empty()) {
        diaryMgr.adddiary(log_.back().userID, "Selected book: " + std::string(book.title));
    }
}

void UserManager::changebook(Book& book) {
    if (!log_.empty()) {
        string ISBN=log_.back().ISBN ;
        log_.back().ISBN = std::string(book.ISBN);
        diaryMgr.adddiary(log_.back().userID, "changed book: " + ISBN + "to" +std::string(book.title));
    }
}

void UserManager::accountinitialise() {
    fstream file_;
    file_.open("accounth5", std::ios::in | std::ios::out);
    if (!file_.is_open()) {
        file_.open("accounth5", std::ios::out);
        file_.close();
        file_.open("accounth5", std::ios::in | std::ios::out | std::ios::binary);
        accountHead.new_id = 0;
        accountHead.cur_size = 0;
        accountHead.head = -1;
        file_.write(reinterpret_cast<char*>(&accountHead.new_id), sizeof(int));
        file_.write(reinterpret_cast<char*>(&accountHead.cur_size), sizeof(int));
        file_.write(reinterpret_cast<char*>(&accountHead.head), sizeof(int));
    } else {
        file_.seekp(0, std::ios::beg);
        file_.read(reinterpret_cast<char*>(&accountHead.new_id), sizeof(int));
        file_.read(reinterpret_cast<char*>(&accountHead.cur_size), sizeof(int));
        file_.read(reinterpret_cast<char*>(&accountHead.head), sizeof(int));
    }
    if (accountHead.head == -1) {
        User rootUser;
        strcpy(rootUser.userID, "root");
        strcpy(rootUser.password, "sjtu");
        rootUser.privilege = 7;
        strcpy(rootUser.username, "");

        accountHeadNode headNode;
        headNode.id = 0;
        headNode.prev_head = -1;
        headNode.nex_head = -1;
        headNode.size = 1;
        userlink[0] = headNode;
        userbloc[0] = rootUser;
        add_account(rootUser);
        return;
    }
    int p = accountHead.head;
    while (p != -1) {
        userlink[p] = accountHead.visitHead(p);
        p = userlink[p].nex_head;
    }
    file_.close();
}

void UserManager::flush() {
    fstream file_;
    file_.open("accounth5", std::ios::in | std::ios::out | std::ios::binary);
    file_.seekp(0);
    file_.write(reinterpret_cast<char*>(&accountHead.new_id), sizeof(int));
    file_.write(reinterpret_cast<char*>(&accountHead.cur_size), sizeof(int));
    file_.write(reinterpret_cast<char*>(&accountHead.head), sizeof(int));
    file_.close();
    int p = accountHead.head;
    while (p != -1) {
        accountHead.writeaccountHead(p, userlink[p]);
        p = userlink[p].nex_head;
    }
}

std::string UserManager::getLastLogbook() const {
    if (!log_.empty()) {
        return log_.back().ISBN;
    } else {
        return "";
    }
}

User UserManager::getLastLoguser() {
    User user = {};
    if (!log_.empty()) {
        std::string userID= log_.back().userID;
        int p = accountHead.getHead();
        while (p != -1) {
            accountBody.visitaccountNode(userlink[p].id);

            if (userlink[p].size > 0 &&
                strcmp(userbloc[0].userID, userID.c_str()) <= 0 &&
                strcmp(userbloc[userlink[p].size - 1].userID, userID.c_str()) >= 0) {

                int left = 0, right = userlink[p].size - 1;
                while (left <= right) {
                    int mid = left + (right - left) / 2;
                    int cmpResult = strcmp(userbloc[mid].userID, userID.c_str());

                    if (cmpResult == 0) {
                        user = userbloc[mid];
                        return user;
                    } else if (cmpResult < 0) {
                        left = mid + 1;
                    } else {
                        right = mid - 1;
                    }
                }
                }

            p = userlink[p].nex_head;
        }
    }
    return user;
}
void UserManager::ModifyLastBookInLogbook(const Book& newBookData) {
    if (log_.empty()) {
        std::cerr << "Invalid\n";
        return;
    }
    string lastISBN = log_.back().ISBN;
    string findISBN = newBookData.ISBN;
    Book findbook = bookMgr.FindByISBN(findISBN);
    Book book = bookMgr.FindByISBN(lastISBN);
    if (book.ISBN[0] == '\0'||findbook.ISBN[0]!='\0') {
        std::cerr << "Invalid\n";
        return;
    }
    if (newBookData.ISBN[0] != '\0'&&compareISBN(newBookData.ISBN,book.ISBN)!=0) {
        strncpy(book.ISBN, newBookData.ISBN, sizeof(book.ISBN) - 1);
        book.ISBN[sizeof(book.ISBN) - 1] = '\0';
        log_.back().ISBN = newBookData.ISBN;
    }
    if (newBookData.title[0] != '\0') {
        strncpy(book.title, newBookData.title, sizeof(book.title) - 1);
        book.title[sizeof(book.title) - 1] = '\0';
    }
    if (newBookData.author[0] != '\0') {
        strncpy(book.author, newBookData.author, sizeof(book.author) - 1);
        book.author[sizeof(book.author) - 1] = '\0';
    }
    if (newBookData.keywords[0] != '\0') {
        strncpy(book.keywords, newBookData.keywords, sizeof(book.keywords) - 1);
        book.keywords[sizeof(book.keywords) - 1] = '\0';
    }
    if (newBookData.price != '\0') {
        book.price = newBookData.price;
    }
    if (newBookData.stock != '\0') {
        book.stock = newBookData.stock;
    }
    bookMgr.Delete(lastISBN);
    bookMgr.Insert(book);
    diaryMgr.adddiary(log_.back().userID, "modifyed book: " + lastISBN + "to" +std::string(book.title));
}
