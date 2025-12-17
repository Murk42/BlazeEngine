#pragma once

template<auto MemberFunction>
class EventMemberFunctionTie
{
public:
	using Arguments = MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunction)>;
	using EventType = RemoveCVRef<typename Arguments::template TypeAtIndex<0>>;
	using ClassType = MemberFunctionPointerTypeClassType<decltype(MemberFunction)>;

	EventMemberFunctionTie(EventDispatcher<EventType>& dispatcher, ClassType& object)
		: dispatcher(dispatcher), object(object)
	{
		dispatcher.AddHandler<MemberFunction>(object);
	}
	~EventMemberFunctionTie()
	{
		dispatcher.RemoveHandler<MemberFunction>(object);
	}
private:
	EventDispatcher<EventType>& dispatcher;
	ClassType& object;
};
#define EVENT_MEMBER_FUNCTION(className, name, dispatcher) EventMemberFunctionTie<&className::name> name##Handler{ dispatcher, *this };
