#include "../src/diary.h"
#include <iostream>
#include <cstring>


diaryManager diaryMgr("diary.bin");


diaryManager::diaryManager(const std::string& fileName) : diaryFileName(fileName) {
    loaddiarysFromFile();
}

void diaryManager::adddiary(const std::string& adminID, const std::string& action) {
    diarymessage newDiary;
    strncpy(newDiary.adminID, adminID.c_str(), sizeof(newDiary.adminID) - 1);
    newDiary.adminID[sizeof(newDiary.adminID) - 1] = '\0'; // 确保字符串以 null 结尾
    strncpy(newDiary.action, action.c_str(), sizeof(newDiary.action) - 1);
    newDiary.action[sizeof(newDiary.action) - 1] = '\0'; // 确保字符串以 null 结尾
    diarys.push_back(newDiary);
    savediarysToFile();
}

void diaryManager::printAlldiarys() const {
    for (const auto& diary : diarys) {
        std::cout << "Admin ID: " << diary.adminID << ", Action: " << diary.action << std::endl;
    }
}

void diaryManager::printAdmindiarys(const std::string& adminID) const {
    for (const auto& diary : diarys) {
        if (strcmp(diary.adminID, adminID.c_str()) == 0) {
            std::cout << "Admin ID: " << diary.adminID << ", Action: " << diary.action << std::endl;
        }
    }
}
void diaryManager::savediarysToFile() const {
    std::ofstream file(diaryFileName, std::ios::binary);
    if (!file.is_open()) {
        return;
    }

    for (const auto& diary : diarys) {
        file.write(reinterpret_cast<const char*>(&diary), sizeof(diarymessage));
    }

    file.close();
}


void diaryManager::loaddiarysFromFile() {
    std::ifstream file(diaryFileName, std::ios::binary);

    if (!file.is_open()) {
        std::ofstream newFile(diaryFileName, std::ios::binary);
        if (!newFile.is_open()) {
            return;
        }
        newFile.close();
        return;
    }

    diarys.clear();
    diarymessage diary;
    while (file.read(reinterpret_cast<char*>(&diary), sizeof(diarymessage))) {
        diarys.push_back(diary);
    }

    file.close();
}


void diaryManager::printsell_books() const {
    for (const auto& diary : diarys) {
        if (strstr(diary.action, "sell") != nullptr) {
            std::cout << "Admin ID: " << diary.adminID << ", Action: " << diary.action << std::endl;
        }
    }
}

void diaryManager::printimport_books() const {
    for (const auto& diary : diarys) {
        if (strstr(diary.action, "import") != nullptr) {
            std::cout << "Admin ID: " << diary.adminID << ", Action: " << diary.action << std::endl;
        }
    }
}