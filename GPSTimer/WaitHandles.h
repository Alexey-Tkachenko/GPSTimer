#ifndef _WAIT_HANDLES_
#define _WAIT_HANDLES_

#include "Arduino.h"
#include "WaitObject.h"
#include "ConditionWrapper.h"
#include "PlacementNew.h"

#define DELETE_COPY(T) T(const T&) = delete; T& operator=(const T&) = delete;

namespace WaitHandles
{

	class Sleep : public WaitObject
	{
	public:
		Sleep();
		Sleep(unsigned interval);
		DELETE_COPY(Sleep);
		void Start(unsigned interval);
		virtual bool IsSignalled() override;
	private:
		unsigned long expired;
	};

	class Event : public WaitObject
	{
	public:
		Event();
		Event(bool active);
		DELETE_COPY(Event);

		virtual bool IsSignalled() override;

		void Set();
		void Reset();

	private:
		volatile bool active;
	};


	class AutoResetEvent : public Event
	{
	public:
		AutoResetEvent();
		AutoResetEvent(bool active);
		DELETE_COPY(AutoResetEvent);
		virtual bool IsSignalled() override;
	};



	class Condition : public WaitObject
	{
	public:
		Condition();

		template<class T>
		Condition(bool(*arg)(T*), T* data)
		{
			SetCondition(arg, data);
		}

		template<class T>
		Condition(T* target, bool(T::*method)())
		{
			SetCondition(target, method);
		}

		Condition(bool(*arg)());

		Condition(WaitCondition* condition);

		void SetCondition(WaitCondition* condition);

		void SetCondition(bool(*arg)());

		template<class T>
		void SetCondition(bool(*arg)(T*), T* data)
		{
			condition = new(ExtWaitConditionT)TFuncWaitCondition<T>(arg, data);
		}

		template<class T>
		void SetCondition(T* target, bool(T::*method)())
		{
			condition = new (ExtMemberCondition)MemberFuncWaitCondition<T>(target, method);
		}

		virtual bool IsSignalled() override;

	private:
		WaitCondition* condition;

		struct Dummy{};

		union
		{
			byte ExtWaitCondition[sizeof(FuncWaitCondition)];
			byte ExtWaitConditionT[sizeof(TFuncWaitCondition<void*>)];
			byte ExtMemberCondition[sizeof(MemberFuncWaitCondition<Dummy>)];
		};
	};

	template<typename T>
	class ValueHolder : public WaitObject
	{
	public:
		ValueHolder() : value(), signalled(false) {}
		ValueHolder(const T& initial) : value(initial), signalled(false) 
		{}
		
		inline const T& Get()
		{
			signalled = false;
			return this->value;
		}

		inline void Set(const T& value)
		{
			this->value = value;
			signalled = true;
		}

		virtual bool IsSignalled() override
		{
			return signalled;
		}

	private:
		T value;
		volatile bool signalled;
	};
}


#undef DELETE_COPY
#endif