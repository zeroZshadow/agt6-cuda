#pragma once

template<typename T>
class Singleton
{
public:
	Singleton();
	virtual ~Singleton();

	static void CreateInstance();
	static void DestroyInstance();
	static T*	GetInstance();
	static bool	IsInstanceCreated();

private:
	static T* m_Instance;
};

#include "Singleton.inl"