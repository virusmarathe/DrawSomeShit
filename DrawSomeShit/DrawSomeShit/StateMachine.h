#pragma once
#include "State.h"
#include <stack>

template <class T>
class StateMachine
{
public:
	StateMachine(T * owner) : m_Owner(owner), m_CurrentState(NULL), m_GlobalState(NULL) { }
	~StateMachine() { }

	void SetCurrentState(State<T> * state) { m_CurrentState = state; }
	void SetGlobalState(State<T> * state) { m_GlobalState = state; }
	State<T>* GetCurrentState() { return m_CurrentState; }
	State<T>* SetGlobalState() { return m_GlobalState; }

	void Update(float deltaTime) const
	{
		if (m_GlobalState)
		{
			m_GlobalState->Update(m_Owner, deltaTime);
		}
		if (m_CurrentState)
		{
			m_CurrentState->Update(m_Owner, deltaTime);
		}
	}

	// use change state when no history needs to be saved
	void ChangeState(State<T> * state)
	{
		if (m_CurrentState != NULL)
		{
			m_CurrentState->Exit(m_Owner);
		}
		m_CurrentState = state;
		m_CurrentState->Enter(m_Owner);
	}

	// use push and pop states if you want to maintain states in a stack
	void PushState(State<T> * state)
	{
		ChangeState(state);
		m_StateStack.push(m_CurrentState);
	}

	State<T>* PopState()
	{
		State<T>* prevState = m_StateStack.pop();
		ChangeState(m_StateStack.top());

		return prevState;
	}


private:
	T * m_Owner;
	State<T>* m_CurrentState;
	State<T>* m_GlobalState;
	std::stack<State<T>*> m_StateStack;
};

