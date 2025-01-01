#include "book.h"
#include "finance.h"
#include "../src/diary.h"
#include "../src/user.h"


Book* ISBNbloc=new Book[128];
ISBNHeadNode ISBNlink[100001];
extern UserManager userMgr;

ISBNNodeHead::ISBNNodeHead(const string& file_name) {
    this->file_name = file_name;
    file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        file.open(file_name, std::ios::out);
        file.close();
        file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    }
}

ISBNNodeHead::~ISBNNodeHead() {
    file.close();
}

int ISBNNodeHead::getSize() {
    return cur_size;
}

int ISBNNodeHead::getHead() {
    return head;
}

ISBNHeadNode ISBNNodeHead::ISBNvisitHead(int index) {
    ISBNHeadNode ret;
    file.seekg(index * sizeofH);
    file.read(reinterpret_cast<char*>(&ret), sizeofH);
    return ret;
}

void ISBNNodeHead::ISBNwriteHead(int index, ISBNHeadNode ret) {
    file.seekp(index * sizeofH);
    file.write(reinterpret_cast<char*>(&ret), sizeofH);
}

int ISBNNodeHead::ISBNaddHead(int index) {
    ISBNHeadNode new_head;
    new_head.id = new_id;
    new_head.prev_head = (index == -1) ? -1 : index;
    new_head.nex_head = (index == -1) ? -1 : ISBNlink[index].nex_head;
    new_head.size = 0;
    strcpy(new_head.minISBN, "");

    if (head == -1) {
        head++;
    }
    ISBNlink[new_id] = new_head;
    if (index != -1) {
        ISBNlink[index].nex_head = new_id;
        if (new_head.nex_head != -1) {
            ISBNlink[new_head.nex_head].prev_head = new_id;
        }
    }
    ISBNwriteHead(new_id, new_head);
    new_id++;
    cur_size++;
    return new_id - 1;
}

void ISBNNodeHead::ISBNdeleteHead(int index) {
    ISBNHeadNode current = ISBNlink[index];
    if (current.prev_head != -1) {
        ISBNlink[current.prev_head].nex_head = current.nex_head;
    }
    if (current.nex_head != -1) {
        ISBNlink[current.nex_head].prev_head = current.prev_head;
    }
    cur_size--;
}

ISBNNodeBody::ISBNNodeBody(const string& file_name) {
    this->file_name = file_name;
    file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        file.open(file_name, std::ios::out);
        file.close();
        file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    }
}

ISBNNodeBody::ISBNNodeBody(const ISBNNodeBody& other) {
    file_name = other.file_name;
    file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        file.open(file_name, std::ios::out);
        file.close();
        file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    }
}

ISBNNodeBody& ISBNNodeBody::operator=(const ISBNNodeBody& other) {
    if (this != &other) {
        file.close();
        file_name = other.file_name;
        file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            file.open(file_name, std::ios::out);
            file.close();
            file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
        }
    }
    return *this;
}

ISBNNodeBody::~ISBNNodeBody() {
    file.close();
}

void ISBNNodeBody::ISBNvisitNode(int index) {
    file.seekg(index * block_size * sizeofP);
    file.read(reinterpret_cast<char*>(ISBNbloc), ISBNlink[index].size * sizeofP);
}

void ISBNNodeBody::ISBNwriteNode(int index) {
    file.seekp(index * block_size * sizeofP);
    file.write(reinterpret_cast<char*>(ISBNbloc), ISBNlink[index].size * sizeofP);
}


BookManager::~BookManager() {
    ISBNflush();
}

void BookManager::ISBNaddNode(int index, const Book& book) {
    ISBNBody.ISBNvisitNode(ISBNlink[index].id);
    int left = 0, right = ISBNlink[index].size - 1;
    int insertPos = ISBNlink[index].size;
    while (left <= right) {
        int mid = (left + right) / 2;
        if (ISBNbloc[mid] < book) {
            left = mid + 1;
        } else {
            right = mid - 1;
            insertPos = mid;
        }
    }

    for (int i = ISBNlink[index].size; i > insertPos; --i) {
        ISBNbloc[i] = ISBNbloc[i - 1];
    }

    ISBNbloc[insertPos] = book;
    ISBNlink[index].size++;

    ISBNBody.ISBNwriteNode(ISBNlink[index].id);
}

void BookManager::ISBNdeleteNode(int index, const Book& book) {
    ISBNBody.ISBNvisitNode(ISBNlink[index].id);
    Book plankbook;
    plankbook.ISBN[0] = '\0';
    plankbook.title[0] = '\0';
    plankbook.author[0] = '\0';
    plankbook.keywords[0] = '\0';
    plankbook.price = '\0';
    plankbook.stock = '\0';
    int left = 0, right = ISBNlink[index].size - 1;
    int deletePos = -1;
    while (left <= right) {
        int mid = (left + right) / 2;
        if (strcmp(ISBNbloc[mid].ISBN, book.ISBN) == 0) {
            deletePos = mid;
            break;
        } else if (ISBNbloc[mid] < book) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    if (deletePos != -1) {
        for (int i = deletePos; i < ISBNlink[index].size - 1; ++i) {
            ISBNbloc[i] = ISBNbloc[i + 1];
        }
        ISBNbloc[ISBNlink[index].size - 1]=plankbook;
        ISBNlink[index].size--;
        ISBNBody.ISBNwriteNode(ISBNlink[index].id);
    }
}

void BookManager::Insert(const Book& book) {
    int p = ISBNHead.getHead();
    int final;
    while (p != -1) {
        ISBNBody.ISBNvisitNode(ISBNlink[p].id);
        if (ISBNlink[p].size > 0 && compareISBN(ISBNbloc[0].ISBN , book.ISBN)<=0 && compareISBN(ISBNbloc[ISBNlink[p].size - 1].ISBN  ,book.ISBN)>=0) {
            ISBNaddNode(p, book);
            if (ISBNlink[p].size == block_size) {
                int new_block_index = ISBNHead.ISBNaddHead(ISBNlink[p].id);
                ISBNlink[p].size = block_size / 2;
                ISBNBody.ISBNwriteNode(ISBNlink[p].id);

                for (int i = block_size / 2; i < block_size; ++i) {
                    ISBNbloc[i - block_size / 2] = ISBNbloc[i];
                }
                ISBNlink[new_block_index].size = block_size / 2;
                ISBNBody.ISBNwriteNode(new_block_index);
            }
            return;
        }
        if (!(ISBNbloc[0] < book)) {
            p = ISBNlink[p].prev_head;
            if (p == -1) {
                p = 0;
                ISBNaddNode(p, book);
            } else {
                ISBNaddNode(p, book);
            }
            if (ISBNlink[p].size == block_size) {
                int new_block_index = ISBNHead.ISBNaddHead(ISBNlink[p].id);
                ISBNlink[p].size = block_size / 2;
                ISBNBody.ISBNwriteNode(ISBNlink[p].id);

                for (int i = block_size / 2; i < block_size; ++i) {
                    ISBNbloc[i - block_size / 2] = ISBNbloc[i];
                }
                ISBNlink[new_block_index].size = block_size / 2;
                ISBNBody.ISBNwriteNode(new_block_index);
            }
            return;
        }
        final = p;
        p = ISBNlink[p].nex_head;
    }
    if (ISBNHead.getSize() != 0) {
        p = final;
        ISBNaddNode(p, book);
        if (ISBNlink[p].size == block_size) {
            int new_block_index = ISBNHead.ISBNaddHead(ISBNlink[p].id);
            ISBNlink[p].size = block_size / 2;
            ISBNBody.ISBNwriteNode(ISBNlink[p].id);

            for (int i = block_size / 2; i < block_size; ++i) {
                ISBNbloc[i - block_size / 2] = ISBNbloc[i];
            }
            ISBNlink[new_block_index].size = block_size / 2;
            ISBNBody.ISBNwriteNode(new_block_index);
        }
    } else {
        int new_block_index = ISBNHead.ISBNaddHead(-1);
        ISBNaddNode(new_block_index, book);
    }
}

void BookManager::Delete(const string& ISBN) {
    Book book;
    strcpy(book.ISBN, ISBN.c_str());

    int p = ISBNHead.getHead();
    while (p != -1) {
        ISBNBody.ISBNvisitNode(ISBNlink[p].id);
        if (ISBNlink[p].size > 0 && compareISBN(ISBNbloc[0].ISBN , book.ISBN)<=0 && compareISBN(ISBNbloc[ISBNlink[p].size - 1].ISBN  ,book.ISBN)>=0) {
            ISBNdeleteNode(p, book);
            if (ISBNlink[p].size == 0&&ISBNHead.cur_size!=1) {
                ISBNHead.ISBNdeleteHead(p);
            }
            return;
        }
        if (!(ISBNbloc[0] < book)) {
            return;
        }
        p = ISBNlink[p].nex_head;
    }
}

Book BookManager::FindByISBN(const string& ISBN) {
    Book book;
    strcpy(book.ISBN, ISBN.c_str());

    int p = ISBNHead.getHead();
    while (p != -1) {
        ISBNBody.ISBNvisitNode(ISBNlink[p].id);
        if (ISBNlink[p].size > 0 && compareISBN(ISBNbloc[0].ISBN ,book.ISBN)<=0 && compareISBN(ISBNbloc[ISBNlink[p].size - 1].ISBN , book.ISBN)>=0) {
            for (int i = 0; i < ISBNlink[p].size; ++i) {
                if (strcmp(ISBNbloc[i].ISBN, ISBN.c_str()) == 0) {
                    return ISBNbloc[i];
                }
            }
        }
        if (compareISBN(ISBNbloc[0].ISBN ,book.ISBN)>0) {
            break;
        }
        p = ISBNlink[p].nex_head;
    }
    return Book();
}

Book BookManager::FindByTitle(const string& title) {
    int p = ISBNHead.getHead();
    while (p != -1) {
        ISBNBody.ISBNvisitNode(ISBNlink[p].id);
        for (int i = 0; i < ISBNlink[p].size; ++i) {
            if (strcmp(ISBNbloc[i].title, title.c_str()) == 0) {
                return ISBNbloc[i];
            }
        }
        p = ISBNlink[p].nex_head;
    }
    return Book();
}

Book BookManager::FindByAuthor(const string& author) {
    int p = ISBNHead.getHead();
    while (p != -1) {
        ISBNBody.ISBNvisitNode(ISBNlink[p].id);
        for (int i = 0; i < ISBNlink[p].size; ++i) {
            if (strcmp(ISBNbloc[i].author, author.c_str()) == 0) {
                return ISBNbloc[i];
            }
        }
        p = ISBNlink[p].nex_head;
    }
    return Book();
}
Book BookManager::FindBookByExactKeywords(const std::string& keywords) {
    int p = ISBNHead.getHead();


    while (p != -1) {
        ISBNBody.ISBNvisitNode(ISBNlink[p].id);
        for (int i = 0; i < ISBNlink[p].size; ++i) {
            Book currentBook = ISBNbloc[i];

            if (areKeywordsEqual(currentBook.keywords, keywords)) {
                return currentBook;
            }
        }
        p = ISBNlink[p].nex_head;
    }

    return Book();
}
void BookManager::ISBNinitialise() {
    fstream file_;
    file_.open("h9", std::ios::in | std::ios::out);
    if (!file_.is_open()) {
        file_.open("h9", std::ios::out);
        file_.close();
        file_.open("h9", std::ios::in | std::ios::out | std::ios::binary);
        ISBNHead.new_id = 0;
        ISBNHead.cur_size = 0;
        ISBNHead.head = -1;
        file_.write(reinterpret_cast<char*>(&ISBNHead.new_id), sizeof(int));
        file_.write(reinterpret_cast<char*>(&ISBNHead.cur_size), sizeof(int));
        file_.write(reinterpret_cast<char*>(&ISBNHead.head), sizeof(int));
    } else {
        file_.read(reinterpret_cast<char*>(&ISBNHead.new_id), sizeof(int));
        file_.read(reinterpret_cast<char*>(&ISBNHead.cur_size), sizeof(int));
        file_.read(reinterpret_cast<char*>(&ISBNHead.head), sizeof(int));
        int p = ISBNHead.head;
        while (p != -1) {
            ISBNlink[p] = ISBNHead.ISBNvisitHead(p);
            p = ISBNlink[p].nex_head;
        }
    }
    file_.close();
}

void BookManager::ISBNflush() {
    fstream file_;
    file_.open("h9", std::ios::in | std::ios::out | std::ios::binary);
    file_.seekp(0);
    file_.write(reinterpret_cast<char*>(&ISBNHead.new_id), sizeof(int));
    file_.write(reinterpret_cast<char*>(&ISBNHead.cur_size), sizeof(int));
    file_.write(reinterpret_cast<char*>(&ISBNHead.head), sizeof(int));
    file_.close();
    int p = ISBNHead.head;
    while (p != -1) {
        ISBNHead.ISBNwriteHead(p, ISBNlink[p]);
        p = ISBNlink[p].nex_head;
    }
    delete[] ISBNbloc;
}

void BookManager::importBook(int quantity, double Totalcost) {
    string ISBN = userMgr.getLastLogbook();
    if ( quantity > 0 && Totalcost > 0 &&ISBN[0]!='\0') {
        Book book;
        strcpy(book.ISBN, ISBN.c_str());
        int p = ISBNHead.getHead();
        while (p != -1) {
            ISBNBody.ISBNvisitNode(ISBNlink[p].id);
            if (ISBNlink[p].size > 0 && compareISBN(ISBNbloc[0].ISBN ,book.ISBN)<=0 && compareISBN(ISBNbloc[ISBNlink[p].size - 1].ISBN , book.ISBN)>=0) {
                for (int i = 0; i < ISBNlink[p].size; ++i) {
                    if (strcmp(ISBNbloc[i].ISBN, ISBN.c_str()) == 0) {
                        ISBNbloc[i].stock+= quantity;
                        ISBNBody.ISBNwriteNode(ISBNlink[p].id);
                        financeMgr.recordPriceChange(-Totalcost);
                        diaryMgr.adddiary(userMgr.getLastLoguser().userID, "import book: " + std::string(book.title) + " " + std::to_string(quantity) + "0");
                    }
                }
            }
            if (compareISBN(ISBNbloc[0].ISBN ,book.ISBN)>0) {
                std::cout<<"Invalid\n";
                return;
            }
            p = ISBNlink[p].nex_head;
        }
    }else {
        std::cout<<"Invalid\n";
    }
}

void BookManager::SellBook(const string& ISBN, int quantity, double price) {
    Book book=FindByISBN(ISBN);
    if ( quantity > 0 && price > 0&&book.ISBN[0]!='\0') {
        strcpy(book.ISBN, ISBN.c_str());
        int p = ISBNHead.getHead();
        while (p != -1) {
            ISBNBody.ISBNvisitNode(ISBNlink[p].id);
            if (ISBNlink[p].size > 0 && compareISBN(ISBNbloc[0].ISBN ,book.ISBN)<=0 && compareISBN(ISBNbloc[ISBNlink[p].size - 1].ISBN , book.ISBN)>=0) {
                for (int i = 0; i < ISBNlink[p].size; ++i) {
                    if (strcmp(ISBNbloc[i].ISBN, ISBN.c_str()) == 0) {
                        if(ISBNbloc[i].stock<quantity) {
                            std::cout<<"Invalid\n";
                            return;
                        }
                        ISBNbloc[i].stock-= quantity;
                        ISBNBody.ISBNwriteNode(ISBNlink[p].id);
                        double totalprice = quantity * price;
                        std::cout << std::fixed << std::setprecision(2) << totalprice << std::endl;
                        financeMgr.recordPriceChange(totalprice);
                        diaryMgr.adddiary(userMgr.getLastLoguser().userID, "buy book: " + std::string(book.title) + " " + std::to_string(quantity) + "0 at");
                        return;
                    }
                }
            }
            if (compareISBN(ISBNbloc[0].ISBN ,book.ISBN)>0) {
                std::cout<<"Invalid\n";
                return;
            }
            p = ISBNlink[p].nex_head;
        }
    }else {
        std::cout<<"Invalid\n";
    }
}

void BookManager::show(const std::string& field, const std::string& value) {
    if (field != "-ISBN" && field != "-name" && field != "-author" && field != "-keyword" && field != "-price") {
        std::cout << "Invalid\n";
        return;
    }

    int p = ISBNHead.getHead();
    bool found = false;

    while (p != -1) {
        ISBNBody.ISBNvisitNode(ISBNlink[p].id);
        for (int i = 0; i < ISBNlink[p].size; ++i) {
            Book& book = ISBNbloc[i];

            if (field == "-keyword") {
                if (value.find('|') != std::string::npos) {
                    std::cout << "Invalid: Multiple keywords are not allowed.\n";
                    return;
                }

                std::vector<std::string> keywordsList;
                std::string keywordsStr(book.keywords);
                std::stringstream ss(keywordsStr);
                std::string keyword;
                while (std::getline(ss, keyword, '|')) {
                    keywordsList.push_back(keyword);
                }


                bool keywordFound = (std::find(keywordsList.begin(), keywordsList.end(), value) != keywordsList.end());
                if (!keywordFound) {
                    continue;
                }
            } else if ((field == "-ISBN" && strcmp(book.ISBN, value.c_str()) != 0) ||
                       (field == "-name" && strcmp(book.title, value.c_str()) != 0) ||
                       (field == "-author" && strcmp(book.author, value.c_str()) != 0) ||
                       (field == "-price" && std::to_string(book.price) != value)) {
                continue;
            }

            // 输出符合条件的书籍
            if (book.title[0] != '\0') {
                std::cout << book.ISBN << "  "
                          << book.title << "  "
                          << book.author << "  "
                          << book.keywords << "  "
                          << std::fixed << std::setprecision(2) << book.price << "  "
                          << book.stock << "\n";
                found = true;
            }
        }
        p = ISBNlink[p].nex_head;
    }
    if (!found) {
        std::cout << "\n";
    }
}
void BookManager::showall() {
    int p = ISBNHead.getHead();
    while (p != -1) {
        ISBNBody.ISBNvisitNode(ISBNlink[p].id);
        for (int i = 0; i < ISBNlink[p].size; ++i) {
            Book& book = ISBNbloc[i];
            std::cout  << book.ISBN << "  "
                                      << book.title << "  "
                                      << book.author << "  "
                                       << book.keywords << "  "
                                       << book.price << "  "
                                      << book.stock << "\n";
        }
        p = ISBNlink[p].nex_head;
    }
    std::cout << "\n";
}
