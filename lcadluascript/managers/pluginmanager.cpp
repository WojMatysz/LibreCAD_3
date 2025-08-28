#include "pluginmanager.h"
#include <kaguya/kaguya.hpp>

using namespace lc::lua;

PluginManager::PluginManager(lua_State* l, const char* interface) :
    _L(l),
    _interface(interface) {

}



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
    catch (const fs::filesystem_error& e) 
    {
        std::cerr << "Error accessing plugins directory:\n" << e.what() << "\n";
    }
}

void PluginManager::loadPlugin(std::filesystem::path file) 
{
    kaguya::State state(_L);
    state["LC_interface"] = _interface;

    if (state.dofile(file.c_str())) {
        kaguya::LuaRef result = state["_RESULT"];
        if (result && result.isType<std::string>()) {
            std::cout << result.get<std::string>() << "\n";
        }
    }
    else std::cerr << "Failed to load plugin: " << file << "\n";
}
