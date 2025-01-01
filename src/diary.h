#ifndef DIARY_H
#define DIARY_H

#include <string>
#include <vector>
#include <fstream>

struct diarymessage {
    char adminID[20];
    char action[100];
};

class diaryManager {
private:
    std::vector<diarymessage> diarys;
    std::string diaryFileName;

public:
    diaryManager(const std::string& fileName);
    void adddiary(const std::string& adminID, const std::string& action);
    void printAlldiarys() const;
    void printAdmindiarys(const std::string& adminID) const;
    void savediarysToFile() const;
    void loaddiarysFromFile();
    void printsell_books() const;
    void printimport_books() const;
};

extern diaryManager diaryMgr;

#endif // DIARY_H