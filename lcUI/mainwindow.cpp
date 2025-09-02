#include "mainwindow.h"

#include "ui_mainwindow.h"
#include "dialogs/aboutdialog.h"
#include "dialogs/textdialog.h"
#include "windowmanager.h"
#include "propertyeditor.h"
#include "managers/contextmenumanager.h"

#include <QStandardPaths>

#include "widgets/guiAPI/coordinategui.h"
#include "widgets/guiAPI/entitygui.h"
#include "widgets/guiAPI/buttongui.h"

using namespace lc::ui;

MainWindow::MainWindow()
    :
    ui(new Ui::MainWindow),
    linePatternSelect(&_cadMdiChild, this, true, true),
    lineWidthSelect(_cadMdiChild.metaInfoManager(), this, true, true),
    colorSelect(_cadMdiChild.metaInfoManager(), this, true, true),
    _cliCommand(this),
    _toolbar(&_luaInterface, this),
    _layers(nullptr, this),
    _copyManager(&_cadMdiChild)
{
    ContextMenuManager::GetContextMenuManager(this);
    _contextMenuManagerId = ContextMenuManager::GetInstanceId(this);

    ui->setupUi(this);
    // new document and set mainwindow attributes
    _cadMdiChild.newDocument();
    setWindowTitle(QObject::tr("LibreCAD"));
    setUnifiedTitleAndToolBarOnMac(true);
    setCentralWidget(&_cadMdiChild);

    _layers.setMdiChild(&_cadMdiChild);

    // add widgets to correct positions
    addDockWidget(Qt::RightDockWidgetArea, &_layers);
    addDockWidget(Qt::BottomDockWidgetArea, &_cliCommand);
    addDockWidget(Qt::TopDockWidgetArea, &_toolbar);

    _toolbar.initializeToolbar(&linePatternSelect, &lineWidthSelect, &colorSelect);
    _cadMdiChild.viewer()->autoScale();

    initMenuAPI();

    // connect required signals and slots
    ConnectInputEvents();

    // open qt bridge and run lua scripts
    _luaInterface.initLua(this);

    _toolbar.addSnapOptions();

    addOtherMenus();

    _toolbar.generateButtonsMap();
    readUiSettings();

    _cadMdiChild.viewer()->setContextMenuManagerId(_contextMenuManagerId);

    PropertyEditor* propertyEditor = PropertyEditor::GetPropertyEditor(this);
    this->addDockWidget(Qt::BottomDockWidgetArea, propertyEditor);

    /* Shortcuts */
    copyShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_C), this);
    pasteShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_V), this);

    connect(copyShortcut, &QShortcut::activated, [this]() { this->copySelectedEntities(this->cadMdiChild()->selection()); });
    connect(pasteShortcut, &QShortcut::activated, this, &MainWindow::pasteEvent);

    this->resizeDocks({ &_cliCommand, propertyEditor }, { 65, 35 }, Qt::Horizontal);
}

MainWindow::~MainWindow()
{
    WindowManager::removeWindow(this);
    delete ui;
}

void MainWindow::addOtherMenus() {
    // add lua script
    sol::state & luaVM = _luaInterface.luaVM();
    luaVM.script("run_luascript = function() lc.LuaScript(mainWindow):show() end");
    luaVM.script("run_customizetoolbar = function() mainWindow:runCustomizeToolbar() end");

    luaVM.set_function("run_aboutdialog", [&] {
        auto aboutDialog = new dialog::AboutDialog(this);
        aboutDialog->show();
        });
    luaVM.set_function("run_textdialog", [&] {
        auto textDialog = new dialog::TextDialog(this, this);
        textDialog->show();
        });

    api::Menu* luaMenu = addMenu("Lua");
    luaMenu->addItem("Run script", luaVM["run_luascript"]);
    luaMenu->addItem("Customize Toolbar", luaVM["run_customizetoolbar"]);

    api::Menu* viewMenu = addMenu("View");
    luaVM.script("changeLayout = function() mainWindow:changeDockLayout(1) end");
    viewMenu->addItem("Default Layout 1", luaVM["changeLayout"]);
    luaVM.script("changeLayout = function() mainWindow:changeDockLayout(2) end");
    viewMenu->addItem("Default Layout 2", luaVM["changeLayout"]);
    luaVM.script("changeLayout = function() mainWindow:changeDockLayout(3) end");
    viewMenu->addItem("Default Layout 3", luaVM["changeLayout"]);
    luaVM.script("changeLayout = function() mainWindow:loadDockLayout() end");
    viewMenu->addItem("Load Dock Layout", luaVM["changeLayout"]);
    luaVM.script("changeLayout = function() mainWindow:saveDockLayout() end");
    viewMenu->addItem("Save Dock Layout", luaVM["changeLayout"]);

    api::Menu* aboutMenu = addMenu("About");
    aboutMenu->addItem("About", luaVM["run_aboutdialog"]);

    api::Menu* textMenu = menuByName("Create")->menuByName("Text");
    if (textMenu != nullptr) {
        textMenu->addItem("Text Dialog", luaVM["run_textdialog"]);
    }
}

void MainWindow::runOperation(sol::table operation, const std::string & init_method)
{
    _cliCommand.setFocus();
    _luaInterface.finishOperation();
    _cadMdiChild.viewer()->setOperationActive(true);
    sol::state & luaVM = _luaInterface.luaVM();

    // if current operation had extra operation _toolbar icons, add them
    if(operation["operation_options"].valid()) 
    {
        std::string commandLine = operation["command_line"];

        // try first key, then fallback to commandLine
        auto it = operation_options.find(commandLine + init_method);
        if(it == operation_options.end()) 
        {
            it = operation_options.find(commandLine);
        }

        if(it != operation_options.end()) 
        {
            for(sol::function& optionCall : it->second) optionCall();
        }
    }

    // add toolbar cancel button
    luaVM.script(R"(finish_op = function() finish_operation() end)");
    _toolbar.addButton("", ":/icons/quit.svg", "Current operation", luaVM["finish_op"], "Cancel");
    luaVM["finish_op"] = sol::lua_nil;

    // call operation (__call metamethod) and run init
    sol::function operationCall = operation;
    sol::table op = operationCall();
    _luaInterface.setOperation(op);

    sol::function initFunction;
    if(init_method.empty()) initFunction = op["_init_default"];
    else initFunction = op[init_method];
    if(initFunction.valid()) initFunction(op);  // pass self

    _oldOperation = operation;
    _oldOpInitMethod = init_method;
}

void MainWindow::addOperationOptions(std::string operation, std::vector<sol::function> options) {
    operation_options[operation] = options;
}

void MainWindow::operationFinished() {
    // remove operation group
    _toolbar.removeGroupByName("Current operation");
    _cadMdiChild.viewer()->setOperationActive(false);
}

lc::ui::widgets::CliCommand* MainWindow::cliCommand() {
    return &_cliCommand;
}

lc::ui::CadMdiChild* MainWindow::cadMdiChild() {
    return &_cadMdiChild;
}

lc::ui::widgets::Toolbar* MainWindow::toolbar() {
    return &_toolbar;
}

lc::ui::widgets::Layers* MainWindow::layers() {
    return &_layers;
}

lc::ui::LuaInterface* MainWindow::luaInterface() {
    return &_luaInterface;
}

int MainWindow::contextMenuManagerId() {
    return _contextMenuManagerId;
}

void MainWindow::ConnectInputEvents()
{
    // CadMdiChild connections, main window should not know about proxy
    QObject::connect(&_cadMdiChild, &CadMdiChild::mousePressEvent, this, &MainWindow::triggerMousePressed);
    QObject::connect(&_cadMdiChild, &CadMdiChild::mouseReleaseEvent, this, &MainWindow::triggerMouseReleased);
    QObject::connect(&_cadMdiChild, &CadMdiChild::mouseMoveEvent, this, &MainWindow::triggerMouseMoved);
    QObject::connect(&_cadMdiChild, &CadMdiChild::selectionChangeEvent, this, &MainWindow::triggerSelectionChanged);
    QObject::connect(&_cadMdiChild, &CadMdiChild::selectionChangeEvent, this, &MainWindow::selectionChanged);
    QObject::connect(&_cadMdiChild, &CadMdiChild::keyPressEventx, this, &MainWindow::triggerKeyPressed);
    QObject::connect(&_cadMdiChild, &CadMdiChild::keyPressed, &_cliCommand, &widgets::CliCommand::onKeyPressed);

    // CliCommand connections
    QObject::connect(&_cliCommand, &widgets::CliCommand::coordinateEntered, this, &MainWindow::triggerCoordinateEntered);
    QObject::connect(&_cliCommand, &widgets::CliCommand::relativeCoordinateEntered, this, &MainWindow::triggerRelativeCoordinateEntered);
    QObject::connect(&_cliCommand, &widgets::CliCommand::numberEntered, this, &MainWindow::triggerNumberEntered);
    QObject::connect(&_cliCommand, &widgets::CliCommand::textEntered, this, &MainWindow::triggerTextEntered);
    QObject::connect(&_cliCommand, &widgets::CliCommand::finishOperation, this, &MainWindow::triggerFinishOperation);
    QObject::connect(&_cliCommand, &widgets::CliCommand::commandEntered, this, &MainWindow::triggerCommandEntered);

    // Layers to select tools connections
    QObject::connect(&_layers, &widgets::Layers::layerChanged, &linePatternSelect, &widgets::LinePatternSelect::onLayerChanged);
    QObject::connect(&_layers, &widgets::Layers::layerChanged, &lineWidthSelect, &widgets::LineWidthSelect::onLayerChanged);
    QObject::connect(&_layers, &widgets::Layers::layerChanged, &colorSelect, &widgets::ColorSelect::onLayerChanged);

    // Other
    QObject::connect(this, &MainWindow::point, this, &MainWindow::triggerPoint);
    QObject::connect(findMenuItemByObjectName("actionExit"), &QAction::triggered, this, &MainWindow::close);

    // File connections
    QObject::connect(findMenuItemByObjectName("actionNew"), &QAction::triggered, this, &MainWindow::newFile);
    QObject::connect(findMenuItemByObjectName("actionOpen"), &QAction::triggered, this, &MainWindow::openFile);
    QObject::connect(findMenuItemByObjectName("actionSave_2"), &QAction::triggered, &_cadMdiChild, &CadMdiChild::saveFile);
    QObject::connect(findMenuItemByObjectName("actionSave_As"), &QAction::triggered, &_cadMdiChild, &CadMdiChild::saveAsFile);

    // Edit connections
    QObject::connect(findMenuItemByObjectName("actionUndo"), &QAction::triggered, this, &MainWindow::undo);
    QObject::connect(findMenuItemByObjectName("actionRedo"), &QAction::triggered, this, &MainWindow::redo);
    QObject::connect(findMenuItemByObjectName("actionSelect_All"), &QAction::triggered, this, &MainWindow::selectAll);
    QObject::connect(findMenuItemByObjectName("actionSelect_None"), &QAction::triggered, this, &MainWindow::selectNone);
    QObject::connect(findMenuItemByObjectName("actionInvert_Selection"), &QAction::triggered, this, &MainWindow::invertSelection);
    QObject::connect(findMenuItemByObjectName("actionClear_Undoable_Stack"), &QAction::triggered, this, &MainWindow::clearUndoableStack);
    QObject::connect(findMenuItemByObjectName("actionAuto_Scale"), &QAction::triggered, this, &MainWindow::autoScale);
}

void MainWindow::runLastOperation() {
    if (_oldOperation.valid()) {
        runOperation(_oldOperation, _oldOpInitMethod);
    }
}

/* Menu functions */

void MainWindow::connectMenuItem(const std::string& itemName, sol::function callback)
{
    lc::ui::api::MenuItem* menuItem = findMenuItemByObjectName(itemName.c_str());
    menuItem->addCallback(callback);
}

void MainWindow::initMenuAPI() {
    QList<QMenu*> allMenus = menuBar()->findChildren<QMenu*>(QString(), Qt::FindDirectChildrenOnly);

    int menuPosition = 0;
    for (QMenu* current_menu : allMenus)
    {
        api::Menu* menu = static_cast<api::Menu*>(current_menu);
        this->menuBar()->addAction(menu->menuAction());
        menuMap[menu->title()] = menu;
        menu->updatePositionVariable(menuPosition);
        menuPosition++;

        QList<QMenu*> allMenusOfCurrentMenu = menu->findChildren<QMenu*>(QString(), Qt::FindDirectChildrenOnly);

        for (QMenu* currentChildMenu : allMenusOfCurrentMenu)
        {
            if (currentChildMenu != nullptr) {
                menu->addAction(currentChildMenu->menuAction());
            }
        }

        addActionsAsMenuItem(menu);
        fixMenuPositioning(menu);
    }
}

void MainWindow::addActionsAsMenuItem(lc::ui::api::Menu* menu) {
    QList<QAction*> actions = menu->actions();
    QList<QAction*> menuItemsToBeAdded;

    for (QAction* action : actions)
    {
        if (action->menu()) {
            addActionsAsMenuItem(static_cast<api::Menu*>(action->menu()));
        }
        else if (action->isSeparator()) {
            QAction* sep = new QAction();
            sep->setSeparator(true);
            menu->removeAction(action);
            menuItemsToBeAdded.push_back(sep);
        }
        else {
            lc::ui::api::MenuItem* newMenuItem = new lc::ui::api::MenuItem(action->text().toStdString().c_str());

            QString oldObjectName = action->objectName();
            action->setObjectName(oldObjectName + QString("changed"));
            newMenuItem->setObjectName(oldObjectName);

            menu->removeAction(action);
            menuItemsToBeAdded.push_back(newMenuItem);
        }
    }

    for (QAction* it : menuItemsToBeAdded)
    {
        menu->addAction(it);
    }

    // reorder menu to appear below
    for (QAction* action : actions)
    {
        if (action->menu()) {
            menu->insertMenu(menuItemsToBeAdded.last(), action->menu());
        }
    }
}

void MainWindow::fixMenuPositioning(lc::ui::api::Menu* menu) {
    QList<QAction*> actions = menu->actions();

    int pos = 0;
    for (QAction* action : actions) {
        lc::ui::api::Menu* actionMenu = qobject_cast<lc::ui::api::Menu*>(action->menu());
        lc::ui::api::MenuItem* actionItem = qobject_cast<lc::ui::api::MenuItem*>(action);

        if (actionMenu != nullptr) {
            actionMenu->updatePositionVariable(pos);
        }

        if (actionItem != nullptr) {
            actionItem->updatePositionVariable(pos);
        }
        pos++;
    }
}

/* Menu Lua GUI API */

lc::ui::api::MenuItem* MainWindow::findMenuItem(std::string label) {
    return findMenuItemBy(label, true);
}

lc::ui::api::MenuItem* MainWindow::findMenuItemByObjectName(std::string objectName) {
    return findMenuItemBy(objectName, false);
}

api::MenuItem* MainWindow::findMenuItemBy(std::string objectName, bool searchByLabel) {
    QString name = QString(objectName.c_str());

    for (auto key : menuMap.keys())
    {
        api::MenuItem* foundIt = findMenuItemRecur(menuMap[key], name, searchByLabel);

        if (foundIt != nullptr) {
            return foundIt;
        }
    }

    return nullptr;
}

api::MenuItem* MainWindow::findMenuItemRecur(QMenu* menu, QString objectName, bool searchByLabel) {
    QList<QAction*> actions = menu->actions();

    for (QAction* action : actions)
    {
        if (action->menu()) {
            api::MenuItem* foundIt = findMenuItemRecur(action->menu(), objectName, searchByLabel);
            if (foundIt != nullptr) {
                return foundIt;
            }
        }
        else if (!action->isSeparator()) {
            if (searchByLabel) {
                if (objectName == action->text()) {
                    return static_cast<api::MenuItem*>(action);
                }
            }
            else {
                if (objectName == action->objectName()) {
                    return static_cast<api::MenuItem*>(action);
                }
            }
        }
    }

    return nullptr;
}

bool MainWindow::checkForMenuOfSameLabel(const std::string& label) {
    QList<QString> keys = menuMap.keys();

    for (QString key : keys)
    {
        std::string keystr = key.toStdString();
        keystr.erase(std::remove(keystr.begin(), keystr.end(), '&'), keystr.end());

        if (keystr == label) {
            return true;
        }
    }

    return false;
}

api::Menu* MainWindow::addMenu(const std::string& menuName) {
    if (checkForMenuOfSameLabel(menuName)) {
        return nullptr;
    }

    api::Menu* newMenu = new api::Menu(menuName.c_str());
    addMenu(newMenu);

    return newMenu;
}

void MainWindow::addMenu(lc::ui::api::Menu* menu) {
    if (checkForMenuOfSameLabel(menu->label())) {
        return;
    }

    menuMap[menu->title()] = menu;
    menuBar()->addMenu(menu);

    QList<QAction*> menuList = menuBar()->actions();
    menu->updatePositionVariable(menuList.size() - 1);
}

api::Menu* MainWindow::menuByName(const std::string& menuName) {
    QList<QString> keys = menuMap.keys();

    for (QString key : keys)
    {
        std::string keystr = key.toStdString();
        keystr.erase(std::remove(keystr.begin(), keystr.end(), '&'), keystr.end());

        if (keystr == menuName) {
            return menuMap[key];
        }
    }

    return nullptr;
}

lc::ui::api::Menu* MainWindow::menuByPosition(int pos) {
    QList<QAction*> menuList = menuBar()->actions();

    if (pos < 0 || pos >= menuList.size()) {
        return nullptr;
    }

    return dynamic_cast<lc::ui::api::Menu*>(menuList[pos]->menu());
}

void MainWindow::removeFromMenuMap(std::string menuName) {
    auto iter = menuMap.begin();
    QString key;
    for(; iter != menuMap.end(); ++iter)
    {
        std::string keystr = iter.key().toStdString();
        keystr.erase(std::remove(keystr.begin(), keystr.end(), '&'), keystr.end());

        if (keystr == menuName) {
            key = iter.key();
            break;
        }
    }

    menuMap.remove(key);
}

void MainWindow::removeMenu(const char* menuLabel) {
    lc::ui::api::Menu* menuremove = menuByName(menuLabel);
    if (menuremove != nullptr) {
        menuremove->remove();
    }
}

void MainWindow::removeMenu(int position) {
    lc::ui::api::Menu* menuremove = menuByPosition(position);
    if (menuremove != nullptr) {
        menuremove->remove();
    }
}

/* Trigger slots */

void MainWindow::triggerMousePressed()
{
    lc::geo::Coordinate cursorPos = _cadMdiChild.cursor()->position();
    sol::state & luaVM = _luaInterface.luaVM();
    luaVM["mousePressed"] = luaVM.create_table();
    sol::table mousePressed = luaVM["mousePressed"];
    mousePressed["position"] = cursorPos;
    mousePressed["widget"] = &_cadMdiChild;
    _luaInterface.triggerEvent("point", mousePressed);

    emit point(cursorPos);
}

void MainWindow::triggerMouseReleased()
{
    sol::state & luaVM = _luaInterface.luaVM();
    luaVM["mouseRelease"] = luaVM.create_table();
    sol::table mouseRelease = luaVM["mouseRelease"];
    mouseRelease["widget"] = &_cadMdiChild;
    _luaInterface.triggerEvent("mouseRelease", mouseRelease);
}

void MainWindow::triggerSelectionChanged()
{
    sol::state & luaVM = _luaInterface.luaVM();
    luaVM["selectionChanged"] = luaVM.create_table();
    sol::table selectionChanged = luaVM["selectionChanged"];
    selectionChanged["widget"] = &_cadMdiChild;
    _luaInterface.triggerEvent("selectionChanged", selectionChanged);
}

void MainWindow::triggerMouseMoved()
{
    lc::geo::Coordinate cursorPos = _cadMdiChild.cursor()->position();
    sol::state & luaVM = _luaInterface.luaVM();
    luaVM["mouseMove"] = luaVM.create_table();
    sol::table mouseMove = luaVM["mouseMove"];
    mouseMove["position"] = cursorPos;
    mouseMove["widget"] = &_cadMdiChild;
    _luaInterface.triggerEvent("mouseMove", mouseMove);
}

void MainWindow::triggerKeyPressed(int key)
{
    sol::state & luaVM = _luaInterface.luaVM();
    if (key == Qt::Key_Escape)
    {
        // run finish operation
        _luaInterface.triggerEvent("finishOperation", luaVM.globals()); // closest equivalent to Kaguya version
        // This line should be sufficient here, sending empty table - nice to test later.
        // _luaInterface.triggerEvent("finishOperation", luaVM.create_table()); 
    }
    else
    {
        luaVM["keyEvent"] = luaVM.create_table();
        sol::table keyEvent = luaVM["keyEvent"];
        keyEvent["key"] = key;
        keyEvent["widget"] = &_cadMdiChild;
        _luaInterface.triggerEvent("keyPressed", keyEvent);
    }
}

void MainWindow::triggerCoordinateEntered(lc::geo::Coordinate coordinate)
{
    sol::state & luaVM = _luaInterface.luaVM();
    luaVM["coordinateEntered"] = luaVM.create_table();
    sol::table coordinateEntered = luaVM["coordinateEntered"];
    coordinateEntered["position"] = coordinate;
    coordinateEntered["widget"] = &_cadMdiChild;
    _luaInterface.triggerEvent("point", coordinateEntered);

    emit point(coordinate);
}

void MainWindow::triggerRelativeCoordinateEntered(lc::geo::Coordinate coordinate)
{
    sol::state & luaVM = _luaInterface.luaVM();
    luaVM["relCoordinateEntered"] = luaVM.create_table();
    sol::table relCoordinateEntered = luaVM["relCoordinateEntered"];
    relCoordinateEntered["position"] = lastPoint + coordinate;
    relCoordinateEntered["widget"] = &_cadMdiChild;
    _luaInterface.triggerEvent("point", relCoordinateEntered);

    emit point(lastPoint + coordinate);
}

void MainWindow::triggerNumberEntered(double number)
{
    sol::state & luaVM = _luaInterface.luaVM();
    luaVM["numberEntered"] = luaVM.create_table();
    sol::table numberEntered = luaVM["numberEntered"];
    numberEntered["number"] = number;
    numberEntered["widget"] = &_cadMdiChild;
    _luaInterface.triggerEvent("number", numberEntered);
}

void MainWindow::triggerTextEntered(QString text)
{
    sol::state & luaVM = _luaInterface.luaVM();
    luaVM["textEntered"] = luaVM.create_table();
    sol::table textEntered = luaVM["textEntered"];
    textEntered["text"] = text.toStdString();
    textEntered["widget"] = &_cadMdiChild;
    _luaInterface.triggerEvent("text", textEntered);
}

void MainWindow::triggerFinishOperation()
{
    sol::state & luaVM = _luaInterface.luaVM();
    _luaInterface.triggerEvent("operationFinished", luaVM.globals());
    _luaInterface.triggerEvent("finishOperation", luaVM.globals());
}

void MainWindow::triggerCommandEntered(QString command)
{
    _cliCommand.runCommand(command.toStdString().c_str());
}

void MainWindow::triggerPoint(lc::geo::Coordinate coordinate)
{
    lastPoint = coordinate;
}

void MainWindow::newFile()
{
    /*
        TODO : Ask user if he wishes to save the file before replacing current window with new file
    */

    WindowManager::newFile(this);
}

void MainWindow::openFile()
{
    WindowManager::openFile();
}

// Edit slots
void MainWindow::undo()
{
    _cadMdiChild.undoManager()->undo();
    _cadMdiChild.viewer()->update();
}

void MainWindow::clearUndoableStack()
{
    _cadMdiChild.undoManager()->removeUndoables();
}

void MainWindow::redo()
{
    _cadMdiChild.undoManager()->redo();
    _cadMdiChild.viewer()->update();
}

void MainWindow::selectAll()
{
    _cadMdiChild.viewer()->docCanvas()->selectAll();
    _cadMdiChild.viewer()->update();
}

void MainWindow::selectNone()
{
    _cadMdiChild.viewer()->docCanvas()->removeSelection();
    _cadMdiChild.viewer()->update();
}

void MainWindow::invertSelection()
{
    _cadMdiChild.viewer()->docCanvas()->inverseSelection();
    _cadMdiChild.viewer()->update();
}

void MainWindow::autoScale() {
    _cadMdiChild.viewer()->autoScale();
    _cadMdiChild.viewer()->update();
};

void MainWindow::runCustomizeToolbar() {
    _customizeToolbar = new widgets::CustomizeToolbar(toolbar());
    connect(_customizeToolbar, &widgets::CustomizeToolbar::customizeWidgetClosed, this, &MainWindow::writeSettings);
    connect(_customizeToolbar, &widgets::CustomizeToolbar::defaultSettingsLoad, this, &MainWindow::loadDefaultSettings);

    _customizeToolbar->show();
}

void MainWindow::writeSettings() {
    _uiSettings.writeSettings(_customizeToolbar);
}

void MainWindow::readUiSettings() {
    _customizeToolbar = new widgets::CustomizeToolbar(toolbar());
    _customizeToolbar->setCloseMode(widgets::CustomizeToolbar::CloseMode::Save);
    _uiSettings.readSettings(_customizeToolbar);
    _customizeToolbar->close();
}

void MainWindow::loadDefaultSettings() {
    _uiSettings.readSettings(_customizeToolbar, true);
}

void MainWindow::selectionChanged() {
    std::vector<lc::entity::CADEntity_CSPtr> selectedEntities = _cadMdiChild.selection();
    PropertyEditor* propertyEditor = PropertyEditor::GetPropertyEditor(this);

    propertyEditor->clear(selectedEntities);

    for (lc::entity::CADEntity_CSPtr selectedEntity : selectedEntities) {
        propertyEditor->addEntity(selectedEntity);
    }

    if (selectedEntities.size() == 0) {
        propertyEditor->hide();
    }
    else {
        propertyEditor->show();
    }
}

std::string MainWindow::lastOperationName() {
    try
    {
        std::string name = _oldOperation["name"];
        return name;
    }
    catch(const sol::error & err)
    {
        std::cerr << "MainWindow: Can not retrive lastOperationName\n" << err.what() << "\n";
        return "";
    }
}

sol::table MainWindow::currentOperation() {
    return _luaInterface.operation();
}

void MainWindow::changeDockLayout(int i) {
    if (i == 1) {
        addDockWidget(Qt::RightDockWidgetArea, &_layers);
        addDockWidget(Qt::BottomDockWidgetArea, &_cliCommand);
        addDockWidget(Qt::TopDockWidgetArea, &_toolbar);
        PropertyEditor* propertyEditor = PropertyEditor::GetPropertyEditor(this);
        addDockWidget(Qt::BottomDockWidgetArea, propertyEditor);
        resizeDocks({ &_cliCommand, propertyEditor }, { 65, 35 }, Qt::Horizontal);
    }

    if (i == 2) {
        addDockWidget(Qt::LeftDockWidgetArea, &_layers);
        addDockWidget(Qt::BottomDockWidgetArea, &_cliCommand);
        addDockWidget(Qt::TopDockWidgetArea, &_toolbar);
        PropertyEditor* propertyEditor = PropertyEditor::GetPropertyEditor(this);
        addDockWidget(Qt::RightDockWidgetArea, propertyEditor);
    }

    if (i == 3) {
        addDockWidget(Qt::LeftDockWidgetArea, &_layers);
        addDockWidget(Qt::BottomDockWidgetArea, &_cliCommand);
        addDockWidget(Qt::TopDockWidgetArea, &_toolbar);
        PropertyEditor* propertyEditor = PropertyEditor::GetPropertyEditor(this);
        addDockWidget(Qt::BottomDockWidgetArea, propertyEditor);
    }
}

void MainWindow::copySelectedEntities(const std::vector<lc::entity::CADEntity_CSPtr>& cadEntities) {
    _copyManager.copyEntitiesToClipboard(cadEntities);
}

void MainWindow::pasteEvent() {
    _copyManager.pasteEvent();
}

void MainWindow::saveDockLayout() {
    int layersPos = this->dockWidgetArea(&_layers);
    int cliCommandPos = this->dockWidgetArea(&_cliCommand);
    int toolbarPos = this->dockWidgetArea(&_toolbar);
    PropertyEditor* propertyEditor = PropertyEditor::GetPropertyEditor(this);
    int propertyEditorPos = this->dockWidgetArea(propertyEditor);

    std::map<std::string, int> positions = {
        {"Toolbar", toolbarPos},
        {"CliCommand", cliCommandPos},
        {"Layers", layersPos},
        {"PropertyEditor", propertyEditorPos}
    };

    std::map<std::string, int> widths = {
        {"Toolbar", _toolbar.width()},
        {"CliCommand", _cliCommand.width()},
        {"Layers", _layers.width()},
        {"PropertyEditor", propertyEditor->width()}
    };

    std::map<int, int> posCount;
    std::map<int, int> posWidth;
    for (auto iter = positions.begin(); iter != positions.end(); ++iter) {
        if (posCount.find(iter->second) == posCount.end()) {
            posCount[iter->second] = 0;
            posWidth[iter->second] = 0;
        }
        posCount[iter->second]++;
        posWidth[iter->second] += widths[iter->first];
    }

    std::map<std::string, int> posProportions;

    for (auto iter = positions.begin(); iter != positions.end(); ++iter) {
        if (posCount[iter->second] > 1) {
            int percent = std::round(((double)widths[iter->first] / (double)posWidth[iter->second]) * 100);
            posProportions[iter->first] = percent;
        }
    }

    _uiSettings.writeDockSettings(positions, posProportions);
}

void MainWindow::loadDockLayout() {
    std::map<std::string, int> dockProportions;
    std::map<std::string, int> dockPositions = _uiSettings.readDockSettings(dockProportions);

    PropertyEditor* propertyEditor = PropertyEditor::GetPropertyEditor(this);
    std::map<std::string, QDockWidget*> dockWidgets = {
        {"Layers", &_layers},
        {"CliCommand", &_cliCommand},
        {"Toolbar", &_toolbar},
        {"PropertyEditor", propertyEditor}
    };

    std::map<int, QList<QDockWidget*>> resizeWidgets;
    std::map<int, QList<int>> resizeProportions;

    if (dockPositions.size() > 0) {
        for (auto iter = dockWidgets.begin(); iter != dockWidgets.end(); ++iter) {
            int pos = dockPositions[iter->first];
            addDockWidget((Qt::DockWidgetArea)pos, iter->second);

            if (dockProportions.find(iter->first) != dockProportions.end()) {
                if (resizeWidgets.find(pos) == resizeWidgets.end()) {
                    resizeWidgets[pos] = QList<QDockWidget*>();
                    resizeProportions[pos] = QList<int>();
                }

                resizeWidgets[pos].append(iter->second);
                resizeProportions[pos].append(dockProportions[iter->first]);
            }
        }

        for (auto iter = resizeWidgets.begin(); iter != resizeWidgets.end(); ++iter) {
            bool horiz = true;
            if ((iter->first == (int)Qt::LeftDockWidgetArea) || (iter->first == (int)Qt::RightDockWidgetArea)) {
                horiz = false;
            }
            resizeDocks(iter->second, resizeProportions[iter->first], (horiz) ? Qt::Horizontal : Qt::Vertical);
        }
    }
}
