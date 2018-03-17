#pragma once

#include "Base.h"
#include <list>
#include <memory>
#include <functional>
#include <algorithm>

namespace cru {
    template<typename ArgsType_>
    class Event {
    public:
        using ArgsType = ArgsType_;
        using EventHandler = std::function<void(ArgsType&)>;
        using EventHandlerPtr = std::shared_ptr<EventHandler>;

        Event();
        ~Event();
        NO_COPY_MOVE(Event)

    public:
        EventHandlerPtr AddHandler(const EventHandler& handler);
        void AddHandler(EventHandlerPtr handler);
        void RemoveHandler(EventHandlerPtr handler);

        void Raise(ArgsType& args);

    private:
        std::list<EventHandlerPtr> handlers;
    };

    template<typename ArgsType_>
    Event<ArgsType_>::Event() {

    }

    template<typename ArgsType_>
    inline Event<ArgsType_>::~Event() {

    }

    template<typename ArgsType_>
    Event<ArgsType_>::EventHandlerPtr Event<ArgsType_>::AddHandler(const EventHandler& handler) {
        auto ptr = std::make_shared<EventHandler>(handler);
        handlers.push_back(ptr);
        return ptr;
    }

	template<typename ArgsType_>
	void Event<ArgsType_>::AddHandler(EventHandlerPtr handler) {
        handlers.push_back(handler);
	}

    template<typename ArgsType_>
	void Event<ArgsType_>::RemoveHandler(EventHandlerPtr handler) {
        auto find_result = std::find(handlers.cbegin(), handlers.cend(), handler);
        if (find_result != handlers.cend())
            handlers.erase(find_result);
	}

	template<typename ArgsType_>
	inline void Event<ArgsType_>::Raise(ArgsType & args) {
        for (auto ptr : handlers)
            (*ptr)(args);
	}

    class BasicEventArgs : public Object {
    public:
        BasicEventArgs(Object* sender);
        ~BasicEventArgs() override;

        //Get the original sender of the event.
        Object* GetSender();

    private:
        Object * sender_;
    };
}
