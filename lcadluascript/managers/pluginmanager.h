#pragma once

#include <filesystem>

#include "sol.hpp"

namespace lc 
{
namespace lua 
{

class PluginManager 
{
public:
    PluginManager(sol::state & luaVM, const std::string & interface);

    void loadPlugins();

private:
    void loadPlugin(std::filesystem::path file);

    sol::state & m_luaVirtualMachine;
    const std::string m_interface;
};

}
}
