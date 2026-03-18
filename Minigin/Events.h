//
// Created by frane on 3/18/2026.
//

#ifndef MINIGIN_EVENTS_H
#define MINIGIN_EVENTS_H
#include <string>
#include <vector>

#include "Singleton.h"

class Subject;
class IObserver;

class Event {
public:
    const std::string name;
    explicit Event(const std::string& name);
};

class Subject {
    std::vector<IObserver*> m_Observers;
public:
    void addObserver(IObserver* obs);
    // Don't remove observers during the event itself.
    void removeObserver(IObserver* obs);

    void callEvent(const Event* event, void* data);
};

class IObserver {
public:
    virtual ~IObserver() = default;
    // Data can be null
    virtual void notify(Subject* subject, const Event* event, void* data) = 0;
};

class GameEvents final : dae::Singleton<GameEvents> {
public:
    // No argument object
    const Event exampleEvent{Event("ExampleEvent")};
};

#endif //MINIGIN_EVENTS_H