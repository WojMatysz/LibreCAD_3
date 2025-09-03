#include "propertyeditor.h"

#include "widgets/guiAPI/entitynamevisitor.h"
#include <QVBoxLayout>
#include <QScrollArea>

#include "widgets/guiAPI/numbergui.h"
#include "widgets/guiAPI/coordinategui.h"
#include "widgets/guiAPI/anglegui.h"
#include "widgets/guiAPI/textgui.h"
#include "widgets/guiAPI/listgui.h"
#include "widgets/guiAPI/horizontalgroupgui.h"
#include "widgets/guiAPI/lineselectgui.h"
#include "widgets/guiAPI/comboboxgui.h"

#include <cad/builders/lwpolyline.h>
#include "widgets/widgettitlebar.h"

using namespace lc::ui;

std::map<lc::ui::MainWindow*, PropertyEditor*> PropertyEditor::instances;

PropertyEditor::PropertyEditor(lc::ui::MainWindow* mainWindow)
    :
    QDockWidget("Property Editor", mainWindow),
    InputGUIContainer("property_editor", mainWindow)
{
    QScrollArea* parentWidget = new QScrollArea(this);
parentWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); parentWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    parentWidget->setWidgetResizable(true);

    QTreeWidget* widget = new QTreeWidget(parentWidget);
    widget->setColumnCount(2);
    widget->setHeaderHidden(true);
    widget->setObjectName("guiContainer");

    this->setStyleSheet("QTreeWidget{background: rgb(252, 255, 247)}");
    widget->setStyleSheet("QTreeWidget::item{ border : 1px solid rgb(156, 220, 31); margin-left: 2; padding-left: 5px;} QTreeView::item:selected {border : 3px solid rgb(156, 220, 31)}");
    widget->setColumnWidth(0, 200);

    parentWidget->setWidget(widget);
    this->setWidget(parentWidget);

    _metaInfoManager = std::make_shared<lc::ui::MetaInfoManager>();

    widgets::WidgetTitleBar* titleBar = new widgets::WidgetTitleBar("Property Editor", this,
            widgets::WidgetTitleBar::TitleBarOptions::HorizontalOnHidden);
    this->setTitleBarWidget(titleBar);
}

PropertyEditor* PropertyEditor::GetPropertyEditor(lc::ui::MainWindow* mainWindow) {
    if (instances.find(mainWindow) == instances.end()) {
        instances[mainWindow] = new PropertyEditor(mainWindow);
    }

    return instances[mainWindow];
}

void PropertyEditor::clear(std::vector<lc::entity::CADEntity_CSPtr> selectedEntities) {
    QTreeWidget* guicontainer = this->widget()->findChild<QTreeWidget*>("guiContainer");

    for (std::set<unsigned long>::iterator iter = _selectedEntities.begin(); iter != _selectedEntities.end();) {
        bool found = false;

        for (lc::entity::CADEntity_CSPtr cadEnt : selectedEntities) {
            if (cadEnt->id() == *iter) {
                found = true;
                break;
            }
        }

        if (!found) {
            for (const std::string& keyStr : _entityProperties[*iter]) {
                removeInputGUI(keyStr, false);
            }

            guicontainer->removeItemWidget(_entityGroup[*iter], 0);
            guicontainer->takeTopLevelItem(guicontainer->indexOfTopLevelItem(_entityGroup[*iter]));
            delete _entityGroup[*iter];

            _entityProperties.erase(*iter);
            _entityGroup.erase(*iter);
            iter = _selectedEntities.erase(iter);
        }
        else {
            ++iter;
        }
    }
}

void PropertyEditor::addEntity(lc::entity::CADEntity_CSPtr entity) {
    QTreeWidget* guicontainer = this->widget()->findChild<QTreeWidget*>("guiContainer");

    if (_selectedEntities.find(entity->id()) == _selectedEntities.end()) {
        _selectedEntities.insert(entity->id());
        _entityProperties[entity->id()] = std::vector<std::string>();

        // Get entity information
        api::EntityNameVisitor entityVisitor;
        entity->dispatch(entityVisitor);
        QString entityInfo = QString(entityVisitor.getEntityInformation().c_str()) + QString(" #") + QString::number(entity->id());
        QTreeWidgetItem* treeItem = new QTreeWidgetItem();
        _entityGroup[entity->id()] = treeItem;
        guicontainer->addTopLevelItem(treeItem);
        guicontainer->setItemWidget(treeItem, 0, new QLabel(entityInfo));
        treeItem->setExpanded(true);
    }

    createPropertiesWidgets(entity->id(), entity->availableProperties());
    createCustomWidgets(entity);
    createLayerAndMetaTypeWidgets(entity);
}

bool PropertyEditor::addWidget(const std::string& key, api::InputGUI* guiWidget) {
    if (_entityGroup.find(_currentEntity) == _entityGroup.end()) {
        return false;
    }
    QTreeWidget* guicontainer = this->widget()->findChild<QTreeWidget*>("guiContainer");

    bool success = InputGUIContainer::addWidget(key, guiWidget);
    if (success) {
        QTreeWidgetItem* entityChildItem = new QTreeWidgetItem();
        _entityGroup[_currentEntity]->addChild(entityChildItem);
        guicontainer->setItemWidget(entityChildItem, 0, new QLabel(guiWidget->label().c_str()));
        guiWidget->hideLabel();
        guicontainer->setItemWidget(entityChildItem, 1, guiWidget);
        entityChildItem->setSizeHint(1, guiWidget->sizeHint());
    }
    return success;
}

void PropertyEditor::propertyChanged(const std::string& key) {

    sol::table propertiesTable = generateInfo(mainWindow->luaInterface()->luaVM());
    lc::entity::CADEntity_CSPtr entity = mainWindow->cadMdiChild()->storageManager()->entityByID(_widgetKeyToEntity[key]);

    std::shared_ptr<lc::operation::EntityBuilder> entityBuilder = std::make_shared<lc::operation::EntityBuilder>(mainWindow->cadMdiChild()->document());

    // delete old entity
    entityBuilder->appendEntity(entity);
    entityBuilder->appendOperation(std::make_shared<lc::operation::Push>());
    entityBuilder->appendOperation(std::make_shared<lc::operation::Remove>());
    entityBuilder->execute();

    auto lastUnderscore = key.find_last_of("_");
    auto secondLastUnderscore = key.find_last_of("_", lastUnderscore-1);

    std::string entityType = key.substr(lastUnderscore + 1);
    std::string propertyName = key.substr(secondLastUnderscore + 1, lastUnderscore - secondLastUnderscore - 1);

    lc::entity::PropertiesMap propertiesList;

    if (entityType == "angle") {
        propertiesList[propertyName] = lc::entity::AngleProperty(propertiesTable[key].get<double>());
    }

    if (entityType == "double") {
        propertiesList[propertyName] = propertiesTable[key].get<double>();
    }

    if (entityType == "bool") {
        propertiesList[propertyName] = propertiesTable[key].get<bool>();
    }

    if (entityType == "coordinate") {
        propertiesList[propertyName] = propertiesTable[key].get<lc::geo::Coordinate>();
    }

    if (entityType == "text") {
        propertiesList[propertyName] = propertiesTable[key].get<std::string>();
    }

    if (entityType == "vector") {
        propertiesList[propertyName] = propertiesTable[key].get<std::vector<lc::geo::Coordinate>>();
    }

    // returns nullptr if not custom property
    lc::entity::CADEntity_CSPtr changedEntity = customPropertyChanged(key, entityType, propertiesTable, entity);

    if (changedEntity == nullptr) {
        changedEntity = entity->setProperties(propertiesList);
    }

    if (entityType == "lineSelect") {
        changedEntity = entity->modify(entity->layer(), _metaInfoManager->metaInfo(), entity->block());
    }

    if (entityType == "layer") {
        std::string layerName = propertiesTable[key].get<std::string>();
        lc::meta::Layer_CSPtr layer = mainWindow->layers()->layerByName(layerName.c_str());
        if (layer != nullptr) {
            changedEntity = entity->modify(layer, entity->metaInfo(), entity->block());
        }
    }

    // add new entity
    entityBuilder->appendEntity(changedEntity);
    entityBuilder->execute();

    // after entity has been changed
    if (entityType == "vector" || entityType == "customLWPolyline") {
        api::ListGUI* listgui = qobject_cast<api::ListGUI*>(_inputWidgets[key]);
        listgui->guiItemChanged(nullptr, nullptr);
    }

    mainWindow->cadMdiChild()->viewer()->docCanvas()->updateSelection();
}

lc::entity::CADEntity_CSPtr PropertyEditor::customPropertyChanged(const std::string& key, const std::string& entityType, sol::table propertiesTable, lc::entity::CADEntity_CSPtr oldEntity) 
{
    if (entityType == "customLWPolyline") 
    {
        sol::object object = propertiesTable[key];
        if(!object.valid() || !object.is<sol::table>()) return nullptr;
        sol::table vertices = object.as<sol::table>();

        std::vector<lc::builder::LWBuilderVertex> builderVertices;
        for (const auto & vertex : vertices)
        {
            lc::geo::Coordinate loc;
            double sWidth = 0.0;
            double eWidth = 0.0;
            double bulge = 0.0;
            sol::table vertexProperties = vertex.second;

            for (const auto & property : vertexProperties)
            {
                std::string propertyName = property.first.as<std::string>();
                auto lastUnderscore = propertyName.find_last_of('_');
                if(lastUnderscore == std::string::npos) continue;
                std::string propertyType = propertyName.substr(lastUnderscore + 1);

                if (propertyType == "Location") loc = property.second.as<lc::geo::Coordinate>();
                else if (propertyType == "StartWidth") sWidth = property.second.as<double>();
                else if (propertyType == "EndWidth") eWidth = property.second.as<double>();
                else if (propertyType == "Bulge") bulge = property.second.as<double>();
            }

            builderVertices.emplace_back(loc, sWidth, eWidth, bulge, 0);
        }

        lc::builder::LWPolylineBuilder lwPolylineBuilder;
        lwPolylineBuilder.copy(std::dynamic_pointer_cast<const lc::entity::LWPolyline>(oldEntity));
        lwPolylineBuilder.setVertices(builderVertices);

        return lwPolylineBuilder.build();
    }

    return nullptr;
}

void PropertyEditor::createPropertiesWidgets(unsigned long entityID, const lc::entity::PropertiesMap& entityProperties) {
    _currentEntity = entityID;

    sol::state & luaVM = mainWindow->luaInterface()->luaVM();

    for (auto iter = entityProperties.cbegin(); iter != entityProperties.cend(); ++iter) {
        std::string key = generatePropertyKey(entityID, iter->first, iter->second.which());

        if (_addedKeys.find(key) == _addedKeys.end()) {
            // angleproperty
            if (iter->second.which() == 0) {
                lc::ui::api::AngleGUI* anglegui = new lc::ui::api::AngleGUI(std::string(1, std::toupper(iter->first[0])) + iter->first.substr(1));
                anglegui->setValue(boost::get<lc::entity::AngleProperty>(iter->second).Get());
                luaVM.script("anglePropertyCalled = function() lc.PropertyEditor.GetPropertyEditor(mainWindow):propertyChanged('" + key + "') end");
                anglegui->addFinishCallback(luaVM["anglePropertyCalled"]);
                addWidget(key, anglegui);
                luaVM["anglePropertyCalled"] = nullptr;
            }

            // double
            if (iter->second.which() == 1) {
                lc::ui::api::NumberGUI* numbergui = new lc::ui::api::NumberGUI(std::string(1,std::toupper(iter->first[0])) + iter->first.substr(1));
                numbergui->setValue(boost::get<double>(iter->second));
                luaVM.script("numberPropertyCalled = function() lc.PropertyEditor.GetPropertyEditor(mainWindow):propertyChanged('" + key + "') end");
                numbergui->addCallback(luaVM["numberPropertyCalled"]);
                addWidget(key, numbergui);
                luaVM["numberPropertyCalled"] = nullptr;
            }

            // bool
            if (iter->second.which() == 2) {
                lc::ui::api::CheckBoxGUI* checkboxgui = new lc::ui::api::CheckBoxGUI(std::string(1, std::toupper(iter->first[0])) + iter->first.substr(1));
                checkboxgui->setValue(boost::get<bool>(iter->second));
                luaVM.script("boolPropertyCalled = function() lc.PropertyEditor.GetPropertyEditor(mainWindow):propertyChanged('" + key + "') end");
                checkboxgui->addCallback(luaVM["boolPropertyCalled"]);
                addWidget(key, checkboxgui);
                luaVM["boolPropertyCalled"] = nullptr;
            }

            // coordinate
            if (iter->second.which() == 3) {
                lc::ui::api::CoordinateGUI* coordinategui = new lc::ui::api::CoordinateGUI(std::string(1, std::toupper(iter->first[0])) + iter->first.substr(1));
                coordinategui->setValue(boost::get<lc::geo::Coordinate>(iter->second));
                luaVM.script("coordinatePropertyCalled = function() lc.PropertyEditor.GetPropertyEditor(mainWindow):propertyChanged('" + key + "') end");
                coordinategui->addFinishCallback(luaVM["coordinatePropertyCalled"]);
                addWidget(key, coordinategui);
                luaVM["coordinatePropertyCalled"] = nullptr;
            }

            // text (string)
            if (iter->second.which() == 4) {
                lc::ui::api::TextGUI* textgui = new lc::ui::api::TextGUI(std::string(1, std::toupper(iter->first[0])) + iter->first.substr(1));
                textgui->setValue(boost::get<std::string>(iter->second));
                luaVM.script("textPropertyCalled = function() lc.PropertyEditor.GetPropertyEditor(mainWindow):propertyChanged('" + key + "') end");
                textgui->addFinishCallback(luaVM["textPropertyCalled"]);
                addWidget(key, textgui);
                luaVM["textPropertyCalled"] = nullptr;
            }

            // vector
            if (iter->second.which() == 5) {
                lc::ui::api::ListGUI* listgui = new lc::ui::api::ListGUI(std::string(1, std::toupper(iter->first[0])) + iter->first.substr(1), lc::ui::api::ListGUI::ListType::COORDINATE);
                listgui->setMainWindow(mainWindow);

                std::vector<lc::geo::Coordinate> coords = boost::get<std::vector<lc::geo::Coordinate>>(iter->second);
                listgui->setValue(coords);

                luaVM.script("vectorPropertyCalled = function() lc.PropertyEditor.GetPropertyEditor(mainWindow):propertyChanged('" + key + "') end");
                listgui->addCallbackToAll(luaVM["vectorPropertyCalled"]);
                addWidget(key, listgui);
                luaVM["vectorPropertyCalled"] = nullptr;
            }

            _entityProperties[entityID].push_back(key);
            _widgetKeyToEntity[key] = entityID;
        }
    }
}

void PropertyEditor::createCustomWidgets(lc::entity::CADEntity_CSPtr entity) {
    _currentEntity = entity->id();
    sol::state & luaVM = mainWindow->luaInterface()->luaVM();

    api::EntityNameVisitor entityVisitor;
    entity->dispatch(entityVisitor);

    if (entityVisitor.getEntityInformation() == "LWPolyline") {
        std::string key = "entity" + std::to_string(entity->id()) + "_" + "customLWPolyline";
        lc::ui::api::ListGUI* listgui = new lc::ui::api::ListGUI("LWVertex List", lc::ui::api::ListGUI::ListType::LW_VERTEX);
        listgui->setMainWindow(mainWindow);

        lc::builder::LWPolylineBuilder lwPolylineBuilder;
        lwPolylineBuilder.copy(std::dynamic_pointer_cast<const lc::entity::LWPolyline>(entity));
        listgui->setValue(lwPolylineBuilder.getVertices());

        luaVM.script("customPropertyCalled = function() lc.PropertyEditor.GetPropertyEditor(mainWindow):propertyChanged('" + key + "') end");
        listgui->addCallbackToAll(luaVM["customPropertyCalled"]);
        addWidget(key, listgui);
        luaVM["customPropertyCalled"] = nullptr;

        _entityProperties[entity->id()].push_back(key);
        _widgetKeyToEntity[key] = entity->id();
    }
}

void PropertyEditor::createLayerAndMetaTypeWidgets(lc::entity::CADEntity_CSPtr entity) {
    unsigned long entityID = entity->id();
    sol::state & luaVM = mainWindow->luaInterface()->luaVM();

    lc::ui::api::LineSelectGUI* lineSelectGUI = new lc::ui::api::LineSelectGUI(mainWindow->cadMdiChild(), _metaInfoManager, "Meta Info");
    lineSelectGUI->setEntityMetaInfo(entity);
    std::string key = "entity" + std::to_string(entityID) + "_" + "lineSelect";
    luaVM.script("customPropertyCalled = function() lc.PropertyEditor.GetPropertyEditor(mainWindow):propertyChanged('" + key + "') end");

    lineSelectGUI->addCallback(luaVM["customPropertyCalled"]);
    addWidget(key, lineSelectGUI);
    _entityProperties[entityID].push_back(key);
    _widgetKeyToEntity[key] = entityID;

    std::string key2 = "entity" + std::to_string(entityID) + "_" + "layer";
    luaVM.script("customPropertyCalled = function() lc.PropertyEditor.GetPropertyEditor(mainWindow):propertyChanged('" + key2 + "') end");

    std::vector<lc::meta::Layer_CSPtr> layersList = mainWindow->layers()->layers();
    lc::ui::api::ComboBoxGUI* layerGUI = new lc::ui::api::ComboBoxGUI("Layer");
    layerGUI->removeStretch();

    for (lc::meta::Layer_CSPtr layer : layersList) {
        layerGUI->addItem(layer->name());
    }

    layerGUI->setValue(entity->layer()->name());
    layerGUI->addCallback(luaVM["customPropertyCalled"]);
    addWidget(key2, layerGUI);
    _entityProperties[entityID].push_back(key2);
    _widgetKeyToEntity[key2] = entityID;
}

std::string PropertyEditor::generatePropertyKey(unsigned long entityID, const std::string& propKey, int propType) const {
    std::string key = "entity" + std::to_string(entityID) + "_" + propKey;

    switch (propType)
    {
    case 0:
        key += "_angle";
        break;
    case 1:
        key += "_double";
        break;
    case 2:
        key += "_bool";
        break;
    case 3:
        key += "_coordinate";
        break;
    case 4:
        key += "_text";
        break;
    case 5:
        key += "_vector";
        break;
    }

    return key;
}

void PropertyEditor::closeEvent(QCloseEvent* event)
{
    this->widget()->hide();
    event->ignore();
}
