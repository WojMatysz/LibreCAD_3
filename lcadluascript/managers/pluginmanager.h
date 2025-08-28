#pragma once

#include <filesystem>

#include <kaguya/kaguya.hpp>

namespace lc 
{
namespace lua 
{

class PluginManager 
{
public:
    PluginManager(kaguya::State & luaVM, const std::string & interface);

    void loadPlugins();

private:
    void loadPlugin(std::filesystem::path file);

    kaguya::State & m_luaVirtualMachine;
    const std::string m_interface;
};

}
}
