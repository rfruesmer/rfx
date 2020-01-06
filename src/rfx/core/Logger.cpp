#include "rfx/pch.h"
#include "rfx/core/Logger.h"
#include <iomanip>

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

#ifdef _WINDOWS
string nowTime()
{
    FILETIME fileTime;
    GetSystemTimeAsFileTime(&fileTime);

    SYSTEMTIME systemTime;
    FileTimeToSystemTime(&fileTime, &systemTime);

    ostringstream oss;
    oss << setfill('0') 
        << setw(2) << systemTime.wHour << ":"
        << setw(2) << systemTime.wMinute << ":"
        << setw(2) << systemTime.wSecond << ":"
        << setw(3) << systemTime.wMilliseconds;

    return oss.str();
}
#else
static_assert(false, "not implemented");
#endif // _WINDOWS

// ---------------------------------------------------------------------------------------------------------------------

string toString(LogLevel logLevel)
{
    switch (logLevel)
    {
    case LogLevel::FATAL:
        return "FATAL";
    case LogLevel::ERROR:
        return "ERROR";
    case LogLevel::WARNING:
        return "WARNING";
    case LogLevel::INFO:
        return "INFO";
    case LogLevel::DEBUG:
        return "DEBUG";
    case LogLevel::TRACE:
        return "TRACE";
    default:
        return "UNKNOWN";
    }
}

// ---------------------------------------------------------------------------------------------------------------------

LogLevel Logger::logLevel = LogLevel::INFO;

// ---------------------------------------------------------------------------------------------------------------------

void Logger::setLogLevel(LogLevel logLevel)
{
    Logger::logLevel = logLevel;
}

// ---------------------------------------------------------------------------------------------------------------------

bool Logger::isFiltered(LogLevel logLevel)
{
    return logLevel > Logger::logLevel;
}

// ---------------------------------------------------------------------------------------------------------------------

LogLevel Logger::toLogLevel(const std::string& logLevelAsString)
{
    static unordered_map<string, LogLevel> logLevelMapping = {
        {"FATAL", LogLevel::FATAL},
        {"ERROR", LogLevel::ERROR},
        {"WARNING", LogLevel::WARNING},
        {"INFO", LogLevel::INFO},
        {"DEBUG", LogLevel::DEBUG},
        {"TRACE", LogLevel::TRACE}
    };

    const auto& it = logLevelMapping.find(logLevelAsString);
    return it != logLevelMapping.end() ? it->second : LogLevel::INFO;
}

// ---------------------------------------------------------------------------------------------------------------------

Logger::~Logger()
{
    os << endl << "\t" << messageFile << "(" << messageLine << ")" << endl;

#ifdef _WINDOWS
    OutputDebugStringA(os.str().c_str());
#else
    cout << os.str();
#endif // _WINDOWS
}

// ---------------------------------------------------------------------------------------------------------------------

ostringstream& Logger::get(LogLevel logLevel, const char* file, int line)
{
    messageLogLevel = logLevel;
    messageFile = file;
    messageLine = line;
    
    os << nowTime() << " " << toString(logLevel) << ": ";

    return os;
}

// ---------------------------------------------------------------------------------------------------------------------
