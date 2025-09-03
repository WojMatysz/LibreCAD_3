#include "operationloader.h"

#include "mainwindow.h"
#include "widgets/clicommand.h"
#include "widgets/toolbar.h"
#include "managers/contextmenumanager.h"

#include <QDir>
#include <QString>
#include <QStringList>

using namespace lc::ui;

OperationLoader::OperationLoader(const std::string& luaPath, QMainWindow* qmainWindow, sol::state & luaVM)
    :
    qmainWindow(qmainWindow),
    _L(luaVM) {
    loadLuaOperations(luaPath);
}

void OperationLoader::loadLuaOperations(const std::string& luaPath)
{
    // load operation definitions
    _L.script_file(luaPath + "/actions/operations.lua");
    _L.script_file(luaPath + "/createActions/createOperations.lua");
    loadLuaFolder("createActions", "createOperations", luaPath);

    // create list of all operations in creationGroup and dimensionsGroup
    getSetOfGroupElements();
    loadLuaFolder("actions", "operations", luaPath);

    // fetch all globals from the Lua state
    sol::table globalTable = _L.globals();

    // properties to track inside operation definitions - this is what we are looking for
    std::set<std::string> interestingProperties = 
    {
        "init",
        "command_line",
        "menu_actions",
        "icon",
        "description",
        "operation_options",
        "context_transitions"
    };

    // loop through all global Lua variables
    for (auto& globalEntry : globalTable) 
    {
        sol::object globalKey = globalEntry.first;
        sol::object globalValue = globalEntry.second;

        if (!globalKey.is<std::string>()) continue;
        std::string globalName = globalKey.as<std::string>();

        // look for globals whose name contains "Operation" and is a table
        if (globalName.find("Operation") != std::string::npos && globalValue.is<sol::table>())
        {
            sol::table operationTable = globalValue.as<sol::table>();
            foundProperties[globalName] = {};

            // check for interesting properties in the operation table
            for (auto& operationEntry : operationTable) 
            {
                sol::object propertyKey = operationEntry.first;
                if (!propertyKey.is<std::string>()) continue;
                std::string propertyName = propertyKey.as<std::string>();
                // if we found property from the set, then map it at current name
                if (interestingProperties.count(propertyName)) foundProperties[globalName].insert(propertyName);
            }

            // assign group name for this operation
            if (groupElements.at("creationGroupElements").count(globalName)) groupNames[globalName] = "Creation";
            else if (groupElements.at("dimensionsGroupElements").count(globalName)) groupNames[globalName] = "Dimensions";
            else groupNames[globalName] = "Modify";


            // initialize the operation
            initializeOperation(globalName);
        }
    }

    // cleanup
    _L["run_op"] = sol::lua_nil; 
}


void OperationLoader::loadLuaFolder(const std::string folderName, const std::string& fileToSkip, const std::string& luaPath) {
    QDir folderDir((luaPath + "/" + folderName).c_str());
    QStringList luaFiles = folderDir.entryList(QStringList() << "*.lua", QDir::Files);
    for (QString str : luaFiles)
    {
        std::string filename = str.toStdString();
        // skip fileToSkip.lua as it has been already called
        if (str.toStdString() != fileToSkip) {
            _L.script_file(luaPath + "/" + folderName + "/" + filename);
        }
    }
}

void OperationLoader::getSetOfGroupElements() {
    groupElements["creationGroupElements"] = {};
    groupElements["dimensionsGroupElements"] = {};

    // fetch all globals from the Lua state
    sol::table globalTable = _L.globals();

    // insert elements into their respective sets
    for (auto & globalEntry : globalTable)
    {
        sol::object globalKey = globalEntry.first;
        sol::object globalValue = globalEntry.second;
        if (!globalKey.is<std::string>()) continue;
        std::string globalName = globalKey.as<std::string>();

        if (globalName.find("Operation") == std::string::npos) continue;

        if (globalName.find("Dim") != std::string::npos) groupElements["dimensionsGroupElements"].insert(globalName);
        else groupElements["creationGroupElements"].insert(globalName);
    }
}

void OperationLoader::initializeOperation(const std::string& vkey)
{
    for (const std::string& opkey : foundProperties[vkey])
    {
        // init function
        if (opkey == "init") {
            // eg. _L["LineOperations"]["init"]()
            _L[vkey][opkey]();
        }

        // command line
        if (opkey == "command_line") {
            addOperationCommandLine(vkey, opkey);
        }

        // menu actions
        if (opkey == "menu_actions") {
            addOperationMenuAction(vkey, opkey);
        }

        // Toolbar attributes
        if (opkey == "icon") {
            addOperationIcon(vkey, opkey);
        }

        // operation icons
        if (opkey == "operation_options") {
            addOperationToolbarOptions(vkey, opkey);
        }

        // context transitions
        if (opkey == "context_transitions") {
            addContextTransitions(vkey, opkey);
        }
    }

    addContextMenuOperations(vkey);
}

void OperationLoader::addOperationCommandLine(const std::string & globalKey, const std::string & operationKey) 
{
    widgets::CliCommand* cliCommand = static_cast<MainWindow*>(qmainWindow)->cliCommand();

    auto createRunOp = [&](const std::string & globalKey, const std::string & init = "")
    {
        if (init.empty()) _L["run_op"] = _L.script("function() run_basic_operation(" + globalKey + ") end");
        else _L["run_op"] = _L.script("function() run_basic_operation(" + globalKey + ", '_init_" + init + "') end");
    };

    sol::object operation = _L[globalKey][operationKey];
    if (operation.is<std::string>())
    {
        createRunOp(globalKey);
        cliCommand->addCommand(operation.as<std::string>().c_str(), _L["run_op"]);
    }

    if (operation.is<sol::table>()) 
    {
        sol::table operationTable = operation.as<sol::table>();
        for (const auto & pair : operationTable)
        {
            std::string key = pair.first.as<std::string>();
            std::string command = pair.second.as<std::string>();

            // if key is digits only i.e. if no key provided, connect it to default init
            if (std::find_if(key.begin(), key.end(), [](unsigned char c) { return !std::isdigit(c); }) == key.end())
            {
                // connect to default init function
                createRunOp(globalKey);
                cliCommand->addCommand(command.c_str(), _L["run_op"]);
            }
            else {
                // connect to provided init function
                createRunOp(globalKey, command);
                cliCommand->addCommand(key.c_str(), _L["run_op"]);
            }
        }
    }
}

void OperationLoader::addOperationMenuAction(const std::string& vkey, const std::string& opkey) {
    MainWindow* mWindow = static_cast<MainWindow*>(qmainWindow);
    std::map<std::string, std::string> map = _L[vkey][opkey];

    for (auto element : map)
    {
        if (element.first == "default") _L.script("run_op = function() run_basic_operation(" + vkey + ") end");
        else _L.script("run_op = function() run_basic_operation(" + vkey + ", '_init_" + element.first + "') end");
        mWindow->connectMenuItem(element.second, _L["run_op"]);
    }
}

void OperationLoader::addOperationIcon(const std::string& vkey, const std::string& opkey) {
    widgets::Toolbar* toolbar = static_cast<MainWindow*>(qmainWindow)->toolbar();
    std::string icon = _L[vkey][opkey].get<std::string>();
    std::string tooltip;

    // if description not provided, use operation name
    if (foundProperties[vkey].find("description") != foundProperties[vkey].end()) {
        tooltip = _L[vkey]["description"].get<std::string>();
    }
    else {
        tooltip = vkey.substr(0, vkey.find("Operation"));
    }

    std::string iconPath = ":/icons/" + icon;
    _L.script("run_op = function() run_basic_operation(" + vkey + ") end");

    toolbar->addButton(vkey.c_str(), iconPath.c_str(), groupNames[vkey].c_str(), _L["run_op"], tooltip.c_str());
}


void OperationLoader::addOperationToolbarOptions(const std::string& vkey, const std::string& opkey) 
{
    MainWindow* mWindow = static_cast<MainWindow*>(qmainWindow);
    sol::table options = _L[vkey][opkey];

    std::vector<sol::function> optionsList;
    for (const auto & element : options) 
    {
        std::string key = element.first.as<std::string>();
        sol::object value = element.second;

        // operation_options for init_method
        if (key.find("_init") != std::string::npos) 
        {
            sol::table optionsInit = value.as<sol::table>();

            std::vector<sol::function> optionsInitList;
            for (const auto & elementInit : optionsInit) 
            {
                std::string initKey = elementInit.first.as<std::string>();
                sol::table optionInit = elementInit.second.as<sol::table>();

                std::string icon   = optionInit["icon"].get<std::string>();
                std::string action = optionInit["action"].get<std::string>();

                std::string luaAction =
                    "operation_op = function() "
                    "mainWindow:toolbar():addButton('cancel', ':/icons/" + icon +
                    "', 'Current operation', function() luaInterface:operation():" + action +
                    "() end, '" + initKey + "') "
                    "end";

                _L.script(luaAction);
                optionsInitList.push_back(_L["operation_op"]);
            }

            // LINEOPERATIONS_init_pal - example key for operation options list
            std::string commandLine = _L[vkey]["command_line"].get<std::string>();
            mWindow->addOperationOptions(commandLine + key, optionsInitList);
        }
        else 
        {
            // default operation_options
            sol::table option = value.as<sol::table>();

            std::string icon   = option["icon"].get<std::string>();
            std::string action = option["action"].get<std::string>();

            std::string luaAction =
                "operation_op = function() "
                "mainWindow:toolbar():addButton('cancel', ':/icons/" + icon +
                "', 'Current operation', function() luaInterface:operation():" + action +
                "() end, '" + key + "') "
                "end";

            _L.script(luaAction);
            optionsList.push_back(_L["operation_op"]);
        }
    }

    // provide options list to mainWindow so it can run necessary function on runOperation
    if (!optionsList.empty()) 
    {
        std::string commandLine = _L[vkey]["command_line"].get<std::string>();
        mWindow->addOperationOptions(commandLine, optionsList);
    }

    _L["operation_op"] = sol::nil;
}

void OperationLoader::addContextMenuOperations(const std::string& vkey) {
    MainWindow* mWindow = static_cast<MainWindow*>(qmainWindow);
    lc::ui::ContextMenuManager* contextMenuManager = ContextMenuManager::GetContextMenuManager(mWindow->contextMenuManagerId());
    contextMenuManager->addOperation(vkey, groupNames[vkey]);
}

void OperationLoader::addContextTransitions(const std::string& vkey, const std::string& opkey) 
{
    MainWindow* mWindow = static_cast<MainWindow*>(qmainWindow);
    lc::ui::ContextMenuManager* contextMenuManager = ContextMenuManager::GetContextMenuManager(mWindow->contextMenuManagerId());
    sol::table functionNames = _L[vkey][opkey];

    for (const auto & element : functionNames) 
    {
        sol::table transitionTable = element.second;
        std::vector<std::string> transitionList;

        for (const auto & transitionFunction : transitionTable) 
        {
            transitionList.push_back(transitionFunction.second.as<std::string>());
        }

        contextMenuManager->addTransition(_L[vkey]["name"].get<std::string>(), element.first.as<std::string>(), transitionList);
    }
}
