#include "Logger.h"
#include <memory>
#include <format>
#include "uv.h"
#include "Utils.h"
#include "UVUtils.h"
using namespace MessagingMesh;

// Static fields...
std::atomic<std::shared_ptr<const Logger::VecCallback>> Logger::m_callbacks;
std::mutex Logger::m_mutex;

// Log levels as strings...
const std::string Logger::DEBUG_STRING = "DEBUG";
const std::string Logger::INFO_STRING = "INFO";
const std::string Logger::WARN_STRING = "WARN";
const std::string Logger::ERROR_STRING = "ERROR";
const std::string Logger::FATAL_STRING = "FATAL";
const std::string Logger::UNKNOWN_LOG_LEVEL_STRING = "[UNKNOWN-LOG-LEVEL]";


// Registers a function to be called when messages are logged.
void Logger::registerCallback(Callback callback)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    // Copy-on-write pattern...
    auto oldCallbacks = m_callbacks.load();
    auto newCallbacks = std::make_shared<VecCallback>(oldCallbacks ? *oldCallbacks : VecCallback{});
    newCallbacks->push_back(callback);
    m_callbacks.store(newCallbacks);
}

// Logs a message at the level specified.
void Logger::log(LogLevel logLevel, const std::string& message)
{
    // Note that we do not use the mutex here. Instead we atomically load the callbacks shared pointer
    // giving us a reference to the immutable collection of callbacks currently in effect.
    auto pCallbacks = m_callbacks.load();
    if (!pCallbacks || pCallbacks->empty())
    {
        return;
    }

    // We add info to the message...
    auto threadName = UVUtils::getThreadName();
    auto messageToLog = std::format("MessagingMesh ({}): {}", threadName, message);

    // We notify the registered callbacks...
    for (auto& callback : *pCallbacks)
    {
        try
        {
            callback(logLevel, messageToLog);
        }
        catch (...)
        {
            // We catch exceptions so that we continue logging to all callbacks even
            // if one of them throws. We do not do anything here. In particular we do 
            // not log the error, as we are the logger!
        }
    }
}

// Logs a message at DEBUG level.
void Logger::debug(const std::string& message)
{
    log(LOG_LEVEL_DEBUG, message);
}

// Logs a message at INFO level.
void Logger::info(const std::string& message)
{
    log(LOG_LEVEL_INFO, message);
}

// Logs a message at WARN level.
void Logger::warn(const std::string& message)
{
    log(LOG_LEVEL_WARN, message);
}

// Logs a message at ERROR level.
void Logger::error(const std::string& message)
{
    log(LOG_LEVEL_ERROR, message);
}

// Logs a message at FATAL level.
void Logger::fatal(const std::string& message)
{
    log(LOG_LEVEL_FATAL, message);
}

// Returns the log-level as a string.
const std::string& Logger::toString(LogLevel logLevel)
{
    switch (logLevel)
    {
    case LOG_LEVEL_DEBUG:
        return DEBUG_STRING;
    case LOG_LEVEL_INFO:
        return INFO_STRING;
    case LOG_LEVEL_WARN:
        return WARN_STRING;
    case LOG_LEVEL_ERROR:
        return ERROR_STRING;
    case LOG_LEVEL_FATAL:
        return FATAL_STRING;
    default:
        return UNKNOWN_LOG_LEVEL_STRING;
    }
}
