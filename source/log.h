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

//---------------------------------------------------------------------------
/// Writes the message to the log file.
//---------------------------------------------------------------------------
void WriteToLog(const char* message, MsgType type, const char* file, int line, const char* function);

void PrintInfo();

#define DATA_MSG(msg) WriteToLog(msg, MsgType::Data, __FILE__, __LINE__, __FUNCTION__);
#define INFO_MSG(msg) WriteToLog(msg, MsgType::Info, __FILE__, __LINE__, __FUNCTION__);
#define ERROR_MSG(msg) WriteToLog(msg, MsgType::Error, __FILE__, __LINE__, __FUNCTION__);

#ifdef ERROR_CONTEXT
#error "ERROR_CONTEXT" already defined.
#endif

//---------------------------------------------------------------------------
/// Attribute used with WriteToLog(), IsNullptr(), IsFalse(), etc.
//---------------------------------------------------------------------------
#define ERROR_CONTEXT __FILE__, __LINE__, __FUNCTION__

//---------------------------------------------------------------------------
/// Checks if the given pointer is a nullptr. If so an error is written to the log file.
/// @note: Should be used with ERROR_CONTEXT
/// @param[in] ptr			The pointer to check.
/// @param[in] message		Message that will be written to the log file.
/// @param[in] file			Name of the source code file.
/// @param[in] line			Number of the current line.
/// @param[in] function		Name of the current function.
/// @return					True if the given pointer is a nullptr.
//---------------------------------------------------------------------------
template<typename T> static bool IsNullptr(const T* const ptr, const char* message, const char* file, int line, const char* function)
{
	if (ptr == nullptr)
	{
		WriteToLog(message, MsgType::Error, file, line, function);
		return true;
	}

	return false;
}

//---------------------------------------------------------------------------
/// Checks if the given value is false. If so an error is written to the log file.
/// @note: Should be used with ERROR_CONTEXT
/// @param[in] value		The value to check.
/// @param[in] message		Message that will be written to the log file.
/// @param[in] file			Name of the source code file.
/// @param[in] line			Number of the current line.
/// @param[in] function		Name of the current function.
/// @return					True if the given value is false.
//---------------------------------------------------------------------------
static bool IsFalse(bool value, const char* message, const char* file, int line, const char* function)
{
	if (value == false)
	{
		WriteToLog(message, MsgType::Error, file, line, function);
		return true;
	}

	return false;
}

//---------------------------------------------------------------------------
/// Checks if the given value is null. If so an error is written to the log file.
/// @note: Should be used with ERROR_CONTEXT
/// @param[in] value		The value to check.
/// @param[in] message		Message that will be written to the log file.
/// @param[in] file			Name of the source code file.
/// @param[in] line			Number of the current line.
/// @param[in] function		Name of the current function.
/// @return					True if the given value is null.
//---------------------------------------------------------------------------
template<typename T> static bool IsNull(const T& value, const char* message, const char* file, int line, const char* function)
{
	if (value == 0)
	{
		WriteToLog(message, MsgType::Error, file, line, function);
		return true;
	}
	return false;
}

//---------------------------------------------------------------------------
/// Checks if the given value is the same as the reference value. If so an error is written to the log file.
/// @note: Should be used with ERROR_CONTEXT
/// @param[in] value		The value to check.
/// @param[in] ref			The reference value.
/// @param[in] message		Message that will be written to the log file.
/// @param[in] file			Name of the source code file.
/// @param[in] line			Number of the current line.
/// @param[in] function		Name of the current function.
/// @return					True if the given value is equal to the reference value.
//---------------------------------------------------------------------------
template<typename T> static bool IsValue(const T& value, const T& ref, const char* message, const char* file, int line, const char* function)
{
	if (value == ref)
	{
		WriteToLog(message, MsgType::Error, file, line, function);
		return true;
	}

	return false;
}

//---------------------------------------------------------------------------
/// Checks if the given value is not the same as the reference value. If so an error is written to the log file.
/// @note: Should be used with ERROR_CONTEXT
/// @param[in] value		The value to check.
/// @param[in] ref			The reference value.
/// @param[in] message		Message that will be written to the log file.
/// @param[in] file			Name of the source code file.
/// @param[in] line			Number of the current line.
/// @param[in] function		Name of the current function.
/// @return					True if the given value is not equal to the reference value.
//---------------------------------------------------------------------------
template<typename T> static bool IsNotValue(const T& value, const T& ref, const char* message, const char* file, int line, const char* function)
{
	if (value != ref)
	{
		WriteToLog(message, MsgType::Error, file, line, function);
		return true;
	}

	return false;
}

#endif // VOLUME_DEMO_LOG_H__
