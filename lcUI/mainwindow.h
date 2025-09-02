#pragma once

#include "luainterface.h"
#include "sol.hpp" 

#include <QMainWindow>
#include <QShortcut>
#include "widgets/clicommand.h"
#include "widgets/layers.h"
#include "widgets/linepatternselect.h"
#include "widgets/linewidthselect.h"
#include "widgets/colorselect.h"
#include "widgets/toolbar.h"
#include "widgets/customizeToolbar/customizetoolbar.h"
#include "managers/uisettings.h"
#include "managers/copymanager.h"
#include "cadmdichild.h"

#include "widgets/guiAPI/menu.h"

namespace Ui {
    class MainWindow;
}

namespace lc
{
    namespace ui
    {
        /*
            MainWindow GUI Initialization and Menu GUI API functions
        */
        class MainWindow : public QMainWindow
        {
            Q_OBJECT
        public:
            /**
            * \brief Constructor for MainWindow
            */
            explicit MainWindow();

            ~MainWindow();

            /**
            * \brief Trigger appropriate signals for input events
            */
            void ConnectInputEvents();

            /**
            * \brief Getter for clicommand
            */
            lc::ui::widgets::CliCommand* cliCommand();

            /**
            * \brief Getter for cadmdichild
            */
            lc::ui::CadMdiChild* cadMdiChild();

            /**
            * \brief Getter for toolbar
            */
            lc::ui::widgets::Toolbar* toolbar();

            /**
            * \brief Getter for layers
            */
            lc::ui::widgets::Layers* layers();

            /**
            * \brief Getter for lua interface
            */
            lc::ui::LuaInterface* luaInterface();

            /**
            * \brief Get context menu manager id for mainwindow
            */
            int contextMenuManagerId();

            /**
            * \brief Connect existing menu item to lua callback function
            * \param itemName item name , callback - function callback
            */
            void connectMenuItem(const std::string& itemName, sol::function callback);

            /**
            * \brief Run tool operation
            * \param operation Operation class , init_method - which init_method to run
            */
            void runOperation(sol::table operation, const std::string& init_method = "");

            /**
            * \brief Called on operation finish
            */
            void operationFinished();

            /**
            * \brief These will be called by runOperation for respective operation if extra icons
            *        are to be added during the operation
            * \param operation string to identify for which operation , options - list of functions to be run
            */
            void addOperationOptions(std::string operation, std::vector<sol::function> options);

            /**
            * \brief Read UI settings on program start up
            */
            void readUiSettings();

            /**
            * \brief Run the last operation
            */
            void runLastOperation();

            /**
            * \brief Return the last operation
            */
            std::string lastOperationName();

            /**
            * \brief Return the current operation
            */
            sol::table currentOperation();

            /**
            * \brief Copy selected entities to the clipboard
            */
            void copySelectedEntities(const std::vector<lc::entity::CADEntity_CSPtr>& cadEntities);
            
            /**
            * \brief Paste entities from the clipboard
            */
            void pasteEvent();

            /* ------------ MENU GUI FUNCTIONS ---------------- */

            /**
            * \brief Find and return menu item, FIND BY LABEL
            */
            lc::ui::api::MenuItem* findMenuItem(std::string label);

            /**
            * \brief Find and return menu item, FIND BY OBJECT NAME
            */
            lc::ui::api::MenuItem* findMenuItemByObjectName(std::string objectName);

            /**
            * \brief Add menu to the menu bar
            */
            lc::ui::api::Menu* addMenu(const std::string& menuName);

            void addMenu(lc::ui::api::Menu* menu);

            /**
            * \brief Get menu from the menu bar
            */
            lc::ui::api::Menu* menuByName(const std::string& menuName);

            /**
            * \brief Get menu by position
            */
            lc::ui::api::Menu* menuByPosition(int pos);

            /**
            * \brief Remove menu from menu bar
            */
            void removeMenu(const char* menuLabel);

            void removeMenu(int position);

            /* --- OTHER FUNCTIONS --- */
            void removeFromMenuMap(std::string menuName);

            /**
            * \brief Change the dock layout
            */
            void changeDockLayout(int i);

            /**
            * \brief Save the dock layout
            */
            void saveDockLayout();

            /**
            * \brief Load dock layout
            */
            void loadDockLayout();


        private:
            /**
            * \brief Add Menu items as actions, replace QAction with MenuItem
            */
            void initMenuAPI();

            /**
            * \brief Readd actions as menu items
            */
            void addActionsAsMenuItem(lc::ui::api::Menu* menu);

            /**
            * \brief Helper function for updating position variables of readded menu items
            */
            void fixMenuPositioning(lc::ui::api::Menu* menu);

            /**
            * \brief Helper function for finding menu item
            *        uses recursive helper function findMenuItemRecur
            */
            lc::ui::api::MenuItem* findMenuItemBy(std::string objectName, bool searchByLabel);

            /**
            * \brief Recursive function to search for menu item
            */
            lc::ui::api::MenuItem* findMenuItemRecur(QMenu* menu, QString objectName, bool searchByLabel);

            /**
            * \brief Check if menu of same label already exists
            */
            bool checkForMenuOfSameLabel(const std::string& label);

            /**
            * \brief Pass the selected entities to the property editor
            */
            void selectionChanged();

            /**
            * \brief Add other additional menus
            */
            void addOtherMenus();

        public slots:
            // CadMdiChild slots
            void triggerMousePressed();
            void triggerMouseReleased();
            void triggerMouseMoved();
            void triggerSelectionChanged();
            void triggerKeyPressed(int key);

            // CliCommand slots
            void triggerCoordinateEntered(lc::geo::Coordinate coordinate);
            void triggerRelativeCoordinateEntered(lc::geo::Coordinate coordinate);
            void triggerNumberEntered(double number);
            void triggerTextEntered(QString text);
            void triggerFinishOperation();
            void triggerCommandEntered(QString command);

            // Slot to deal with things to do in C++ after a point is triggered
            void triggerPoint(lc::geo::Coordinate coordinate);

            // File related slots
            void newFile();
            void openFile();

            // Edit slots
            void undo();
            void redo();
            void selectAll();
            void selectNone();
            void invertSelection();
            void clearUndoableStack();
            void autoScale();

            // Customize toolbar slots
            void runCustomizeToolbar();
            void writeSettings();
            void loadDefaultSettings();

        signals:
            void point(lc::geo::Coordinate coordinate);

        protected:
            Ui::MainWindow* ui;
            lc::ui::LuaInterface _luaInterface;

            lc::ui::CadMdiChild _cadMdiChild;
            lc::ui::widgets::Layers _layers;
            lc::ui::widgets::CliCommand _cliCommand;
            lc::ui::widgets::Toolbar _toolbar;

            // Select tools
            lc::ui::widgets::LinePatternSelect linePatternSelect;
            lc::ui::widgets::LineWidthSelect lineWidthSelect;
            lc::ui::widgets::ColorSelect colorSelect;

            lc::ui::widgets::CustomizeToolbar* _customizeToolbar;
            lc::ui::UiSettings _uiSettings;
            lc::ui::CopyManager _copyManager;

            lc::geo::Coordinate lastPoint;
            std::map<std::string, std::vector<sol::function>> operation_options;

            QMap<QString, api::Menu*> menuMap;
            int _contextMenuManagerId;

            sol::table _oldOperation;
            std::string _oldOpInitMethod;

            // Shortcuts
            QShortcut* copyShortcut;
            QShortcut* pasteShortcut;
        };
    }
}
