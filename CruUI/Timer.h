#pragma once

#include "Base.h"
#include <functional>
#include <memory>
#include <map>
#include <optional>

namespace cru
{
	using TimerAction = std::function<void()>;

	class TimerManager : public Object
	{
	private:
		static TimerManager* instance;

	public:
		static TimerManager* GetInstance();

	public:
		TimerManager();
		~TimerManager() override;

		UINT_PTR CreateTimer(UINT milliseconds, bool loop, const TimerAction& action);
		void KillTimer(UINT_PTR id);
		std::optional<TimerAction> GetAction(UINT_PTR id);

	private:
		std::map<UINT_PTR, TimerAction> map_{};
	};

	struct ITimerTask : virtual Interface
	{
		virtual void Cancel() = 0;
	};

	std::shared_ptr<ITimerTask> SetTimeout(double seconds, const TimerAction& action);
	std::shared_ptr<ITimerTask> SetInterval(double seconds, const TimerAction& action);
}
