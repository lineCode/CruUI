#pragma once

#include "Base.h"
#include <type_traits>
#include <list>
#include <memory>
#include <functional>
#include <algorithm>

namespace cru {
	//Base class of all event args.
	class BasicEventArgs : public Object
	{
	public:
		BasicEventArgs(Object* sender);
		~BasicEventArgs() override;

		//Get the sender of the event.
		Object* GetSender();

	private:
		Object* sender_;
	};


	//A non-copyable non-movable Event class.
	//It stores a list of event handlers.
	//ArgsType_ must be subclass of BasicEventArgs.
    template<typename ArgsType_>
    class Event
	{
    public:
		static_assert(std::is_base_of_v<BasicEventArgs, ArgsType_>,
			"ArgsType_ must be subclass of BasicEventArgs.");


        using ArgsType = ArgsType_;
        using EventHandler = std::function<void(ArgsType&)>;
        using EventHandlerPtr = std::shared_ptr<EventHandler>;

		Event()
		{

		}

		~Event()
		{

		}

        CRU_NO_COPY_MOVE(Event)

    public:

		EventHandlerPtr AddHandler(const EventHandler& handler)
		{
			EventHandlerPtr ptr = std::make_shared<EventHandler>(handler);
			handlers_.push_back(ptr);
			return ptr;
		}

		void AddHandler(EventHandlerPtr handler) {
			handlers_.push_back(handler);
		}

		void RemoveHandler(EventHandlerPtr handler) {
			auto find_result = std::find(handlers_.cbegin(), handlers_.cend(), handler);
			if (find_result != handlers_.cend())
				handlers_.erase(find_result);
		}

		void Raise(ArgsType& args) {
			for (auto ptr : handlers_)
				(*ptr)(args);
		}

    private:
        std::list<EventHandlerPtr> handlers_;
    };
}
