#ifndef VOLUME_DEMO_LOG_H__
#define VOLUME_DEMO_LOG_H__

enum class MsgType 
{ 
	Data,
	Info, 
	Error 
};

//---------------------------------------------------------------------------
/// Tests the error handling system.
//---------------------------------------------------------------------------
bool UnitTestLogSystem();

void PrintInfo();


#include <functional>

namespace error_sys_intern
{
struct ErrorInfo
{
	const char* _file;
	int _line;
	std::string _msgStr;
};

//---------------------------------------------------------------------------
/// Writes the message to the log file.
/// Use InfoMessage(), DataMessage() or ErrorMessage() instead.
//---------------------------------------------------------------------------
void WriteToLog(const char* message, MsgType type, const char* file, int line, const char* function);

template<typename F> static void WriteMessage(const char* functionName, F && f, MsgType type)
{
	error_sys_intern::ErrorInfo info;
	f(info);
	error_sys_intern::WriteToLog(info._msgStr.c_str(), type, info._file, info._line, functionName);
}
}

#ifdef MSG_INFO
#error "MSG_INFO" already defined.
#endif

#define MSG_INFO(msg) __func__, \
					[&](error_sys_intern::ErrorInfo& info) -> void \
					{\
						info._msgStr = std::string(msg); \
						info._file = __FILE__; \
						info._line = __LINE__; \
					}

template<typename F> static void InfoMessage(const char* functionName, F && f)
{
	error_sys_intern::WriteMessage(functionName, f, MsgType::Info);
}

template<typename F> static void DataMessage(const char* functionName, F && f)
{
	error_sys_intern::WriteMessage(functionName, f, MsgType::Data);
}

template<typename F> static void ErrorMessage(const char* functionName, F && f)
{
	error_sys_intern::WriteMessage(functionName, f, MsgType::Error);
}

//---------------------------------------------------------------------------
/// Checks if the given pointer is a nullptr. If so an error is written to the log file.
/// @note: Should be used with MSG_INFO
/// @param[in] ptr			The pointer to check.
/// @param[in] function		Name of the current function.
/// @param[in] f			Function called if an error is detected.
/// @return					True if the given pointer is a nullptr.
//---------------------------------------------------------------------------
template<typename T, typename F> static bool IsNullptr(const T* const ptr, const char* function, F && f)
{
	if (ptr == nullptr)
	{
		ErrorMessage(function, f);
		return true;
	}

	return false;
}

//---------------------------------------------------------------------------
/// Checks if the given value is false. If so an error is written to the log file.
/// @note: Should be used with MSG_INFO.
/// @param[in] value		The value to check.
/// @param[in] function		Name of the current function.
/// @param[in] f			Function called if an error is detected.
/// @return					True if the given pointer is false.
//---------------------------------------------------------------------------
template<typename F> static bool IsFalse(bool value, const char* function, F && f)
{
	if (value == false)
	{
		ErrorMessage(function, f);
		return true;
	}

	return false;
}

//---------------------------------------------------------------------------
/// Checks if the given value is null. If so an error is written to the log file.
/// @note: Should be used with MSG_INFO.
/// @param[in] value		The value to check.
/// @param[in] function		Name of the current function.
/// @param[in] f			Function called if an error is detected.
/// @return					True if the given pointer is null.
//---------------------------------------------------------------------------
template<typename T, typename F> static bool IsNull(const T& value, const char* function, F && f)
{
	if (value == 0)
	{
		ErrorMessage(function, f);
		return true;
	}
	return false;
}

//---------------------------------------------------------------------------
/// Checks if the given value is the same as the reference value. If so an error is written to the log file.
/// @note: Should be used with MSG_INFO.
/// @param[in] value		The value to check.
/// @param[in] ref			The reference value.
/// @param[in] function		Name of the current function.
/// @param[in] f			Function called if an error is detected.
/// @return					True if the given value is equal to the reference value.
//---------------------------------------------------------------------------
template<typename T, typename F> static bool IsValue(const T& value, const T& ref, const char* function, F && f)
{
	if (value == ref)
	{
		ErrorMessage(function, f);
		return true;
	}

	return false;
}

//---------------------------------------------------------------------------
/// Checks if the given value is not the same as the reference value. If so an error is written to the log file.
/// @note: Should be used with MSG_INFO.
/// @param[in] value		The value to check.
/// @param[in] ref			The reference value.
/// @param[in] function		Name of the current function.
/// @param[in] f			Function called if an error is detected.
/// @return					True if the given value is not equal to the reference value.
//---------------------------------------------------------------------------
template<typename T, typename F> static bool IsNotValue(const T& value, const T& ref, const char* function, F && f)
{
	if (value != ref)
	{
		ErrorMessage(function, f);
		return true;
	}

	return false;
}

#endif // VOLUME_DEMO_LOG_H__
