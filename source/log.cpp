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
		const int* const ptr = nullptr;
		if (IsNullptr(ptr, "", ERROR_CONTEXT) == false)
			foundErrors.push_back(std::string("False negative in IsNullptr())"));

		const int value = 123;
		if (IsNullptr(&value, "", ERROR_CONTEXT) == true)
			foundErrors.push_back(std::string("False positive in IsNullptr())"));

		if (IsFalse(false, "", ERROR_CONTEXT) == false)
			foundErrors.push_back(std::string("False negative in IsFalse())"));

		if (IsFalse(true, "", ERROR_CONTEXT) == true)
			foundErrors.push_back(std::string("False positive in IsFalse())"));

		if (IsNull(0, "", ERROR_CONTEXT) == false)
			foundErrors.push_back(std::string("False negative in IsNull())"));

		if (IsNull(1, "", ERROR_CONTEXT) == true)
			foundErrors.push_back(std::string("False positiv in IsNull())"));

		if (IsValue(1, 0, "", ERROR_CONTEXT) == true)
			foundErrors.push_back(std::string("False positive in IsValue()"));

		if (IsValue(1, 1, "", ERROR_CONTEXT) == false)
			foundErrors.push_back(std::string("False negative in IsValue()"));

		if (IsNotValue(1, 0, "", ERROR_CONTEXT) == false)
			foundErrors.push_back(std::string("False negative in IsNotValue()"));

		if (IsNotValue(1, 1, "", ERROR_CONTEXT) == true)
			foundErrors.push_back(std::string("False positive in IsNotValue()"));
	}
	catch (std::bad_alloc& ba)
	{
		g_unitTestMode = false;
		WriteToLog(ba.what(), MsgType::Error, ERROR_CONTEXT);
		return false;
	}

	g_unitTestMode = false;

	// report errors
	if (foundErrors.size() > 0)
	{
		for (const std::string& error : foundErrors)
		{
			WriteToLog(error.c_str(), MsgType::Error, ERROR_CONTEXT);
		}

		return false;
	}

	return true;
}

static bool IsError(MsgType type)
{
	return type == MsgType::Error;
}

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

static void PrintError(std::ofstream& stream, MsgType type, const char* file, int line, const char* function)
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

void PrintInfo()
{
	// prints application name
	// OS
	// Memory etc.
}

void WriteToLog(const char* message, MsgType type, const char* file, int line, const char* function)
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

