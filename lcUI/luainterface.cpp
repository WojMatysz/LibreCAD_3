#include <lclua.h>
#include <managers/luacustomentitymanager.h>
#include "luainterface.h"

#include <QDir>
#include <QFileDialog>

#include "mainwindow.h"
#include "operationloader.h"

using namespace lc::ui;

LuaInterface::LuaInterface() :
    _pluginManager(_L, "gui") {
}

LuaInterface::~LuaInterface() {
    _events.clear();

    lc::lua::LuaCustomEntityManager::getInstance().removePlugins();
}

void LuaInterface::initLua(QMainWindow* mainWindow) {
    auto lcLua = lc::lua::LCLua(_L);
    lcLua.setF_openFileDialog(&LuaInterface::openFileDialog);
    lcLua.addLuaLibs();
    lcLua.importLCKernel();

    luaOpenGUIBridge(_L);

    _L["luaInterface"] = this;
    registerGlobalFunctions(mainWindow);

    QString luaFile = QCoreApplication::applicationDirPath() + "/path.lua";
    sol::protected_function_result result = _L.script_file(luaFile.toStdString(), sol::script_pass_on_error);

    std::string luaPath = _L["lua_path"];
    lc::ui::OperationLoader opLoader(luaPath, mainWindow, _L);

    if (!result.valid()) {
        sol::error err = result;
        LOG_WARNING << "Lua error: " << err.what() << std::endl;
    } 
    else 
    {
        sol::optional<std::string> out = result;
        if(out) LOG_INFO << "Lua output: " << *out << "\n";
        else LOG_INFO << "Lua script executed successfully (no string result)\n";
    }

    _pluginManager.loadPlugins();
}

QWidget* LuaInterface::loadUiFile(const char* fileName) {
    QUiLoader uiLoader;
    QFile file(fileName);
    file.open(QFile::ReadOnly);

    QWidget* widget = uiLoader.load(&file);

    file.close();

    return widget;
}

void LuaInterface::hideUI(bool hidden) {
    _L["hideUI"] = hidden;
}


std::vector<std::string> LuaInterface::pluginList(const char* path) {
    std::vector<std::string> plugins;
    QDir dir(path);

    auto list = dir.entryList(QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot);
    for(const auto& dir : list) {
        plugins.push_back(dir.toStdString());
    }

    return plugins;
}

FILE* LuaInterface::openFileDialog(bool isOpening, const char* description, const char* mode) {
    QString path;

    if(isOpening) {
        path = QFileDialog::getOpenFileName(nullptr, (std::string("Open ") + description).c_str());
    }
    else {
        path = QFileDialog::getSaveFileName(nullptr, (std::string("Save ") + description).c_str());
    }

    if(path.isEmpty()) {
        return nullptr;
    }

    return fopen(path.toStdString().c_str(), mode);
}

sol::table LuaInterface::operation() {
    return _operation;
}

void LuaInterface::setOperation(sol::table operation) {
    _operation = std::move(operation);
}

void LuaInterface::finishOperation() 
{
    if(_operation.valid())
    {
        sol::function close = _operation["close"];
        if(close.valid()) close(_operation);
    }
}

void LuaInterface::registerEvent(const std::string & event, const sol::object & callback) {
    if (!callback.valid()) return;

    if (callback.is<sol::table>()) {
        sol::table t = callback.as<sol::table>();
        sol::function onEvent = t["onEvent"];
        if (!onEvent.valid()) return;  // table without onEvent: ignore
    }

    // either a function, or a table with onEvent
    _events[event].push_back(callback);
}

void LuaInterface::deleteEvent(const std::string& event, const sol::object & callback) 
{
    auto it = std::find(_events[event].begin(), _events[event].end(), callback);
    if(it != _events[event].end()) _events[event].erase(it);
    
}

void LuaInterface::triggerEvent(const std::string& event, sol::table args) {
    auto events = _events[event];
    for(auto eventCallback : events) {
        if(eventCallback.valid() && eventCallback.is<sol::function>()) {

            sol::function callback = eventCallback;
            callback(event, args);
        }
        else if(eventCallback.valid() && eventCallback.is<sol::table>()) {
            sol::table callbackTable = eventCallback;
            callbackTable["onEvent"](eventCallback, event, args);
        }
    }
}

void LuaInterface::registerGlobalFunctions(QMainWindow* mainWindow) {
    // register common functions i.e. run_basic_operation and message
    _L["mainWindow"] = static_cast<lc::ui::MainWindow*>(mainWindow);
    _L.script("run_basic_operation = function(operation, init_method) mainWindow:runOperation(operation, init_method) end");
    _L.script("finish_operation = function() luaInterface:finishOperation() end");
    _L.script("operationFinished = function() mainWindow:operationFinished() end");

    // cli command helper functions
    _L.script("message = function(m) mainWindow:cliCommand():write(tostring(m)) end");
    _L.script("cli_get_text = function(getText) mainWindow:cliCommand():returnText(getText) end");
    _L.script("add_command = function(command, callback) mainWindow:cliCommand():addCommand(command, callback) end");
    _L.script("run_command = function(command) mainWindow:cliCommand():runCommand(command) end");
    _L.script("add_command('CLEAR', function() mainWindow:cliCommand():clear() end)");
    _L.script("CreateDialogWidget = function(widgetName) return gui.DialogWidget(widgetName,mainWindow) end");

    _L.script("luaInterface:registerEvent('finishOperation', finish_operation)");
    _L.script("luaInterface:registerEvent('operationFinished', operationFinished)");
}
