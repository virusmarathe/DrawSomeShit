#pragma once
template <class T>
class State
{
public:

	State(bool isServer) { mIsServer = isServer; }

	virtual void Enter(T*) = 0;
	virtual void Update(T*, float deltaTime) = 0;
	virtual void Exit(T*) = 0;

	virtual ~State() {}

protected:
	bool mIsServer;
};

