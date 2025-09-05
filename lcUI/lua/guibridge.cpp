#include "lua/guibridge.h"
#include "luainterface.h"

#include "cadmdichild.h"
#include "documentcanvas.h"
#include "lcadviewer.h"
#include "propertyeditor.h"
#include "widgets/luascript.h"
#include "widgets/customizeToolbar/customizetoolbar.h"
#include "widgets/clicommand.h"
#include "widgets/toolbar.h"
#include "widgets/guiAPI/toolbartab.h"
#include "widgets/layers.h"
#include "widgets/guiAPI/menu.h"
#include "widgets/guiAPI/menuitem.h"
#include "widgets/guiAPI/toolbarbutton.h"
#include "widgets/guiAPI/toolbargroup.h"
#include "widgets/guiAPI/dialogwidget.h"
#include "widgets/guiAPI/inputgui.h"
#include "widgets/guiAPI/textgui.h"
#include "widgets/guiAPI/horizontalgroupgui.h"
#include "widgets/guiAPI/buttongui.h"
#include "widgets/guiAPI/checkboxgui.h"
#include "widgets/guiAPI/radiobuttongui.h"
#include "widgets/guiAPI/radiogroupgui.h"
#include "widgets/guiAPI/coordinategui.h"
#include "widgets/guiAPI/anglegui.h"
#include "widgets/guiAPI/slidergui.h"
#include "widgets/guiAPI/comboboxgui.h"
#include "widgets/guiAPI/numbergui.h"
#include "widgets/guiAPI/colorgui.h"
#include "widgets/guiAPI/entitygui.h"
#include "widgets/guiAPI/listgui.h"
#include <drawables/tempentities.h>
#include "mainwindow.h"

using namespace lc;
using namespace viewer;
using namespace ui;
using namespace widgets;
using namespace dialog;
using namespace manager;
using namespace drawable;

void luaOpenGUIBridge(sol::state & luaVM)
{
    luaVM["gui"] = luaVM.create_table();

    addLCBindings(luaVM);
    addLuaGUIAPIBindings(luaVM);
}

void addLCBindings(sol::state & luaVM) 
{
    sol::table lc = luaVM["lc"];

    lc.new_usertype<CadMdiChild>(
            "CadMdiChild",
            "new", []() { return new CadMdiChild; },
            "getSnapManager", &CadMdiChild::getSnapManager,
            "cursor", &CadMdiChild::cursor,
            "document", &CadMdiChild::document,
            "saveFile", &CadMdiChild::saveFile,
            "saveAsFile", &CadMdiChild::saveAsFile,
            "openFile", &CadMdiChild::openFile,
            "selection", &CadMdiChild::selection,
            "newDocument", &CadMdiChild::newDocument,
            "setDestroyCallback", &CadMdiChild::setDestroyCallback,
            "tempEntities", &CadMdiChild::tempEntities,
            "undoManager", &CadMdiChild::undoManager,
            "viewer", &CadMdiChild::viewer,
            "activeLayer", &CadMdiChild::activeLayer,
            "metaInfoManager", &CadMdiChild::metaInfoManager,
            "getFilename", &CadMdiChild::getFilename,
            "activeViewport", &CadMdiChild::activeViewport
            );

    lc.new_usertype<drawable::Cursor>(
            "Cursor",
            "position", &drawable::Cursor::position
            );

    lc.new_usertype<LCADViewer>(
            "LCADViewer",
            "autoScale", &LCADViewer::autoScale,
            "setOperationActive", &LCADViewer::setOperationActive,
            "docCanvas", &LCADViewer::docCanvas
            );

    lc.new_usertype<LuaInterface>(
            "LuaInterface",
            "pluginList", &LuaInterface::pluginList,
            "operation", &LuaInterface::operation,
            "setOperation", &LuaInterface::setOperation,
            "registerEvent", &LuaInterface::registerEvent,
            "deleteEvent", &LuaInterface::deleteEvent,
            "triggerEvent", &LuaInterface::triggerEvent,
            "finishOperation", &LuaInterface::finishOperation
            );

    lc.new_usertype<widgets::LuaScript>(
            "LuaScript",
            sol::constructors<widgets::LuaScript(lc::ui::MainWindow*)>(),
            "show", [](widgets::LuaScript& self) { self.show(); }
            );

    lc.new_usertype<widgets::CustomizeToolbar>(
            "CustomizeToolbar",
            sol::constructors<widgets::CustomizeToolbar(widgets::Toolbar*)>(),
            "show", [](widgets::CustomizeToolbar& self) { self.show(); }
            );

    lc.new_usertype<DocumentCanvas>(
            "DocumentCanvas",
            "autoScale", &DocumentCanvas::autoScale,
            "selectPoint", &DocumentCanvas::selectPoint,
            "removeSelection", &DocumentCanvas::removeSelection,
            "inverseSelection", &DocumentCanvas::inverseSelection,
            "selectAll", &DocumentCanvas::selectAll
            );

    lc.new_usertype<widgets::CliCommand>(
            "CliCommand",
            "addCommand", &widgets::CliCommand::addCommand,
            "write", [](widgets::CliCommand* cliCommand, const char* message) { cliCommand->write(message); },
            "returnText", &widgets::CliCommand::returnText,
            "commandActive", &widgets::CliCommand::commandActive,
            "runCommand", &widgets::CliCommand::runCommand,
            "enableCommand", [](widgets::CliCommand* cliCommand, const char* command) { cliCommand->enableCommand(command, true); },
            "disableCommand", [](widgets::CliCommand* cliCommand, const char* command) { cliCommand->enableCommand(command, false); },
            "isCommandEnabled", &widgets::CliCommand::isCommandEnabled,
            "availableCommands", &widgets::CliCommand::availableCommands,
            "commandsHistory", &widgets::CliCommand::commandsHistory,
            "clear", &widgets::CliCommand::clear
            );

    lc.new_usertype<widgets::Toolbar>(
            "Toolbar",
            "tabByName", &widgets::Toolbar::tabByName,
            "removeGroupByName", &widgets::Toolbar::removeGroupByName,
            "updateSnapButtons", &widgets::Toolbar::updateSnapButtons,
            "addTab", sol::overload(
                static_cast<api::ToolbarTab*(widgets::Toolbar::*)(const char*)>(&widgets::Toolbar::addTab),
                static_cast<void(widgets::Toolbar::*)(api::ToolbarTab*)>(&widgets::Toolbar::addTab)
                ),
            "removeTab", sol::overload(
                static_cast<void(widgets::Toolbar::*)(api::ToolbarTab*)>(&widgets::Toolbar::removeTab),
                static_cast<void(widgets::Toolbar::*)(const char*)>(&widgets::Toolbar::removeTab)
                ),
            "addButton", [](widgets::Toolbar* toolbar, const char* name, const char* icon, const char* group, sol::function cb, const char* tooltip) 
            { toolbar->addButton(name, icon, group, cb, tooltip); }
            );

    lc.new_usertype<drawable::TempEntities>(
            "TempEntities",
            "addEntity", &drawable::TempEntities::addEntity,
            "removeEntity", &drawable::TempEntities::removeEntity
            );

    lc.new_usertype<lc::ui::MetaInfoManager>(
            "MetaInfoManager",
            "metaInfo", &lc::ui::MetaInfoManager::metaInfo
            );

    lc.new_usertype<widgets::Layers>(
            "Layers",
            "setMdiChild", &widgets::Layers::setMdiChild,
            "layerByName", &widgets::Layers::layerByName,
            "addLayer", sol::overload(
                static_cast<void(widgets::Layers::*)(lc::meta::Layer_CSPtr)>(&widgets::Layers::addLayer),
                static_cast<lc::meta::Layer_CSPtr(widgets::Layers::*)(const char*)>(&widgets::Layers::addLayer),
                static_cast<lc::meta::Layer_CSPtr(widgets::Layers::*)(const char*, double)>(&widgets::Layers::addLayer),
                static_cast<lc::meta::Layer_CSPtr(widgets::Layers::*)(const char*, int, int, int)>(&widgets::Layers::addLayer),
                static_cast<lc::meta::Layer_CSPtr(widgets::Layers::*)(const char*, double, int, int, int)>(&widgets::Layers::addLayer),
                static_cast<lc::meta::Layer_CSPtr(widgets::Layers::*)(const char*, lc::Color)>(&widgets::Layers::addLayer),
                static_cast<lc::meta::Layer_CSPtr(widgets::Layers::*)(const char*, double, lc::Color)>(&widgets::Layers::addLayer)
                ),
            "removeLayer", sol::overload(
                static_cast<void(widgets::Layers::*)(lc::meta::Layer_CSPtr)>(&widgets::Layers::removeLayer),
                static_cast<void(widgets::Layers::*)(const char*)>(&widgets::Layers::removeLayer)
                ),
            "renameLayer", sol::overload(
                static_cast<void(widgets::Layers::*)(lc::meta::Layer_CSPtr, const char*)>(&widgets::Layers::renameLayer),
                static_cast<void(widgets::Layers::*)(const char*, const char*)>(&widgets::Layers::renameLayer)
                ),
            "replaceLayer", sol::overload(
                    static_cast<void(widgets::Layers::*)(lc::meta::Layer_CSPtr, lc::meta::Layer_CSPtr)>(&widgets::Layers::replaceLayerAPI),
                    static_cast<void(widgets::Layers::*)(const char*, lc::meta::Layer_CSPtr)>(&widgets::Layers::replaceLayerAPI)
                    )
                );

    lc.new_usertype<manager::SnapManagerImpl>(
            "SnapManager",
            "setGridSnappable", &manager::SnapManagerImpl::setGridSnappable,
            "setIntersectionSnappable", &manager::SnapManagerImpl::setIntersectionsSnappable,
            "setMiddleSnappable", &manager::SnapManagerImpl::setMiddleSnappable,
            "setEntitySnappable", &manager::SnapManagerImpl::setEntitySnappable
            );

    lc.new_usertype<lc::ui::MainWindow>(
            "MainWindow",
            "connectMenuItem", &lc::ui::MainWindow::connectMenuItem,
            "cliCommand", &lc::ui::MainWindow::cliCommand,
            "cadMdiChild", &lc::ui::MainWindow::cadMdiChild,
            "toolbar", &lc::ui::MainWindow::toolbar,
            "layers", &lc::ui::MainWindow::layers,
            "operationFinished", &lc::ui::MainWindow::operationFinished,
            "findMenuItem", &lc::ui::MainWindow::findMenuItem,
            "findMenuItemByObjectName", &lc::ui::MainWindow::findMenuItemByObjectName,
            "menuByName", &lc::ui::MainWindow::menuByName,
            "menuByPosition", &lc::ui::MainWindow::menuByPosition,
            "runCustomizeToolbar", &lc::ui::MainWindow::runCustomizeToolbar,
            "changeDockLayout", &lc::ui::MainWindow::changeDockLayout,
            "saveDockLayout", &lc::ui::MainWindow::saveDockLayout,
            "loadDockLayout", &lc::ui::MainWindow::loadDockLayout,
            "undo", &lc::ui::MainWindow::undo,
            "redo", &lc::ui::MainWindow::redo,
            "selectAll", &lc::ui::MainWindow::selectAll,
            "selectNone", &lc::ui::MainWindow::selectNone,
            "invertSelection", &lc::ui::MainWindow::invertSelection,
            "runLastOperation", &lc::ui::MainWindow::runLastOperation,
            "currentOperation", &lc::ui::MainWindow::currentOperation,
            "copySelectedEntities", &lc::ui::MainWindow::copySelectedEntities,
            "pasteEvent", &lc::ui::MainWindow::pasteEvent,
            "addMenu", sol::overload(
                    static_cast<lc::ui::api::Menu*(lc::ui::MainWindow::*)(const std::string&)>(&lc::ui::MainWindow::addMenu),
                    static_cast<void(lc::ui::MainWindow::*)(lc::ui::api::Menu*)>(&lc::ui::MainWindow::addMenu)
                    ),
            "removeMenu", sol::overload(
                    static_cast<void(lc::ui::MainWindow::*)(const char*)>(&lc::ui::MainWindow::removeMenu),
                    static_cast<void(lc::ui::MainWindow::*)(int)>(&lc::ui::MainWindow::removeMenu)
                    ),
            "runOperation", sol::overload(
                    &lc::ui::MainWindow::runOperation, 
                    [](lc::ui::MainWindow& self, sol::table operation) { self.runOperation(operation); }
                    )
                );

    lc.new_usertype<lc::ui::PropertyEditor>(
            "PropertyEditor",
            "GetPropertyEditor", &lc::ui::PropertyEditor::GetPropertyEditor,
            "propertyChanged", &lc::ui::PropertyEditor::propertyChanged
            );
}

void addLuaGUIAPIBindings(sol::state & luaVM) 
{
    sol::table gui = luaVM["gui"];

    gui.new_usertype<lc::ui::api::Menu>(
            "Menu",
            sol::constructors<lc::ui::api::Menu(const char*)>(),
            "label", &lc::ui::api::Menu::label,
            "setLabel", &lc::ui::api::Menu::setLabel,
            "position", &lc::ui::api::Menu::position,
            "setPosition", &lc::ui::api::Menu::setPosition,
            "remove", &lc::ui::api::Menu::remove,
            "itemByName", &lc::ui::api::Menu::itemByName,
            "itemByPosition", &lc::ui::api::Menu::itemByPosition,
            "menuByName", &lc::ui::api::Menu::menuByName,
            "menuByPosition", &lc::ui::api::Menu::menuByPosition,
            "hide", [](lc::ui::api::Menu& self) { self.hide(); },
            "show", [](lc::ui::api::Menu& self) { self.show(); },
            "isEnabled", [](lc::ui::api::Menu& self) { return self.isEnabled(); },
            "setEnabled", [](lc::ui::api::Menu& self, bool enable) { self.setEnabled(enable); },
            "addItem", sol::overload(
                static_cast<void(lc::ui::api::Menu::*)(lc::ui::api::MenuItem*)>(&lc::ui::api::Menu::addItem),
                static_cast<lc::ui::api::MenuItem * (lc::ui::api::Menu::*)(const char*)>(&lc::ui::api::Menu::addItem),
                static_cast<lc::ui::api::MenuItem * (lc::ui::api::Menu::*)(const char*, sol::function)>(&lc::ui::api::Menu::addItem)
                ),
            "removeItem", sol::overload(
                    static_cast<void(lc::ui::api::Menu::*)(lc::ui::api::MenuItem*)>(&lc::ui::api::Menu::removeItem),
                    static_cast<void(lc::ui::api::Menu::*)(const char*)>(&lc::ui::api::Menu::removeItem)
                    ),
            "removeMenu", sol::overload(
                    static_cast<void(lc::ui::api::Menu::*)(lc::ui::api::Menu*)>(&lc::ui::api::Menu::removeMenu),
                    static_cast<void(lc::ui::api::Menu::*)(const char*)>(&lc::ui::api::Menu::removeMenu)
                    ),
            "addMenu", sol::overload(
                    static_cast<lc::ui::api::Menu * (lc::ui::api::Menu::*)(const char*)>(&lc::ui::api::Menu::addMenu),
                    static_cast<void(lc::ui::api::Menu::*)(lc::ui::api::Menu*)>(&lc::ui::api::Menu::addMenu)
                    )
                );

    gui.new_usertype<lc::ui::api::MenuItem>(
            "MenuItem",
            sol::constructors<lc::ui::api::MenuItem(const char*), lc::ui::api::MenuItem(const char*, sol::function)>(),
            "label", &lc::ui::api::MenuItem::label,
            "setLabel", &lc::ui::api::MenuItem::setLabel,
            "position", &lc::ui::api::MenuItem::position,
            "setPosition", &lc::ui::api::MenuItem::setPosition,
            "remove", &lc::ui::api::MenuItem::remove,
            "removeCallback", &lc::ui::api::MenuItem::removeCallback,
            "setCheckable", &lc::ui::api::MenuItem::setCheckable,
            "setChecked", &lc::ui::api::MenuItem::setChecked,
            "addCheckedCallback", &lc::ui::api::MenuItem::addCheckedCallback,
            "hide", [](lc::ui::api::MenuItem& self) { self.hide(); },
            "show", [](lc::ui::api::MenuItem& self) { self.show(); },
            "isEnabled", [](lc::ui::api::MenuItem& self) { return self.isEnabled(); },
            "setEnabled", [](lc::ui::api::MenuItem& self, bool enable) { self.setEnabled(enable); },
            "addCallback", sol::overload(
                static_cast<void(lc::ui::api::MenuItem::*)(sol::function)>(&lc::ui::api::MenuItem::addCallback),
                static_cast<void(lc::ui::api::MenuItem::*)(const char*, sol::function)>(&lc::ui::api::MenuItem::addCallback)
            )
        );

    gui.new_usertype<api::ToolbarTab>(
            "ToolbarTab",
            sol::constructors<api::ToolbarTab(const char*)>(),
            "addButton", &api::ToolbarTab::addButton,
            "buttonByText", &api::ToolbarTab::buttonByText,
            "groupByName", &api::ToolbarTab::groupByName,
            "label", &api::ToolbarTab::label,
            "setLabel", &api::ToolbarTab::setLabel,
            "groups", &api::ToolbarTab::groups,
            "remove", &api::ToolbarTab::remove,
            "enable", [](lc::ui::api::ToolbarTab& self) { self.setEnabled(true); },
            "disable", [](lc::ui::api::ToolbarTab& self) { self.setEnabled(false); },
            "removeGroup", sol::overload(
                static_cast<void(api::ToolbarTab::*)(lc::ui::api::ToolbarGroup*)>(&api::ToolbarTab::removeGroup),
                static_cast<void(api::ToolbarTab::*)(const char*)>(&api::ToolbarTab::removeGroup)
                ),
            "addGroup", sol::overload(
                static_cast<void(api::ToolbarTab::*)(api::ToolbarGroup*)>(&api::ToolbarTab::addGroup),
                static_cast<api::ToolbarGroup * (api::ToolbarTab::*)(const char*, int)>(&api::ToolbarTab::addGroup),
                [](api::ToolbarTab& self, const char* name) { return self.addGroup(name); }
                )
                );

    gui.new_usertype<lc::ui::api::ToolbarButton>(
            "ToolbarButton",
            sol::constructors<lc::ui::api::ToolbarButton(const char*, const char*),
            lc::ui::api::ToolbarButton(const char*, const char*, sol::function),
            lc::ui::api::ToolbarButton(const char*, const char*, const char*),
            lc::ui::api::ToolbarButton(const char*, const char*, sol::function, const char*),
            lc::ui::api::ToolbarButton(const char*, const char*, const char*, bool),
            lc::ui::api::ToolbarButton(const char*, const char*, sol::function, const char*, bool)>(),
            "label", &lc::ui::api::ToolbarButton::label,
            "setLabel", &lc::ui::api::ToolbarButton::setLabel,
            "setTooltip", &lc::ui::api::ToolbarButton::setTooltip,
            "removeCallback", &lc::ui::api::ToolbarButton::removeCallback,
            "remove", &lc::ui::api::ToolbarButton::remove,
            "hide", [](lc::ui::api::ToolbarButton& self) { self.hide(); },
            "show", [](lc::ui::api::ToolbarButton& self) { self.show(); },
            "isEnabled", [](lc::ui::api::ToolbarButton& self) { return self.isEnabled(); },
            "enable", [](lc::ui::api::ToolbarButton& self) { self.setEnabled(true); },
            "disable", [](lc::ui::api::ToolbarButton& self) { self.setEnabled(false); },
            "addCallback", sol::overload(
                static_cast<void(lc::ui::api::ToolbarButton::*)(sol::function)>(&lc::ui::api::ToolbarButton::addCallback),
                static_cast<void(lc::ui::api::ToolbarButton::*)(const char*, sol::function)>(&lc::ui::api::ToolbarButton::addCallback)
                )
                );

    gui.new_usertype<lc::ui::api::ToolbarGroup>(
            "ToolbarGroup",
            sol::constructors<lc::ui::api::ToolbarGroup(const char*), lc::ui::api::ToolbarGroup(const char*, int)>(),
            "label", &lc::ui::api::ToolbarGroup::label,
            "setLabel", &lc::ui::api::ToolbarGroup::setLabel,
            "buttonByName", &lc::ui::api::ToolbarGroup::buttonByName,
            "buttons", &lc::ui::api::ToolbarGroup::buttons,
            "remove", &lc::ui::api::ToolbarGroup::remove,
            "setWidth", &lc::ui::api::ToolbarGroup::setWidth,
            "width", &lc::ui::api::ToolbarGroup::width,
            "hide", [](lc::ui::api::ToolbarGroup& self) { self.hide(); },
            "show", [](lc::ui::api::ToolbarGroup& self) { self.show(); },
            "enable", [](lc::ui::api::ToolbarGroup& self) { self.setEnabled(true); },
            "disable", [](lc::ui::api::ToolbarGroup& self) { self.setEnabled(false); },
            "addButton", sol::overload(
                static_cast<void(lc::ui::api::ToolbarGroup::*)(lc::ui::api::ToolbarButton*)>(&lc::ui::api::ToolbarGroup::addButton),
                static_cast<lc::ui::api::ToolbarButton * (lc::ui::api::ToolbarGroup::*)(const char*, const char*)>(&lc::ui::api::ToolbarGroup::addButton),
                static_cast<lc::ui::api::ToolbarButton * (lc::ui::api::ToolbarGroup::*)(const char*, const char*, sol::function)>(&lc::ui::api::ToolbarGroup::addButton)
                ),
            "removeButton", sol::overload(
                static_cast<void(lc::ui::api::ToolbarGroup::*)(lc::ui::api::ToolbarButton*)>(&lc::ui::api::ToolbarGroup::removeButton),
                static_cast<void(lc::ui::api::ToolbarGroup::*)(const char*)>(&lc::ui::api::ToolbarGroup::removeButton)
                )
                );

    gui.new_usertype<lc::ui::api::InputGUIContainer>(
            "InputGUIContainer",
            "inputWidgets", &lc::ui::api::InputGUIContainer::inputWidgets,
            "addFinishCallback", &lc::ui::api::InputGUIContainer::addFinishCallback,
            "keys", &lc::ui::api::InputGUIContainer::keys,
            "addWidget", &lc::ui::api::InputGUIContainer::addWidget
            );

    gui.new_usertype<lc::ui::api::DialogWidget>(
            "DialogWidget", 
            sol::constructors<lc::ui::api::DialogWidget(const std::string&, lc::ui::MainWindow*)>(),
            sol::base_classes, sol::bases<lc::ui::api::InputGUIContainer>(),
            "setFinishButton", &lc::ui::api::DialogWidget::setFinishButton,
            "enable", [](lc::ui::api::DialogWidget& self) { self.setEnabled(true); },
            "disable", [](lc::ui::api::DialogWidget& self) { self.setEnabled(false); },
            "addWidget", &lc::ui::api::DialogWidget::addWidget
            );

    gui.new_usertype<lc::ui::api::InputGUI>(
            "InputGUI",
            "label", &lc::ui::api::InputGUI::label,
            "setLabel", &lc::ui::api::InputGUI::setLabel,
            "key", &lc::ui::api::InputGUI::key,
            "enable", [](lc::ui::api::InputGUI& self) { self.setEnabled(true); },
            "disable", [](lc::ui::api::InputGUI& self) { self.setEnabled(false); }
            );

    gui.new_usertype<lc::ui::api::TextGUI>(
            "Text", 
            sol::constructors<lc::ui::api::TextGUI(std::string)>(),
            sol::base_classes, sol::bases<lc::ui::api::InputGUI>(),
            "value", &lc::ui::api::TextGUI::value,
            "setValue", &lc::ui::api::TextGUI::setValue,
            "addFinishCallback", &lc::ui::api::TextGUI::addFinishCallback,
            "addOnChangeCallback", &lc::ui::api::TextGUI::addOnChangeCallback
            );

    gui.new_usertype<lc::ui::api::ButtonGUI>(
            "Button", 
            sol::constructors<lc::ui::api::ButtonGUI(std::string)>(),
            sol::base_classes, sol::bases<lc::ui::api::InputGUI>(),
            "setLabel", &lc::ui::api::ButtonGUI::setLabel,
            "addCallback", &lc::ui::api::ButtonGUI::addCallback
            );

    gui.new_usertype<lc::ui::api::CheckBoxGUI>(
            "CheckBox", 
            sol::constructors<lc::ui::api::CheckBoxGUI(std::string), lc::ui::api::CheckBoxGUI(std::string, bool)>(),
            sol::base_classes, sol::bases<lc::ui::api::InputGUI>(),
            "setLabel", &lc::ui::api::CheckBoxGUI::setLabel,
            "addCallback", &lc::ui::api::CheckBoxGUI::addCallback,
            "value", &lc::ui::api::CheckBoxGUI::value,
            "setValue", &lc::ui::api::CheckBoxGUI::setValue
            );

    gui.new_usertype<lc::ui::api::RadioButtonGUI>(
            "RadioButton",
            sol::constructors<lc::ui::api::RadioButtonGUI(std::string)>(),
            "label", &lc::ui::api::RadioButtonGUI::label,
            "setLabel", &lc::ui::api::RadioButtonGUI::setLabel,
            "addCallback", &lc::ui::api::RadioButtonGUI::addCallback,
            "checked", [](lc::ui::api::RadioButtonGUI& self) { self.isChecked(); },
            "setChecked", [](lc::ui::api::RadioButtonGUI& self, bool check) { self.setChecked(check); }
            );

    gui.new_usertype<lc::ui::api::HorizontalGroupGUI>(
            "HorizontalGroup",
            sol::constructors<lc::ui::api::HorizontalGroupGUI(std::string)>(),
            sol::base_classes, sol::bases<lc::ui::api::InputGUI>(),
            "addWidget", sol::overload(
                [](lc::ui::api::HorizontalGroupGUI& self, const std::string& key, lc::ui::api::ButtonGUI* newButton) { self.addWidget(key, newButton); },
                [](lc::ui::api::HorizontalGroupGUI& self, const std::string& key, lc::ui::api::InputGUI* newGUI) { self.addWidget(key, newGUI); },
                [](lc::ui::api::HorizontalGroupGUI& self, const std::string& key, lc::ui::api::CheckBoxGUI* checkGUI) { self.addWidget(key, checkGUI); }
            )
        );

    gui.new_usertype<lc::ui::api::RadioGroupGUI>(
            "RadioGroup",
            sol::constructors<lc::ui::api::RadioGroupGUI(std::string)>(),
            sol::base_classes, sol::bases<lc::ui::api::InputGUI>(),
            "addButton", &lc::ui::api::RadioGroupGUI::addButton
            );

    gui.new_usertype<lc::ui::api::CoordinateGUI>(
            "Coordinate",
            sol::constructors<lc::ui::api::CoordinateGUI(std::string)>(),
            sol::base_classes, sol::bases<lc::ui::api::InputGUI>(),
            "addFinishCallback", &lc::ui::api::CoordinateGUI::addFinishCallback,
            "addOnChangeCallback", &lc::ui::api::CoordinateGUI::addOnChangeCallback,
            "value", &lc::ui::api::CoordinateGUI::value,
            "setValue", &lc::ui::api::CoordinateGUI::setValue
            );

    gui.new_usertype<lc::ui::api::AngleGUI>(
            "Angle", 
            sol::constructors<lc::ui::api::AngleGUI(std::string)>(),
            sol::base_classes, sol::bases<lc::ui::api::InputGUI>(),
            "toDegrees", &lc::ui::api::AngleGUI::toDegrees,
            "toRadians", &lc::ui::api::AngleGUI::toRadians,
            "addFinishCallback", &lc::ui::api::AngleGUI::addFinishCallback,
            "addOnChangeCallback", &lc::ui::api::AngleGUI::addOnChangeCallback,
            "value", &lc::ui::api::AngleGUI::value,
            "setValue", &lc::ui::api::AngleGUI::setValue
            );

    gui.new_usertype<lc::ui::api::SliderGUI>(
            "Slider", 
            sol::constructors<lc::ui::api::SliderGUI(std::string), lc::ui::api::SliderGUI(std::string, int minVal, int maxVal)>(),
            sol::base_classes, sol::bases<lc::ui::api::InputGUI>(),
            "addCallback", &lc::ui::api::SliderGUI::addCallback,
            "setLabel", &lc::ui::api::SliderGUI::setLabel,
            "value", &lc::ui::api::SliderGUI::value,
            "setValue", &lc::ui::api::SliderGUI::setValue
            );

    gui.new_usertype<lc::ui::api::ComboBoxGUI>(
            "ComboBox", 
            sol::constructors<lc::ui::api::ComboBoxGUI(std::string)>(),
            sol::base_classes, sol::bases<lc::ui::api::InputGUI>(),
            "addCallback", &lc::ui::api::ComboBoxGUI::addCallback,
            "setLabel", &lc::ui::api::ComboBoxGUI::setLabel,
            "value", &lc::ui::api::ComboBoxGUI::value,
            "addItem", sol::overload(
                [](lc::ui::api::ComboBoxGUI& self, const std::string& item) { self.addItem(item); },
                [](lc::ui::api::ComboBoxGUI& self, const std::string& item, int index) { self.addItem(item, index); }
                ),
            "setValue", sol::overload(
                static_cast<void(lc::ui::api::ComboBoxGUI::*)(const std::string&)>(&lc::ui::api::ComboBoxGUI::setValue),
                static_cast<void(lc::ui::api::ComboBoxGUI::*)(int)>(&lc::ui::api::ComboBoxGUI::setValue)
                )
            );

    gui.new_usertype<lc::ui::api::NumberGUI>(
            "Number", 
            sol::constructors<lc::ui::api::NumberGUI(std::string)>(),
            sol::base_classes, sol::bases<lc::ui::api::InputGUI>(),
            "addCallback", &lc::ui::api::NumberGUI::addCallback,
            "setLabel", &lc::ui::api::NumberGUI::setLabel,
            "value", &lc::ui::api::NumberGUI::value,
            "setValue", &lc::ui::api::NumberGUI::setValue
            );

    gui.new_usertype<lc::ui::api::ColorGUI>(
            "ColorPicker", 
            sol::constructors<lc::ui::api::ColorGUI(std::string)>(),
            sol::base_classes, sol::bases<lc::ui::api::InputGUI>(),
            "value", &lc::ui::api::ColorGUI::value,
            "setValue", &lc::ui::api::ColorGUI::setValue,
            "addCallback", &lc::ui::api::ColorGUI::addCallback
            );

    gui.new_usertype<lc::ui::api::EntityGUI>(
            "EntityPicker",
            sol::constructors<lc::ui::api::EntityGUI(std::string)>(),
            sol::base_classes, sol::bases<lc::ui::api::InputGUI>(),
            "value", &lc::ui::api::EntityGUI::value,
            "setValue", &lc::ui::api::EntityGUI::setValue,
            "addEntity", &lc::ui::api::EntityGUI::addEntity,
            "addCallback", &lc::ui::api::EntityGUI::addCallback
            );

    gui.new_usertype<lc::ui::api::ListGUI>(
            "List", 
            sol::constructors<lc::ui::api::ListGUI(std::string)>(),
            sol::base_classes, sol::bases<lc::ui::api::InputGUI>(),
            "addItem", &lc::ui::api::ListGUI::addItem,
            "setListType", static_cast<void(lc::ui::api::ListGUI::*)(const std::string&)>(&lc::ui::api::ListGUI::setListType)
            );
}
