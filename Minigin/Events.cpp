//
// Created by frane on 3/18/2026.
//

#include "Events.h"

Event::Event(const std::string &name) : name{name} {

}

void Subject::addObserver(IObserver *obs) {
    m_Observers.emplace_back(obs);
}

void Subject::removeObserver(IObserver *obs) {
    auto it = std::find(m_Observers.begin(), m_Observers.end(), obs);
    m_Observers.erase(it);
}

void Subject::callEvent(const Event *event, void *data) {
    for (IObserver *obs : m_Observers) {
        obs->notify(this, event, data);
    }
}
