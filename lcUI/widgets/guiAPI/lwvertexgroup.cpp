#include "lwvertexgroup.h"

using namespace lc::ui::api;

LWVertexGroup::LWVertexGroup(std::string label, QWidget* parent)
    :
    HorizontalGroupGUI(label, true, parent)
{
}

void LWVertexGroup::addCallback(sol::function cb) {
    coordgui->addFinishCallback(cb);
    startWidth->addCallback(cb);
    endWidth->addCallback(cb);
}

void LWVertexGroup::setMainWindow(lc::ui::MainWindow* mainWindowIn) {
    coordgui->enableCoordinateSelection(mainWindowIn);
}

void LWVertexGroup::getLuaValue(sol::table & table) {
    HorizontalGroupGUI::getLuaValue(table);
    sol::state_view luaVM = table.lua_state();
    table[_key] = luaVM.create_table();

    table[_key][_key + "_Location"] = table[_key + "_Location"];
    table[_key][_key + "_StartWidth"] = table[_key + "_StartWidth"];
    table[_key][_key + "_EndWidth"] = table[_key + "_EndWidth"];
    table[_key][_key + "_Bulge"] = _bulge;
}

void LWVertexGroup::setKey(const std::string& keyIn) {
    InputGUI::setKey(keyIn);

    coordgui = new lc::ui::api::CoordinateGUI("Location");
    startWidth = new lc::ui::api::NumberGUI("Start Width");
    endWidth = new lc::ui::api::NumberGUI("End Width");

    coordgui->hideLabel();
    startWidth->layout()->removeItem(startWidth->layout()->itemAt(1));
    endWidth->layout()->removeItem(endWidth->layout()->itemAt(1));
    qboxlayout->setContentsMargins(QMargins(0, 0, 0, 0));

    addWidget(_key + "_Location", coordgui);
    addWidget(_key + "_StartWidth", startWidth);
    addWidget(_key + "_EndWidth", endWidth);
}

void LWVertexGroup::setValue(lc::geo::Coordinate coord, double sw, double ew, double bulge) {
    coordgui->setValue(coord);
    startWidth->setValue(sw);
    endWidth->setValue(ew);
    _bulge = bulge;
}

lc::geo::Coordinate LWVertexGroup::location() const {
    return coordgui->value();
}
