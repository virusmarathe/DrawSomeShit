#include "EventManager.h"

// ==================
// EVENT
// ==================

Event::~Event()
{
	for (it = m_FunctionList.begin(); it != m_FunctionList.end(); ++it)
	{
		delete *it;
	}
}

void Event::AddListener(void(*func)(EventParam*))
{
	Delegate<void, EventParam*> * del = new Delegate<void, EventParam*>(func);
	m_FunctionList.push_back(del);
	m_funcToDelMap[func] = del;
}

void Event::AddListener(IEventListener * listener)
{
	Delegate<void, EventParam*> * del = new Delegate<void, EventParam*>(listener, &IEventListener::HandleEvent);
	m_FunctionList.push_back(del);
	m_memberFuncToDelMap[listener] = del;
}

void Event::RemoveListener(void(*func)(EventParam*))
{
	Delegate<void, EventParam*> * del = m_funcToDelMap[func];
	m_FunctionList.remove(del);
	delete del;
}

void Event::RemoveListener(IEventListener * listener)
{
	Delegate<void, EventParam*> * del = m_memberFuncToDelMap[listener];
	m_FunctionList.remove(del);
	delete del;
}

void Event::TriggerEvent(EventParam * param)
{
	for (it = m_FunctionList.begin(); it != m_FunctionList.end(); ++it)
	{
		(**it)(param);
	}
}

// ==================
// EVENT MANAGER
// ==================

EventManager * EventManager::s_Instance = NULL;

EventManager::EventManager()
{
	if (s_Instance == NULL)
	{
		s_Instance = this;
	}
}

EventManager::~EventManager()
{
	for (std::map<EventType, Event*>::iterator it = m_EventMap.begin(); it != m_EventMap.end(); ++it)
	{
		if (it->second != NULL)
		{
			delete it->second;
		}
	}
}

void EventManager::AddListener(EventType eventType, void(*func)(EventParam *))
{
	Event * curEvent;
	if (m_EventMap[eventType] == NULL)
	{
		curEvent = new Event();
		m_EventMap[eventType] = curEvent;
	}

	m_EventMap[eventType]->AddListener(func);
}

void EventManager::AddListener(EventType eventType, IEventListener * listener)
{
	Event * curEvent;
	if (m_EventMap[eventType] == NULL)
	{
		curEvent = new Event();
		m_EventMap[eventType] = curEvent;
	}

	m_EventMap[eventType]->AddListener(listener);

}

void EventManager::RemoveListener(EventType eventType, void(*func)(EventParam *))
{
	Event * curEvent = m_EventMap[eventType];

	if (curEvent != NULL)
	{
		curEvent->RemoveListener(func);
	}
}

void EventManager::RemoveListener(EventType eventType, IEventListener * listener)
{
	Event * curEvent = m_EventMap[eventType];

	if (curEvent != NULL)
	{
		curEvent->RemoveListener(listener);
	}
}

void EventManager::TriggerEvent(EventType eventType, EventParam * param)
{
	Event * curEvent = m_EventMap[eventType];
	if (curEvent != NULL)
	{
		param->m_ID = eventType;
		curEvent->TriggerEvent(param);
	}
}
