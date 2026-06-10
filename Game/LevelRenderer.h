//
// Created by frane on 6/9/2026.
//

#ifndef PROG4MINIGINPROJECT_LEVELRENDERER_H
#define PROG4MINIGINPROJECT_LEVELRENDERER_H
#include "Component.h"
#include "LevelLoader.h"


class LevelRenderer : public dae::Component {
public:
    int GetFlags() override;

    void Render() override;

    void SetLevelData(const LevelData* levelData);

    void Start() override;

    float GetResolution() const;

private:
    const LevelData* m_LevelData{nullptr};
    float m_TileSize{16.0f};
};


#endif //PROG4MINIGINPROJECT_LEVELRENDERER_H