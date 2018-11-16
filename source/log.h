#ifndef VOLUME_DEMO_LOG_H__
#define VOLUME_DEMO_LOG_H__

enum class MsgType 
{ 
	Data,
	Info, 
	Error 
};

bool UnitTestLogSystem();

void WriteToLog(const char* message, MsgType type, const char* file, int line, const char* function);

void PrintInfo();

#define DATA_MSG(msg) WriteToLog(msg, MsgType::Data, __FILE__, __LINE__, __FUNCTION__);
#define INFO_MSG(msg) WriteToLog(msg, MsgType::Info, __FILE__, __LINE__, __FUNCTION__);
#define ERROR_MSG(msg) WriteToLog(msg, MsgType::Error, __FILE__, __LINE__, __FUNCTION__);

#ifdef ERROR_CONTEXT
#error "ERROR_CONTEXT" already defined.
#endif

#define ERROR_CONTEXT __FILE__, __LINE__, __FUNCTION__

template<typename T> static bool IsNullptr(const T* const ptr, const char* message, const char* file, int line, const char* function)
{
	if (ptr == nullptr)
	{
		WriteToLog(message, MsgType::Error, file, line, function);
		return true;
	}

	return false;
}

static bool IsFalse(bool value, const char* message, const char* file, int line, const char* function)
{
	if (value == false)
	{
		WriteToLog(message, MsgType::Error, file, line, function);
		return true;
	}

	return false;
}

template<typename T> static bool IsNull(const T& value, const char* message, const char* file, int line, const char* function)
{
	if (value == 0)
	{
		WriteToLog(message, MsgType::Error, file, line, function);
		return true;
	}
	return false;
}

template<typename T> static bool IsValue(const T& value, const T& ref, const char* message, const char* file, int line, const char* function)
{
	if (value == ref)
	{
		WriteToLog(message, MsgType::Error, file, line, function);
		return true;
	}

	return false;
}

template<typename T> static bool IsNotValue(const T& value, const T& ref, const char* message, const char* file, int line, const char* function)
{
	if (value != ref)
	{
		WriteToLog(message, MsgType::Error, file, line, function);
		return true;
	}

	return false;
}


#endif
