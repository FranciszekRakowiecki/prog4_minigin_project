//
// Created by frane on 5/6/2026.
//

#ifndef PROG4MINIGINPROJECT_ENGINEHOOK_H
#define PROG4MINIGINPROJECT_ENGINEHOOK_H


namespace dae {
    class EngineHook {
    public:
        virtual void Start() = 0;
        virtual void Update() = 0;
        virtual void Render() = 0;
    };
}


#endif //PROG4MINIGINPROJECT_ENGINEHOOK_H