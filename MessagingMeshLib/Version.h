#pragma once
#include <string>

namespace MessagingMesh
{
    /// <summary>
    /// Reports version info.
    /// </summary>
    class Version
    {
    public:
        // Gets the library version.
        static const std::string& getVersion() { return m_version; }

    private:
        inline static std::string m_version = "0.1";
    };
} // namespace


