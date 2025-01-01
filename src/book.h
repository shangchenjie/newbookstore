#ifndef BOOK_H
#define BOOK_H

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <string>
#include <algorithm>
#include <vector>
#include <sstream>
using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;



struct Book {
    char ISBN[21];
    char title[61];
    char author[61];
    char keywords[61];
    double price;
    int stock;

    bool operator<(const Book& other) const {
        return std::string(ISBN) < std::string(other.ISBN); // 按 ISBN 排序
    }
};
inline int compareISBN(const char* thisISBN,const char* otherISBN) {
    return strcmp(thisISBN, otherISBN);
}
struct ISBNHeadNode {
    int id;
    int prev_head;
    int nex_head;
    int size;
    char minISBN[20];
};
const int block_size = 128;
const int sizeofP = sizeof(Book);
const int sizeofH = sizeof(ISBNHeadNode);
extern Book* ISBNbloc;
extern ISBNHeadNode ISBNlink[];

class ISBNNodeHead {
    fstream file;
    string file_name;

public:
    int new_id = 0;
    int cur_size = 0;
    int head = -1;

    ISBNNodeHead() = default;
    ISBNNodeHead(const string& file_name);
    ~ISBNNodeHead();

    int getSize();
    int getHead();
    ISBNHeadNode ISBNvisitHead(int index);
    void ISBNwriteHead(int index, ISBNHeadNode ret);
    int ISBNaddHead(int index);
    void ISBNdeleteHead(int index);
};

class ISBNNodeBody {
    fstream file;
    string file_name;

public:
    ISBNNodeBody() = default;
    ISBNNodeBody(const string& file_name);
    ISBNNodeBody(const ISBNNodeBody& other);
    ISBNNodeBody& operator=(const ISBNNodeBody& other);
    ~ISBNNodeBody();

    void ISBNvisitNode(int index);
    void ISBNwriteNode(int index);
};

class BookManager {
private:

    ISBNNodeHead ISBNHead;
    ISBNNodeBody ISBNBody;

    void ISBNaddNode(int index, const Book& book);
    void ISBNdeleteNode(int index, const Book& book);

public:
    BookManager() : ISBNHead("ISBNhead_"), ISBNBody("ISBNbody_") {
        ISBNinitialise();
    };
    ~BookManager();

    void Insert(const Book& book);
    void Delete(const string& ISBN);
    Book FindByISBN(const string& ISBN);
    Book FindByTitle(const string& title);
    Book FindByAuthor(const string& author);
    void ISBNinitialise();
    void ISBNflush();
    void importBook( int quantity, double Totalcost);
    void SellBook(const string& ISBN, int quantity, double price);
    Book FindBookByExactKeywords(const std::string& keywords);
    void show(const std::string& field,const std::string& value);
    void showall();
};
inline std::vector<std::string> splitKeywords(const std::string& keywords, char delimiter = ' ') {
    std::vector<std::string> words;
    std::istringstream iss(keywords);
    std::string word;
    while (std::getline(iss, word, delimiter)) {
        if (!word.empty()) {
            words.push_back(word);
        }
    }
    return words;
}

inline bool areKeywordsEqual(const std::string& keyword1, const std::string& keyword2) {
    std::vector<std::string> keywords1 = splitKeywords(keyword1);
    std::vector<std::string> keywords2 = splitKeywords(keyword2);

    if (keywords1.size() != keywords2.size()) {
        return false;
    }
    std::sort(keywords1.begin(), keywords1.end());
    std::sort(keywords2.begin(), keywords2.end());

    return keywords1 == keywords2;
}

#endif // BOOK_Hj