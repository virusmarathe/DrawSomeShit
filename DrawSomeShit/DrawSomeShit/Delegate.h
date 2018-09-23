#pragma once

template<typename Ret, typename Param0>
class Callback
{
public:
	virtual Ret invoke(Param0 param0) = 0;
};

template<typename Ret, typename Param0>
class StaticFunctionCallback : public Callback<Ret, Param0>
{
public:
	StaticFunctionCallback(Ret (*func)(Param0)) : m_func_(func) {}

	virtual Ret invoke(Param0 param0)
	{
		return (*m_func_)(param0);
	}
private:
	Ret(*m_func_)(Param0);
};

template<typename Ret, typename T, typename Param0>
class MemberCallback : public Callback<Ret, Param0>
{
public:
	MemberCallback(void * object, Ret (T::*func)(Param0)) : m_Object(object), m_func_(func) {}

	virtual Ret invoke(Param0 param0)
	{
		T * obj = static_cast<T *>(m_Object);
		return (obj->*m_func_)(param0);
	}
private:
	void *m_Object;
	Ret (T::*m_func_)(Param0);
};

template<typename Ret, typename Param0>
class Delegate
{
public:
	Delegate(Ret(*func)(Param0)) : m_Callback(new StaticFunctionCallback<Ret, Param0>(func)){}

	template <typename T>
	Delegate(T *object, Ret(T::*func)(Param0)) : m_Callback(new MemberCallback<Ret, T, Param0>(object, func)) {}

	~Delegate() { delete m_Callback; }

	Ret operator()(Param0 param0)
	{
		return m_Callback->invoke(param0);
	}

private:
	Callback<Ret, Param0> *m_Callback;
};