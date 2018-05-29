#include "Timer.h"

namespace cru
{
	TimerManager* TimerManager::instance = nullptr;

	TimerManager* TimerManager::GetInstance()
	{
		return instance;
	}

	TimerManager::TimerManager()
	{
		instance = this;
	}

	TimerManager::~TimerManager()
	{
		instance = nullptr;
	}

	UINT_PTR TimerManager::CreateTimer(UINT milliseconds, bool loop, const TimerAction & action)
	{
		auto id = ::SetTimer(nullptr, 0, milliseconds, nullptr);
		if (loop)
			map_[id] = action;
		else
			map_[id] = [this, action, id]() {
				action();
				this->KillTimer(id);
			};

		return id;
	}

	void TimerManager::KillTimer(UINT_PTR id)
	{
		auto find_result = map_.find(id);
		if (find_result != map_.cend())
		{
			::KillTimer(nullptr, id);
			map_.erase(find_result);
		}
	}

	std::optional<TimerAction> TimerManager::GetAction(UINT_PTR id)
	{
		auto find_result = map_.find(id);
		if (find_result == map_.cend())
			return std::nullopt;
		return find_result->second;
	}

	class TimerTask_Impl : public ITimerTask
	{
	public:
		TimerTask_Impl(UINT_PTR id);
		~TimerTask_Impl() override;

		void Cancel() override;

	private:
		UINT_PTR id_;
	};

	TimerTask_Impl::TimerTask_Impl(UINT_PTR id)
		: id_(id)
	{

	}

	TimerTask_Impl::~TimerTask_Impl()
	{

	}

	void TimerTask_Impl::Cancel()
	{
		TimerManager::GetInstance()->KillTimer(id_);
	}

	inline UINT SecondToMilliSecond(double seconds)
	{
		return static_cast<UINT>(seconds * 1000);
	}

	std::shared_ptr<ITimerTask> SetTimeout(double seconds, const TimerAction & action)
	{
		auto id = TimerManager::GetInstance()->CreateTimer(SecondToMilliSecond(seconds), false, action);
		return std::make_shared<TimerTask_Impl>(id);
	}

	std::shared_ptr<ITimerTask> SetInterval(double seconds, const TimerAction & action)
	{
		auto id = TimerManager::GetInstance()->CreateTimer(SecondToMilliSecond(seconds), true, action);
		return std::make_shared<TimerTask_Impl>(id);
	}
}
