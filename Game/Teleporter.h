//
// Created by frane on 6/10/2026.
//

#ifndef PROG4MINIGINPROJECT_TELEPORTER_H
#define PROG4MINIGINPROJECT_TELEPORTER_H
#include "Component.h"


struct LevelData;

class Teleporter : public dae::Component {
public:
    int GetFlags() override;

    void Update() override;

    void Render() override;

    void SetLevelData(const LevelData* data);
    void SetPlayers(const std::vector<dae::GameObject*>& players);

private:
    const LevelData* m_LevelData{};
    std::vector<dae::GameObject*> m_Players{};
};


#endif //PROG4MINIGINPROJECT_TELEPORTER_H