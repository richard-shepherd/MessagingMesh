#pragma once
#include <exception>
#include <string>

namespace MessagingMesh
{
    // Exception type thrown by messaging-mesh code.
    class Exception : public std::exception
    {
    public:
        Exception(const std::string& message) :
            std::exception(message.c_str())
        {
        }
    };

} // namespace

