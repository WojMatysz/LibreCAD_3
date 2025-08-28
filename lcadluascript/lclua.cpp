#include "lclua.h"
#include <cad/storage/document.h>

#include <utils/timer.h>
#include <managers/luacustomentitymanager.h>
#include <kaguya/kaguya.hpp>
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


LCLua::LCLua(kaguya::State & luaVM) :
    m_luaVM(luaVM),
    _f_openFileDialog(nullptr) {

    m_luaVM["registerPlugin"].setFunction([](const std::string& name, kaguya::LuaRef onNewWaitingEntityFunction) {
        LuaCustomEntityManager::getInstance().registerPlugin(name, onNewWaitingEntityFunction);
    });
}

void LCLua::addLuaLibs() {
    const luaL_Reg *lib;

    
    /* Originaly only those libs are loaded
       static const luaL_Reg loadedlibs[] = {
       {"_G", luaopen_base},
       {LUA_LOADLIBNAME, luaopen_package},
       {LUA_COLIBNAME, luaopen_coroutine},
       {LUA_TABLIBNAME, luaopen_table},
       {LUA_STRLIBNAME, luaopen_string},
       {LUA_MATHLIBNAME, luaopen_math},
       {nullptr, nullptr}
       };
       */
    m_luaVM.openlibs(); // Loads all stadndard libs

    //Add others non-LC tools
    m_luaVM["microtime"].setFunction(&lua_microtime);
    m_luaVM["openFile"].setFunction(&openFile);

    m_luaVM["FILE"].setClass(kaguya::UserdataMetatable<FILE>()
    .addStaticFunction("read", [](FILE* file, const size_t len) {
        return read(file, len);
    })
    .addStaticFunction("write", [](FILE* file, const char* content) {
        return write(file, content);
    })
                      );

    if(_f_openFileDialog == nullptr) {
        m_luaVM["openFileDialog"].setFunction([]() {
            return (FILE*) nullptr;
        });
    }
    else {
        m_luaVM["openFileDialog"].setFunction(_f_openFileDialog);
    }
}

void LCLua::setDocument(const lc::storage::Document_SPtr& document) {
    m_luaVM["document"] = document;
}

std::string LCLua::runString(const char* code) 
{
    try {
        m_luaVM.dostring(code);
        return "";
    } 
    catch(const kaguya::LuaException & e) {
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
    _f_openFileDialog = f_openFileDialog;
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
