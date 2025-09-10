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
    try
    {
        _L.script_file(luaPath + "/actions/operations.lua");
        _L.script_file(luaPath + "/createActions/createOperations.lua");
        std::cout << "Running scripts from: operations.lua and createOperation.lua\n";
    }
    catch(const sol::error & err)
    {
        std::cerr << "OperationLoader::loadLuaOperations: sol2 error occured when scripting files\n";
        std::cerr << err.what();
        throw;
    }

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

    try
    {
        sol::function luaToString = _L["tostring"];

        // loop through all global Lua variables
        for (const auto & globalEntry : globalTable) 
        {
            sol::object globalKey = globalEntry.first;
            sol::object globalValue = globalEntry.second;

            if (!globalKey.is<std::string>()) 
            {
                std::string globalKeyString = luaToString(globalKey);
                std::string globalValueString = luaToString(globalValue);
                std::cerr << "OperationLoader::loadLuaOperations: globalKey is not a string\n";
                std::cerr << "globalKey: " <<  globalKeyString << "\n";
                std::cerr << "globalEntry skipped:" << globalValueString << "\n";
                continue;
            }
            std::string globalName = globalKey.as<std::string>();

            // look for globals whose name contains "Operation" and is a table
            if (globalName.find("Operation") != std::string::npos && globalValue.is<sol::table>())
            {
                sol::table operationTable = globalValue.as<sol::table>();
                foundProperties[globalName] = {};

                // check for interesting properties in the operation table
                for (const auto & operationEntry : operationTable) 
                {
                    sol::object propertyKey = operationEntry.first;
                    if (!propertyKey.is<std::string>()) 
                    {
                        std::string propertyKeyString = luaToString(globalKey);
                        std::cerr << "OperationLoader::loadLuaOperations: propertyKey is not a string: " << propertyKeyString << "\n";
                        std::cerr << "Skipping this property\n";
                        continue;
                    }
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
    }
    catch(const sol::error & err)
    {
        std::cerr << "OperationLoader::loadLuaOperations: sol2 error occured when iterating through global table.\n";
        std::cerr << err.what();
        throw;
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

    // Debug / error support function
    sol::function luaToString = _L["tostring"];

    // insert elements into their respective sets
    for (auto & globalEntry : globalTable)
    {
        sol::object globalKey = globalEntry.first;
        sol::object globalValue = globalEntry.second;
        std::string globalKeyString = luaToString(globalKey);
        std::string globalValueString = luaToString(globalValue);
        if (!globalKey.is<std::string>()) 
        {
            /*
            std::cerr << "OperationLoader::getSetOfGroupElements: globalKey is not a string\n";
            std::cerr << "globalKey: " <<  globalKeyString << "\n";
            std::cerr << "globalEntry skipped, globalValue:" << globalValueString << "\n";
            */
            continue;
        }
        std::string globalName = globalKey.as<std::string>();

        if (globalName.find("Operation") == std::string::npos) 
        {
            /*
            std::cerr << "OperationLoader::getSetOfGroupElements: Can not find word: \"Operation\" in globalKey\n";
            std::cerr << "globalEntry skipped, globalValue:" << globalValueString << "\n";
            */
            continue;
        }

        if (globalName.find("Dim") != std::string::npos) groupElements["dimensionsGroupElements"].insert(globalName);
        else groupElements["creationGroupElements"].insert(globalName);
        std::cout << "OperationLoader::getSetOfGroupElements: OK. Operation categorized and stored. globalKey: " << globalKeyString << "\n";
    }
}

void OperationLoader::initializeOperation(const std::string& vkey)
{
    for (const std::string & opkey : foundProperties[vkey])
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

    try
    {

        auto createRunOp = [&](const std::string & globalKey, const std::string & init = "")
        {
            if (init.empty()) _L["run_op"] = _L.script("return function() run_basic_operation('" + globalKey + "') end");
            else _L["run_op"] = _L.script("return function() run_basic_operation('" + globalKey + "', '_init_" + init + "') end");
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
                try 
                {

                    sol::function luaToString = _L["tostring"];
                    std::string keyString = luaToString(pair.first);
                    /*
                    if(!pair.first.is<std::string>())
                    {
                        std::cerr << "OperationLoader::addOperationCommandLine: Key is not a string: " <<  keyString << "\n";
                    }
                    */

                    if(!pair.second.is<std::string>())
                    {
                        std::string valueString = luaToString(pair.second);
                        std::cerr << "OperationLoader::addOperationCommandLine: Value is not a string: " << valueString << "\n";
                    }


                    // Also should be safeguarded
                    std::string command = pair.second.as<std::string>();

                    // If the key is all digits, treat it as default init
                    bool keyIsDigits = std::all_of(keyString.begin(), keyString.end(),
                            [](unsigned char c){ return std::isdigit(c); });

                    if (keyIsDigits) 
                    {
                        // connect to default init function
                        createRunOp(globalKey);
                        cliCommand->addCommand(command.c_str(), _L["run_op"]);
                    } 
                    else 
                    {
                        // connect to provided init function
                        createRunOp(globalKey, command);
                        cliCommand->addCommand(keyString.c_str(), _L["run_op"]);
                    }
                }
                catch(const sol::error & err)
                {
                    std::cerr << "OperationLoader::addOperationCommandLine: error occured when iterating through operation table\n";
                    std::cerr << err.what();
                    throw;
                }
            }
        }
    }
    catch(const sol::error & err)
    {
        std::cerr << "OperationLoader::addOperationCommandLine: SOL2 error occured\n";
        std::cerr << err.what();
        throw;
    }
}

void OperationLoader::addOperationMenuAction(const std::string & operationName, const std::string & propertyName)
{
    MainWindow* mainWindow = static_cast<MainWindow*>(qmainWindow);
    try
    {
        sol::table menuActionsTable = _L[operationName][propertyName];

        for (const auto & menuActionEntry : menuActionsTable) 
        {
            sol::object luaKey = menuActionEntry.first;
            sol::object luaValue = menuActionEntry.second;

            // ignore non-string entries
            if (!luaKey.is<std::string>() || !luaValue.is<std::string>()) 
            {
                sol::function luaToString = _L["tostring"];
                std::string luaKeyString = luaToString(luaKey);
                std::string luaValueString = luaToString(luaValue);
                std::cerr << "OperationLoader::addOperationMenuAction: luaKey or luaValue are not a string.\n";
                std::cerr << "Skipping luaKey: " << luaKeyString << " luaValue: " << luaValueString << "\n";
                continue; 
            }

            std::string actionKey = luaKey.as<std::string>();
            std::string actionValue = luaValue.as<std::string>();

            if (actionKey == "default") _L.script("run_op = function() run_basic_operation('" + operationName + "') end");
            else _L.script("run_op = function() run_basic_operation('" + operationName + "', '_init_" + actionKey + "') end");

            mainWindow->connectMenuItem(actionValue, _L["run_op"]);
        }
    }

    catch(const sol::error & err)
    {
        std::cerr << "OperationLoader::addOperationMenuAction: sol2 error  occured\n";
        std::cerr << err.what();
        throw;
    }
}

void OperationLoader::addOperationIcon(const std::string& vkey, const std::string& opkey) {
    widgets::Toolbar* toolbar = static_cast<MainWindow*>(qmainWindow)->toolbar();
    std::string icon = _L[vkey][opkey].get<std::string>();
    std::string tooltip;

    // if description not provided, use operation name
    if (foundProperties[vkey].find("description") != foundProperties[vkey].end())
    {
        tooltip = _L[vkey]["description"].get<std::string>();
    }
    else 
    {
        tooltip = vkey.substr(0, vkey.find("Operation"));
    }

    std::string iconPath = ":/icons/" + icon;
    _L.script("run_op = function() run_basic_operation('" + vkey + "') end");

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
