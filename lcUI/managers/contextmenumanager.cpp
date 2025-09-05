#include "contextmenumanager.h"
#include "widgets/guiAPI/menuitem.h"

using namespace lc::ui;

std::map<MainWindow*, int> ContextMenuManager::instancesId;
std::map<int, ContextMenuManager*> ContextMenuManager::instances;
int ContextMenuManager::_instanceCount = 0;

ContextMenuManager::ContextMenuManager(lc::ui::MainWindow* mainWindowIn)
    :
    _L(mainWindowIn->luaInterface()->luaVM()),
    _mainWindow(mainWindowIn)
{}

ContextMenuManager* ContextMenuManager::GetContextMenuManager(lc::ui::MainWindow* mainWindowIn) {
    if (instancesId.find(mainWindowIn) == instancesId.end()) {
        instancesId[mainWindowIn] = _instanceCount;
        instances[_instanceCount] = new ContextMenuManager(mainWindowIn);
        _instanceCount++;
    }

    return instances[instancesId[mainWindowIn]];
}

ContextMenuManager* ContextMenuManager::GetContextMenuManager(int instanceIdIn) {
    if (instances.find(instanceIdIn) == instances.end()) {
        return nullptr;
    }

    return instances[instanceIdIn];
}

int ContextMenuManager::GetInstanceId(lc::ui::MainWindow* mainWindowIn) {
    if (instancesId.find(mainWindowIn) == instancesId.end()) {
        return -1;
    }

    return instancesId[mainWindowIn];
}

void ContextMenuManager::addOperation(const std::string& key, const std::string& groupName) {
    if (key == "CreateOperations" || key == "Operations") {
        return;
    }

    if (_operationMap.find(groupName) == _operationMap.end()) {
        _operationMap[groupName] = std::vector<std::string>();
    }

    _operationMap[groupName].push_back(key);
}

void ContextMenuManager::generateMenu(api::Menu* menu, std::vector<lc::entity::CADEntity_CSPtr> selectedEntities) {
    if (_mainWindow->cadMdiChild()->viewer()->operationActive()) {
        activeCommands(menu, selectedEntities);
    }else if (selectedEntities.size() > 0) {
        selectedCommands(menu, selectedEntities);
    }
    else {
        inactiveCommands(menu);
    }
}

void ContextMenuManager::activeCommands(api::Menu* menu, const std::vector<lc::entity::CADEntity_CSPtr>& selectedEntities) {
    // Cancel command
    api::MenuItem* cancelItem = new api::MenuItem("Cancel");
    _L.script("contextmenu_op = function() finish_operation() end");
    cancelItem->addCallback(_L["contextmenu_op"]);
    menu->addItem(cancelItem);

    operationContextCommands(menu, selectedEntities);

    // PAN command
    api::MenuItem* panItem = new api::MenuItem("PAN");
    panItem->setCheckable(true);
    _L.script("ispaused = mainWindow:currentOperation():isPaused()");
    panItem->setChecked(_L["ispaused"].get<bool>());
    _L.script("contextmenu_op = function(enablePan) mainWindow:currentOperation():pauseCreation(enablePan) end");
    panItem->addCheckedCallback(_L["contextmenu_op"]);
    menu->addItem(panItem);

    // Snap commands
    addSnapCommands(menu);
}

void ContextMenuManager::inactiveCommands(api::Menu* menu) {
    api::MenuItem* lastCommandItem = new api::MenuItem("Last Command");
    api::MenuItem* undoItem = new api::MenuItem("Undo");
    api::MenuItem* redoItem = new api::MenuItem("Redo");

    _L.script("contextmenu_op = function() mainWindow:runLastOperation() end");
    lastCommandItem->addCallback(_L["contextmenu_op"]);

    _L.script("contextmenu_op = function() mainWindow:undo() end");
    undoItem->addCallback(_L["contextmenu_op"]);

    _L.script("contextmenu_op = function() mainWindow:redo() end");
    redoItem->addCallback(_L["contextmenu_op"]);

    menu->addItem(lastCommandItem);
    menu->addItem(undoItem);
    menu->addItem(redoItem);

    // paste command
    api::MenuItem* pasteItem = new api::MenuItem("Paste");
    _L.script("contextmenu_op = function() mainWindow:pasteEvent() end");
    pasteItem->addCallback(_L["contextmenu_op"]);
    menu->addItem(pasteItem);

    // Select commands
    api::Menu* selectMenu = new api::Menu("Select");
    api::MenuItem* selectAllItem = new api::MenuItem("Select All");
    _L.script("contextmenu_op = function() mainWindow:selectAll() end");
    selectAllItem->addCallback(_L["contextmenu_op"]);

    selectMenu->addItem(selectAllItem);
    menu->addMenu(selectMenu);

    // Creation commands
    api::Menu* groupMenu = new api::Menu("Creation");
    for (const std::string& opName : _operationMap["Creation"]) {
        lc::ui::api::MenuItem* opItem = new lc::ui::api::MenuItem(cleanOperationName(opName).c_str());
        _L.script("contextmenu_op = function() run_basic_operation(" + opName + ") end");
        opItem->addCallback(_L["contextmenu_op"]);
        groupMenu->addItem(opItem);
    }

    menu->addMenu(groupMenu);

    // Dimension commands
    api::Menu* groupMenu2 = new api::Menu("Dimensions");
    for (const std::string& opName : _operationMap["Dimensions"]) {
        lc::ui::api::MenuItem* opItem = new lc::ui::api::MenuItem(cleanOperationName(opName).c_str());
        _L.script("contextmenu_op = function() run_basic_operation(" + opName + ") end");
        opItem->addCallback(_L["contextmenu_op"]);
        groupMenu2->addItem(opItem);
    }

    menu->addMenu(groupMenu2);
}

void ContextMenuManager::selectedCommands(api::Menu* menu, const std::vector<lc::entity::CADEntity_CSPtr>& selectedEntities) {
    // Copy Command
    api::MenuItem* copyItem = new api::MenuItem("Copy");
    _L.script("contextmenu_op = function() mainWindow:copySelectedEntities(mainWindow:cadMdiChild():selection()) end");
    copyItem->addCallback(_L["contextmenu_op"]);
    menu->addItem(copyItem);

    // Modify commands
    api::Menu* groupMenu = new api::Menu("Modify");
    for (const std::string& opName : _operationMap["Modify"]) {
        lc::ui::api::MenuItem* opItem = new lc::ui::api::MenuItem(cleanOperationName(opName).c_str());
        _L.script("contextmenu_op = function() run_basic_operation(" + opName + ") end");
        opItem->addCallback(_L["contextmenu_op"]);
        groupMenu->addItem(opItem);
    }
    menu->addMenu(groupMenu);

    // Select commands
    api::Menu* selectMenu = new api::Menu("Select");
    api::MenuItem* selectAllItem = new api::MenuItem("Select All");
    api::MenuItem* selectNoneItem = new api::MenuItem("Select None");
    api::MenuItem* selectInverted = new api::MenuItem("Invert Selection");

    _L.script("contextmenu_op = function() mainWindow:selectAll() end");
    selectAllItem->addCallback(_L["contextmenu_op"]);

    _L.script("contextmenu_op = function() mainWindow:selectNone() end");
    selectNoneItem->addCallback(_L["contextmenu_op"]);

    _L.script("contextmenu_op = function() mainWindow:invertSelection() end");
    selectInverted->addCallback(_L["contextmenu_op"]);

    selectMenu->addItem(selectAllItem);
    selectMenu->addItem(selectNoneItem);
    selectMenu->addItem(selectInverted);
    menu->addMenu(selectMenu);
}

std::string ContextMenuManager::cleanOperationName(const std::string& opName) const {
    std::string result = opName;
    result = result.substr(0, result.find("Operation"));
    return result;
}

std::string ContextMenuManager::cleanTransitionName(const std::string& transitionName) const {
    std::string result;
    int n = transitionName.size();
    int last = 0;

    for (int i = 1; i < n; i++) {
        if (transitionName[i] >= 'A' && transitionName[i] <= 'Z') {
            result += transitionName.substr(last, i-last) + " ";
            last = i;
        }
    }

    result += transitionName.substr(last);
    return result;
}

void ContextMenuManager::addSnapCommands(api::Menu* menu) {
    api::Menu* snapMenu = new api::Menu("Snap");
    api::MenuItem* snapGridItem = new api::MenuItem("Grid");
    api::MenuItem* snapIntersectionItem = new api::MenuItem("Intersection");
    api::MenuItem* snapMiddleItem = new api::MenuItem("Middle");
    api::MenuItem* snapEntityItem = new api::MenuItem("Entity");

    snapGridItem->setCheckable(true);
    snapIntersectionItem->setCheckable(true);
    snapMiddleItem->setCheckable(true);
    snapEntityItem->setCheckable(true);

    // check current state of snaps
    const viewer::manager::SnapManagerImpl_SPtr snapManager = _mainWindow->cadMdiChild()->getSnapManager();

    snapGridItem->setChecked(snapManager->isGridSnappable());
    snapIntersectionItem->setChecked(snapManager->isIntersectionsSnappable());
    lc::SimpleSnapConstrain snapConstrain = snapManager->snapConstrain();
    snapMiddleItem->setChecked(snapConstrain.hasConstrain(lc::SimpleSnapConstrain::LOGICAL));
    snapEntityItem->setChecked(snapConstrain.hasConstrain(lc::SimpleSnapConstrain::ON_ENTITY));

    // add callbacks
    _L.script("contextmenu_op = function(enabled) mainWindow:cadMdiChild():getSnapManager():setGridSnappable(enabled) mainWindow:toolbar():updateSnapButtons(mainWindow:cadMdiChild():getSnapManager()) end");
    snapGridItem->addCheckedCallback(_L["contextmenu_op"]);

    _L.script("contextmenu_op = function(enabled) mainWindow:cadMdiChild():getSnapManager():setIntersectionSnappable(enabled) mainWindow:toolbar():updateSnapButtons(mainWindow:cadMdiChild():getSnapManager()) end");
    snapIntersectionItem->addCheckedCallback(_L["contextmenu_op"]);

    _L.script("contextmenu_op = function(enabled) mainWindow:cadMdiChild():getSnapManager():setMiddleSnappable(enabled) mainWindow:toolbar():updateSnapButtons(mainWindow:cadMdiChild():getSnapManager()) end");
    snapMiddleItem->addCheckedCallback(_L["contextmenu_op"]);

    _L.script("contextmenu_op = function(enabled) mainWindow:cadMdiChild():getSnapManager():setEntitySnappable(enabled) mainWindow:toolbar():updateSnapButtons(mainWindow:cadMdiChild():getSnapManager()) end");
    snapEntityItem->addCheckedCallback(_L["contextmenu_op"]);

    snapMenu->addItem(snapGridItem);
    snapMenu->addItem(snapIntersectionItem);
    snapMenu->addItem(snapMiddleItem);
    snapMenu->addItem(snapEntityItem);
    menu->addMenu(snapMenu);
}


void ContextMenuManager::operationContextCommands(api::Menu* menu, const std::vector<lc::entity::CADEntity_CSPtr>& selectedEntities) {
    std::string entityName = _mainWindow->lastOperationName();
    sol::table currentOp = _mainWindow->currentOperation();

    if (_transitionMap.find(entityName) != _transitionMap.end()) 
    {
        sol::object stepName = currentOp["step"];

        if (stepName.valid() && stepName.is<std::string>()) 
        {
            std::string step = stepName.as<std::string>();

            if (_transitionMap[entityName].find(step) != _transitionMap[entityName].end()) 
            {
                std::vector<std::string> transitions = _transitionMap[entityName][step];

                for (const std::string & transition : transitions) 
                {
                    api::MenuItem* menuItem = new api::MenuItem(cleanTransitionName(transition).c_str());
                    _L.script("contextmenu_op = function() mainWindow:currentOperation().step = '" + transition + "' end");
                    menuItem->addCallback(_L["contextmenu_op"]);
                    menu->addItem(menuItem);
                }
            }
        }
    }

    if (currentOp.valid()) {
        sol::function otherOptions = currentOp["contextMenuOptions"];
        if (otherOptions.valid()) otherOptions(currentOp, menu);
    }
}

void ContextMenuManager::addTransition(std::string entityName, std::string fname, std::vector<std::string> transitionList) {
    if (_transitionMap.find(entityName) == _transitionMap.end()) {
        _transitionMap[entityName] = std::map<std::string, std::vector<std::string>>();
    }

    _transitionMap[entityName][fname] = transitionList;
}
