#pragma once

#include <list>
#include <map>
#include "Delegate.h"


enum EventType
{
	TEST_EVENT
};

struct EventParam
{
	EventType m_ID;
	void * m_Data;

	EventParam(EventType id) : m_ID(id) {}
	EventParam() {}
	EventParam(void * data) : m_Data(data) {}
};

class IEventListener
{
public:
	virtual void HandleEvent(EventParam * param) = 0;
};

class Event
{
public:

	~Event();
	void AddListener(void(*func)(EventParam*));
	void AddListener(IEventListener * listener);
	void RemoveListener(void(*func)(EventParam*));
	void RemoveListener(IEventListener * listener);
	void TriggerEvent(EventParam * param);

private:
	std::list<Delegate<void, EventParam*>*> m_FunctionList;
	std::list<Delegate<void, EventParam*>*>::iterator it;
	std::map<void(*)(EventParam*), Delegate<void, EventParam*>*> m_funcToDelMap;
	std::map<IEventListener *, Delegate<void, EventParam*>*> m_memberFuncToDelMap;
};

class EventManager
{
public:
	EventManager();
	~EventManager();
	void AddListener(EventType eventType, void(*func)(EventParam*));
	void AddListener(EventType eventType, IEventListener * listener);
	void RemoveListener(EventType eventType, void(*func)(EventParam*));
	void RemoveListener(EventType eventType, IEventListener * listener);
	void TriggerEvent(EventType eventType, EventParam * param);

	static EventManager * Instance() { return s_Instance; }

private:
	std::map<EventType, Event*> m_EventMap;
	static EventManager * s_Instance;
};

