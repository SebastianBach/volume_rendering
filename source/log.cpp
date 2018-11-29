#include "log.h"

#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <vector>

static bool g_unitTestMode = false;

bool UnitTestLogSystem()
{
	g_unitTestMode = true;

	std::vector<std::string> foundErrors;

	// vector::push_back() might throw exception bad_alloc
	try
	{
		const int* const n_ptr = nullptr;
		if(IsNullptr(n_ptr, MSG_INFO("")) == false)
			foundErrors.push_back(std::string("False negative in IsNullptr())"));

		const int iValue = 123;
		if(IsNullptr(&iValue, MSG_INFO("")) == true)
			foundErrors.push_back(std::string("False positive in IsNullptr())"));

		if (IsFalse(false, MSG_INFO("")) == false)
			foundErrors.push_back(std::string("False negative in IsFalse())"));

		if (IsFalse(true, MSG_INFO("")) == true)
			foundErrors.push_back(std::string("False positive in IsFalse())"));

		if (IsNull(0, MSG_INFO("")) == false)
			foundErrors.push_back(std::string("False negative in IsNull())"));

		if (IsNull(1, MSG_INFO("")) == true)
			foundErrors.push_back(std::string("False positiv in IsNull())"));

		if (IsValue(1, 0, MSG_INFO("")) == true)
			foundErrors.push_back(std::string("False positive in IsValue()"));

		if (IsValue(1, 1, MSG_INFO("")) == false)
			foundErrors.push_back(std::string("False negative in IsValue()"));

		if (IsNotValue(1, 0, MSG_INFO("")) == false)
			foundErrors.push_back(std::string("False negative in IsNotValue()"));

		if (IsNotValue(1, 1, MSG_INFO("")) == true)
			foundErrors.push_back(std::string("False positive in IsNotValue()"));
	}
	catch (std::bad_alloc& ba)
	{
		g_unitTestMode = false;
		ErrorMessage(MSG_INFO(ba.what()));
		return false;
	}

	g_unitTestMode = false;

	// report errors
	if (foundErrors.size() > 0)
	{
		for (const std::string& error : foundErrors)
		{
			ErrorMessage(MSG_INFO(error.c_str()));
		}

		return false;
	}

	return true;
}

//---------------------------------------------------------------------------
/// Returns true if the given message type is an error.
/// @param[in]	type		The message tpe to check
/// @return					True if the message type is Msg::TypeError.
//---------------------------------------------------------------------------
static bool IsError(error_sys_intern::MsgType type)
{
	return type == error_sys_intern::MsgType::Error;
}

//---------------------------------------------------------------------------
/// Prints the current time.
/// @param[in]	stream		The file stream to write into.
//---------------------------------------------------------------------------
static void PrintTime(std::ofstream& stream)
{
	SYSTEMTIME systemTime;

	GetLocalTime(&systemTime);

	stream << std::to_string(systemTime.wDay);
	stream << "-";
	stream << std::to_string(systemTime.wMonth);
	stream << "-";
	stream << std::to_string(systemTime.wYear);
	stream << " - ";
	stream << std::to_string(systemTime.wHour);
	stream << ":";
	stream << std::to_string(systemTime.wMinute);
	stream << ":";
	stream << std::to_string(systemTime.wSecond);
	stream << "\n";
}

//---------------------------------------------------------------------------
/// Prints error information to the given stream and the console.
/// @param[in]  stream		The stream to write into.
/// @param[in]	type		The message type. Must be MsgType::Error.
/// @param[in]	file		The file name.
/// @param[in]	line		The line number.
/// @param[in]	function	The function name.
//---------------------------------------------------------------------------
static void PrintError(std::ofstream& stream, error_sys_intern::MsgType type, const char* file, int line, const char* function)
{
	if (IsError(type))
	{
		stream << "Error in File: ";
		stream << file;

		stream << " at line ";

		const std::string lineStr = std::to_string(line);
		stream << lineStr;

		stream << " (";
		stream << function;
		stream << ")";

		stream << "\n";

		// print to std::cout
		std::cout << file << std::endl;

		// print to console window
		OutputDebugStringA(file);
		OutputDebugStringA(" - ");
		OutputDebugStringA(function);
		OutputDebugStringA("\n");
	}
}

//---------------------------------------------------------------------------
/// Prints a message to the stream an the console.
/// @param[in]	stream		Stream to write to.
/// @param[in]	message		The message to print.
//---------------------------------------------------------------------------
static void PrintMessage(std::ofstream& stream, const char* message)
{
	stream << ">>> ";
	stream << message;
	stream << "\n";

	// print to std::cout
	std::cout << message << std::endl;

	// print to console window
	OutputDebugStringA(message);
	OutputDebugStringA("\n");
}

void error_sys_intern::WriteToLog(const char* message, MsgType type, const char* file, int line, const char* function)
{
	// do nothing in unit test mode
	if (g_unitTestMode)
		return;

	// get application location
	char  appFilePath[MAX_PATH];
	GetModuleFileNameA(GetModuleHandle(0), appFilePath, sizeof(appFilePath));

	// create log file location next to the application location
	std::string logFilePath(appFilePath);
	const std::size_t found = logFilePath.find("exe");
	logFilePath.replace(found, 3, "txt");

	// write to file
	std::ofstream stream(logFilePath, std::ofstream::app);

	if(type != MsgType::Data)
		PrintTime(stream);

	PrintError(stream, type, file, line, function);
	PrintMessage(stream, message);

	stream.close();

	if (IsError(type))
	{
		// You hit this debug break beause some error occured.
		// Check the output console or the log file for more info.

		DebugBreak();
	}
}

