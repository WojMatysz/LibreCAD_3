#include "lclua.h"
#include <cad/storage/document.h>

#include <utils/timer.h>
#include <managers/luacustomentitymanager.h>

#include <bridge/lc.h>
#include <bridge/lc_geo.h>
#include <bridge/lc_meta.h>
#include <bridge/lc_entity.h>
#include <bridge/lc_builder.h>
#include <bridge/lc_storage.h>
#include <bridge/lc_maths.h>
#include <bridge/lc_operation.h>
#include <bridge/lc_event.h>

using namespace lc::lua;


LCLua::LCLua(sol::state & luaVM) :
    m_luaVM(luaVM),
    m_f_openFileDialog(nullptr) 
{

    m_luaVM.set_function("registerPlugin", [](const std::string & name, sol::function onNewWaitingEntityFunction) {
        LuaCustomEntityManager::getInstance().registerPlugin(name, onNewWaitingEntityFunction);
            });
}

void LCLua::addLuaLibs() {
    //const luaL_Reg *lib;

    m_luaVM.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine,
                           sol::lib::table, sol::lib::string, sol::lib::math);

    //Add others non-LC tools
    m_luaVM.set_function("microtime", &lua_microtime);
    m_luaVM.set_function("openFile", &openFile);

    m_luaVM.new_usertype<FILE>("FILE", 
        // bind functions
        "read", [](FILE * file, const std::size_t len) { return read(file, len); },
        "write", [](FILE * file, const char * content) { return write(file, content); }
        );

    if(m_f_openFileDialog) m_luaVM.set_function("openFileDialog", m_f_openFileDialog);
    else m_luaVM.set_function("openFileDialog", []() -> FILE * { return nullptr; });
}

void LCLua::setDocument(const lc::storage::Document_SPtr& document) {
    m_luaVM["document"] = document;
}

std::string LCLua::runString(const char* code) 
{
    try {
        m_luaVM.script(code);
        return "";
    } 
    catch(const sol::error & e) {
        std::cerr << "Lua error: " << e.what() << "\n";
        return e.what();
    }
}

FILE* LCLua::openFile(const char* path, const char* mode) {
    //TODO: check if the file can be opened
    return fopen(path, mode);
}

std::string LCLua::read(FILE* file, size_t len) {
    char* buf = new char[len + 1];

    size_t n = fread(buf, sizeof(char), len, file);
    buf[n] = '\0';

    auto bufferStr = std::string(buf);
    delete buf;

    return bufferStr;
}

void LCLua::write(FILE* file, const char* content) {
    fwrite(content, sizeof(char), strlen(content), file);
}

void LCLua::setF_openFileDialog(FILE* (* f_openFileDialog)(bool, const char*, const char*)) {
    m_f_openFileDialog = f_openFileDialog;
}

void LCLua::importLCKernel() {
    import_lc_namespace(m_luaVM);
    import_lc_geo_namespace(m_luaVM);
    import_lc_meta_namespace(m_luaVM);
    import_lc_entity_namespace(m_luaVM);
    import_lc_builder_namespace(m_luaVM);
    import_lc_storage_namespace(m_luaVM);
    import_lc_maths_namespace(m_luaVM);
    import_lc_event_namespace(m_luaVM);
    import_lc_operation_namespace(m_luaVM);
}
