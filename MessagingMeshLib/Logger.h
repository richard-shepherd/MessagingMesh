#pragma once
#include <functional>
#include <string>
#include <vector>
#include <mutex>

namespace MessagingMesh
{
    /// <summary>
    /// Not so much a logger as a log-broker.
    /// 
    /// Log messages can be logged to the Logger and clients can register
    /// for callbacks when messages are logged. The client can then show
    /// or log the messages.
    /// </summary>
    class Logger
    {
    // Public types...
    public:
        // Log level enum.
        enum LogLevel
        {
            LOG_LEVEL_DEBUG,
            LOG_LEVEL_INFO,
            LOG_LEVEL_WARN,
            LOG_LEVEL_ERROR,
            LOG_LEVEL_FATAL
        };

        // Callback signature.
        using Callback = std::function<void(LogLevel logLevel, const std::string& message)>;

        // Vector of callbacks.
        using VecCallback = std::vector<Callback>;

    // Public methods...
    public:
        // Registers a function to be called when messages are logged.
        static void registerCallback(Callback callback);

        // Logs a message at the level specified.
        static void log(LogLevel logLevel, const std::string& message);

        // Logs a message at DEBUG level.
        static void debug(const std::string& message);

        // Logs a message at INFO level.
        static void info(const std::string& message);

        // Logs a message at WARN level.
        static void warn(const std::string& message);

        // Logs a message at ERROR level.
        static void error(const std::string& message);

        // Logs a message at FATAL level.
        static void fatal(const std::string& message);

        // Returns the log-level as a string.
        static const std::string& toString(LogLevel logLevel);

    // Private data...
    private:
        // Vector of registered callbacks and a mutex for it...
        static VecCallback m_callbacks;
        static std::mutex m_mutex;

        // Log levels as strings...
        static const std::string DEBUG_STRING;
        static const std::string INFO_STRING;
        static const std::string WARN_STRING;
        static const std::string ERROR_STRING;
        static const std::string FATAL_STRING;
        static const std::string UNKNOWN_LOG_LEVEL_STRING;
    };

} // namespace

