//
// Created by frane on 5/6/2026.
//

#ifndef PROG4MINIGINPROJECT_GAMESTATE_H
#define PROG4MINIGINPROJECT_GAMESTATE_H


class GameState {
public:
    virtual ~GameState() = default;

    virtual void Enter() {}
    virtual void Exit() {}

    virtual void Update() = 0;
    virtual void Render() = 0;
};

class NullGameState : public GameState {
public:
    void Update() override {}

    void Render() override {}
};

// Temporary class locations will be moved to separate files

#endif //PROG4MINIGINPROJECT_GAMESTATE_H