//
// Created by frane on 6/9/2026.
//

#ifndef PROG4MINIGINPROJECT_LEVELLOADER_H
#define PROG4MINIGINPROJECT_LEVELLOADER_H


class LevelLoader {
public:
    static LevelData LoadFromFile(const std::string& file);
    static LevelData LoadFromBytes(const std::vector<uint8_t>& bytes);
};


#endif //PROG4MINIGINPROJECT_LEVELLOADER_H