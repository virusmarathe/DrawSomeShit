#pragma once
template <class T>
class State
{
public:
	virtual void Enter(T*) = 0;
	virtual void Update(T*, float deltaTime) = 0;
	virtual void Exit(T*) = 0;

	virtual ~State() {}
};

