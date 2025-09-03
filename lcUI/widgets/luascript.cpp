#include "luascript.h"
#include "ui_luascript.h"

#include <lua/guibridge.h>

using namespace lc::ui::widgets;

LuaScript::LuaScript(lc::ui::MainWindow* mainWindow) :
    ui(new Ui::LuaScript),
    _mainWindow(mainWindow),
    _mdiChild(mainWindow->cadMdiChild()),
    _cliCommand(mainWindow->cliCommand()) {
    ui->setupUi(this);

    auto lcLua = lc::lua::LCLua(m_luaVM);
    lcLua.setF_openFileDialog(&LuaInterface::openFileDialog);
    lcLua.addLuaLibs();
    lcLua.importLCKernel();
    luaOpenGUIBridge(m_luaVM);
    registerGlobalFunctions(m_luaVM);
}

LuaScript::~LuaScript() {
    delete ui;
}


void LuaScript::on_luaRun_clicked() {
    auto lcLua = lc::lua::LCLua(m_luaVM);
    lcLua.setDocument(_mdiChild->document());

    auto out = lcLua.runString(ui->luaInput->toPlainText().toStdString().c_str());
    _cliCommand->write(out);
}

void LuaScript::on_open_clicked() {
    auto fileName = QFileDialog::getOpenFileName(
                        nullptr,
                        tr("Open File"),
                        QString(),
                        tr("Lua (*.lua)")
                    );

    if(!fileName.isEmpty()) {
        QFile file(fileName);

        file.open(QFile::ReadOnly | QFile::Text);
        QTextStream stream(&file);

        ui->luaInput->setPlainText(stream.readAll());

        file.close();
    }
}

void LuaScript::on_save_clicked() {
    auto fileName = QFileDialog::getSaveFileName(
                        nullptr,
                        tr("Save File"),
                        QString(),
                        tr("Lua (*.lua)")
                    );

    if(!fileName.isEmpty()) {
        QFile file(fileName);

        file.open(QFile::WriteOnly | QFile::Text);
        QTextStream stream(&file);

        stream << ui->luaInput->toPlainText();

        file.close();
    }
}

void LuaScript::registerGlobalFunctions(sol::state & luaVM) 
{
    // register common functions i.e. run_basic_operation and message
    luaVM["mainWindow"] = static_cast<lc::ui::MainWindow*>(_mainWindow);
    luaVM.script("run_basic_operation = function(operation, init_method) mainWindow:runOperation(operation, init_method) end");

    // cli command helper functions
    luaVM.script("message = function(m) mainWindow:cliCommand():write(m) end");
    luaVM.script("add_command = function(command, callback) mainWindow:cliCommand():addCommand(command, callback) end");
    luaVM.script("run_command = function(command) mainWindow:cliCommand():runCommand(command) end");
    luaVM.script("CreateDialogWidget = function(widgetName) return gui.DialogWidget(widgetName,mainWindow) end");
}
