#pragma once

#ifdef _WINDOWS
// imported from wingdi.h:
#undef ERROR
#endif // _WINDOWS

#define	RFX_LOG(level) \
    if (rfx::Logger::isFiltered(level)) ; \
    else rfx::Logger().get(level, __FILE__, __LINE__)

#define RFX_LOG_TRACE	RFX_LOG(rfx::LogLevel::TRACE)
#define RFX_LOG_DEBUG	RFX_LOG(rfx::LogLevel::DEBUG)
#define RFX_LOG_INFO	RFX_LOG(rfx::LogLevel::INFO)
#define RFX_LOG_WARNING	RFX_LOG(rfx::LogLevel::WARNING)
#define RFX_LOG_ERROR	RFX_LOG(rfx::LogLevel::ERROR)
#define RFX_LOG_FATAL	RFX_LOG(rfx::LogLevel::FATAL)

namespace rfx
{

enum class LogLevel : uint8_t
{
    FATAL,
    ERROR,
    WARNING,
    INFO,
    DEBUG,
    TRACE
};


class Logger
{
public:
    static void setLogLevel(LogLevel logLevel);
    static bool isFiltered(LogLevel logLevel);
    static LogLevel toLogLevel(const std::string& logLevelAsString);
    
    ~Logger();
    
    std::ostringstream& get(LogLevel logLevel, const char* file, int line);

private:
    static LogLevel logLevel;

    std::ostringstream os;
    LogLevel messageLogLevel = LogLevel::INFO;
    std::string messageFile;
    int messageLine = -1;
};

} // namespace rfx


