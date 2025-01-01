//
// Created by 27861 on 2024/12/24.
//

#ifndef MEMORYRIVER_H
#define MEMORYRIVER_H
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

const int block_size = 128;
const int sizeofP = 16;
const int sizeofH = 16;
const long long mod = 9999999967;

struct Pair {
    long long key;
    long long value;
};

Pair* bloc = new Pair[block_size];

struct HeadNode {
    int id;
    int prev_head;
    int nex_head;
    int size;
    int cur_index;
} link[100001];

bool comp(Pair x, Pair y) {
    if (x.key != y.key) {
        return x.key <= y.key;
    }
    return x.value <= y.value;
}
bool comp1(Pair x, Pair y) {
    if (x.key != y.key) {
        return x.key >= y.key;
    }
    return x.value >= y.value;
}
bool equal(Pair x, Pair y) {
    if (x.value == y.value && x.key == y.key) {
        return true;
    }
    return false;
}

class NodeHead {
    fstream file;
    std::string file_name;
public:

    int new_id = -1;
    int cur_size = 0;
    int head = -1;

    NodeHead() = default;

    NodeHead(const std::string &file_name) {
        this->file_name = file_name;
        file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            file.open(file_name, std::ios::out);
            file.close();
            file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
        }
    }

    ~NodeHead() {
        file.close();
    }

    int getSize() {
        return cur_size;
    }

    int getHead() {
        return head;
    }

    HeadNode visitHead(int index) {
        HeadNode ret;
        file.seekg(index * sizeofH);
        file.read(reinterpret_cast<char*>(&ret), sizeofH );
        return ret;
    }

    void writeHead(int index, HeadNode ret) {
        file.seekp(index * sizeofH );
        file.write(reinterpret_cast<char*>(&ret), sizeofH );
    }

    int addHead(int index) {
        HeadNode new_head;
        new_head.id = new_id;
        new_head.prev_head = (index == -1) ? -1 : index;
        new_head.nex_head = (index == -1) ? -1 : link[index].nex_head;
        new_head.size = 0;
        new_head.cur_index = new_id;

        if (head == -1) {
            head = 0;
        }

        link[new_id] = new_head;
        if (index != -1) {
            link[index].nex_head = new_id;
            if (new_head.nex_head != -1) {
                link[new_head.nex_head].prev_head = new_id;
            }
        }
        writeHead(new_id, new_head);
        new_id++;
        cur_size++;
        return new_id - 1;
    }

    void deleteHead(int index) {
        HeadNode current = link[index];
        if (current.prev_head != -1) {
            link[current.prev_head].nex_head = current.nex_head;
        }
        if (current.nex_head != -1) {
            link[current.nex_head].prev_head = current.prev_head;
        }
        cur_size--;
    }
};

class NodeBody {
    fstream file;
    std::string file_name;
public:

    NodeBody() = default;

    NodeBody(const std::string &file_name) {
        this->file_name = file_name;
        file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            file.open(file_name, std::ios::out);
            file.close();
            file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
        }
    }

    ~NodeBody() {
        file.close();
    }

    void visitNode(int index) {
        file.seekg(index * block_size * sizeofP);
        file.read(reinterpret_cast<char*>(bloc), link[index].size * sizeofP);
    }

    void writeNode(int index) {
        file.seekp(index * block_size * sizeofP);
        file.write(reinterpret_cast<char*>(bloc), link[index].size * sizeofP);
    }
};

void Insert(long long, int);
void Delete(long long, int);
void Find(long long);

NodeHead Head = NodeHead("head_");
NodeBody Body = NodeBody("body_");

void initialise() {
    fstream file_;
    file_.open("h9", std::ios::in | std::ios::out);
    if (!file_.is_open()) {
        file_.open("h9", std::ios::out);
        file_.close();
        file_.open("h9", std::ios::in | std::ios::out | std::ios::binary);
        Head.new_id = 0;
        Head.cur_size = 0;
        Head.head = -1;
        file_.write(reinterpret_cast<char*>(&Head.new_id), sizeof(int));
        file_.write(reinterpret_cast<char*>(&Head.cur_size), sizeof(int));
        file_.write(reinterpret_cast<char*>(&Head.head), sizeof(int));
   } else {
        file_.read(reinterpret_cast<char*>(&Head.new_id), sizeof(int));
        file_.read(reinterpret_cast<char*>(&Head.cur_size), sizeof(int));
        file_.read(reinterpret_cast<char*>(&Head.head), sizeof(int));
        int p = Head.head;
        while (p != -1) {
            link[p] = Head.visitHead(p);
            p = link[p].nex_head;
        }
    }
    file_.close();
}

void flush() {
    fstream file_;
    file_.open("h9", std::ios::in | std::ios::out | std::ios::binary);
    file_.seekp(0);
    file_.write(reinterpret_cast<char*>(&Head.new_id), sizeof(int));
    file_.write(reinterpret_cast<char*>(&Head.cur_size), sizeof(int));
    file_.write(reinterpret_cast<char*>(&Head.head), sizeof(int));
    file_.close();
    int p = Head.head;
    while (p != -1) {
        Head.writeHead(p, link[p]);
        p = link[p].nex_head;
    }

    delete[] bloc;
}

int main() {
    //freopen("in", "r", stdin);
    //freopen("1_.out", "w", stdout);
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    initialise();
    int n;
    std::cin >> n;
    for (int i = 0; i < n; ++i) {
        std::string s, index_;
        int value;
        std::cin >> s >> index_;
        long long index = std::hash<std::string>{}(index_) % mod;
        switch (s[0]) {
            case 'i': {
                std::cin >> value;
                Insert(index, value);
                break;
            }
            case 'd': {
                std::cin >> value;
                Delete(index, value);
                break;
            }
            case 'f': {
                Find(index);
                break;
            }
        }
    }
    flush();
}

void addNode(int index, Pair data) {
    Body.visitNode(link[index].id);
    int left = 0, right = link[index].size - 1;
    int insertPos = link[index].size;
    while (left <= right) {
        int mid = (left + right) / 2;
        if (comp(bloc[mid], data)) {
            left = mid + 1;
        } else {
            right = mid - 1;
            insertPos = mid;
        }
    }
    for (int i = link[index].size; i > insertPos; --i) {
        bloc[i] = bloc[i - 1];
    }
    bloc[insertPos] = data;
    link[index].size++;

    Body.writeNode(link[index].id);
}

void deleteNode(int index, Pair data) {
    Body.visitNode(link[index].id);
    int left = 0, right = link[index].size - 1;
    int deletePos = -1;
    while (left <= right) {
        int mid = (left + right) / 2;
        if (equal(bloc[mid], data)) {
            deletePos = mid;
            break;
        } else if (comp(bloc[mid], data)) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    if (deletePos != -1) {
        for (int i = deletePos; i < link[index].size - 1; ++i) {
            bloc[i] = bloc[i + 1];
        }
        link[index].size--;

        Body.writeNode(link[index].id);
    }
}

void Insert(long long index, int value) {
    Pair data;
    data.key = index;
    data.value = value;

    int p = Head.head;
    int final;
    while (p != -1) {
        Body.visitNode(link[p].id);
        if (link[p].size < block_size && comp(bloc[0],data) && comp1(bloc[link[p].size - 1],data) ) {
            addNode(p, data);
            if (link[p].size == block_size) {
                int new_block_index = Head.addHead(link[p].id);
                link[p].size = block_size / 2;
                Body.writeNode(link[p].id);

                for (int i = block_size / 2; i < block_size; ++i) {
                    bloc[i - block_size / 2] = bloc[i];
                }
                link[new_block_index].size = block_size / 2;
                Body.writeNode(new_block_index);
            }
            return;
        }
        if(comp1(bloc[0],data)) {
            p = link[p].prev_head;
            if(p==-1) {
                p=0;
                addNode(p, data);
            }
            else{addNode(p, data);}
            if (link[p].size == block_size) {
                int new_block_index = Head.addHead(link[p].id);
                link[p].size = block_size / 2;
                Body.writeNode(link[p].id);

                for (int i = block_size / 2; i < block_size; ++i) {
                    bloc[i - block_size / 2] = bloc[i];
                }
                link[new_block_index].size = block_size / 2;
                Body.writeNode(new_block_index);
            }
            return;
        }
        final=p;
        p = link[p].nex_head;
    }
    if(Head.cur_size!=0) {
        p = final;
        addNode(p, data);
        if (link[p].size == block_size) {
            int new_block_index = Head.addHead(link[p].id);
            link[p].size = block_size / 2;
            Body.writeNode(link[p].id);

            for (int i = block_size / 2; i < block_size; ++i) {
                bloc[i - block_size / 2] = bloc[i];
            }
            link[new_block_index].size = block_size / 2;
            Body.writeNode(new_block_index);
        }
    }else {
        int new_block_index = Head.addHead(-1);
        addNode(new_block_index, data);
    }
}

void Delete(long long index, int value) {
    Pair data;
    data.key = index;
    data.value = value;

    int p = Head.head;
    while (p != -1) {
        Body.visitNode(link[p].id);
        if (link[p].size > 0 && comp(bloc[0],data) && comp1(bloc[link[p].size - 1],data)) {
            deleteNode(p, data);
            if (link[p].size == 0) {
                Head.deleteHead(p);
            }
            return;
        }
        if(!comp(bloc[0],data)) {
            return;
        }
        p = link[p].nex_head;
    }
}

void Find(long long index) {
    int p = Head.head;
    bool find=false;
    while (p != -1) {
        Body.visitNode(link[p].id);
        if (link[p].size > 0 && bloc[0].key <= index && bloc[link[p].size - 1].key >= index) {
            for (int i = 0; i < link[p].size; ++i) {
                if (bloc[i].key == index) {
                    find=true;
                    std::cout<<bloc[i].value<<" ";
                }
            }
        }
        if(bloc[0].key > index) {
            break;
        }
        p = link[p].nex_head;
    }
    if (!find) {
        std::cout << "null";
    }
    std::cout << "\n";
}
#endif //MEMORYRIVER_H
