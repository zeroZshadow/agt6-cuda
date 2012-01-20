template<typename T>
T* Singleton<T>::m_Instance = 0;

template<typename T>
Singleton<T>::Singleton()
{
	if(m_Instance)
	{
		// Report error that singleton already is initialized
		return;
	}

	m_Instance = static_cast<T*>(this);
}

template<typename T>
Singleton<T>::~Singleton()
{
	if(m_Instance)
	{
		//delete m_Instance;
		m_Instance = 0;
	}
}

template<typename T>
void Singleton<T>::CreateInstance()
{
	if(!m_Instance)
	{
		m_Instance = new T();
	}
}

template<typename T>
void Singleton<T>::DestroyInstance()
{
	if(m_Instance)
	{
		delete m_Instance;
		m_Instance = 0;
	}
}

template<typename T>
T* Singleton<T>::GetInstance()
{
	if(!m_Instance)
	{
		CreateInstance();
	}

	return m_Instance;
}

template<typename T>
bool Singleton<T>::IsInstanceCreated()
{
	return m_Instance ? true : false;
}