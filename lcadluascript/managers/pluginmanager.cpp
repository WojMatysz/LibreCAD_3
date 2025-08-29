#include "pluginmanager.h"

using namespace lc::lua;

PluginManager::PluginManager(sol::state & luaVM, const std::string & interface) :
    m_luaVirtualMachine(luaVM),
    m_interface(interface) {}


void PluginManager::loadPlugins() 
{
    namespace fs = std::filesystem;
    fs::path pluginsDirecory{"lcUILua/plugins/"};

    try 
    {
        // Iterate over all entries in the plugins directory
        for (const auto & entry : fs::directory_iterator(pluginsDirecory)) {
            // Only process directories
            if (entry.is_directory()) {
                fs::path pluginFile = entry.path() / "plugin.lua";

                // Check if plugin.lua exists before loading
                if (fs::exists(pluginFile) && fs::is_regular_file(pluginFile)) 
                    loadPlugin(pluginFile.string().c_str());
                else 
                    std::cerr << "No plugin.lua in: " << entry.path() << "\n";
                
            }
        }
    } 
    catch(const fs::filesystem_error& e) 
    {
        std::cerr << "Error accessing plugins directory:\n" << e.what() << "\n";
    }
}

void PluginManager::loadPlugin(std::filesystem::path file) 
{
    m_luaVirtualMachine["LC_interface"] = m_interface;

    try 
    {
        m_luaVirtualMachine.script_file(file.string());
        std::cout << "Plugin: " << file << " loaded to LuaVM\n";
    } 
    catch(const sol::error& e) {
        std::cerr << "Failed to load plugin: " << file << "\n"
                  << e.what() << "\n";
    }
}
