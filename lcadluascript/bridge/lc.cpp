#include <cad/meta/color.h>
#include <cad/base/visitor.h>
#include <cad/interface/entitydispatch.h>
#include <cad/vo/entitycoordinate.h>
#include <cad/interface/snapconstrain.h>
#include <cad/vo/entitydistance.h>
#include "lc.h"

void import_lc_namespace(sol::state & luaVM) {
    luaVM["lc"] = luaVM.create_table();
    sol::table lcTable = luaVM["lc"];

    lcTable.new_usertype<lc::Visitable>("Visitable", "accept", &lc::Visitable::accept);

    lcTable.new_usertype<lc::Color>(
            "Color", 
            sol::constructors<lc::Color(), lc::Color(int, int, int, int), lc::Color(double, double, double, double), lc::Color(const lc::Color &)>(),
            "alpha", &lc::Color::alpha,
            "alphaI", &lc::Color::alphaI,
            "blue", &lc::Color::blue,
            "blueI", &lc::Color::blueI,
            "green", &lc::Color::green,
            "greenI", &lc::Color::greenI,
            "red", &lc::Color::red,
            "redI", &lc::Color::redI
            );

    lcTable.new_usertype<lc::EntityDispatch>(
            "EntityDispatch", 
            "visit", sol::overload(
                static_cast<void(lc::EntityDispatch::*)(lc::entity::Line_CSPtr)>(&lc::EntityDispatch::visit), 
                static_cast<void(lc::EntityDispatch::*)(lc::entity::Point_CSPtr)>(&lc::EntityDispatch::visit), 
                static_cast<void(lc::EntityDispatch::*)(lc::entity::Circle_CSPtr)>(&lc::EntityDispatch::visit), 
                static_cast<void(lc::EntityDispatch::*)(lc::entity::Arc_CSPtr)>(&lc::EntityDispatch::visit), 
                static_cast<void(lc::EntityDispatch::*)(lc::entity::Ellipse_CSPtr)>(&lc::EntityDispatch::visit), 
                static_cast<void(lc::EntityDispatch::*)(lc::entity::Text_CSPtr)>(&lc::EntityDispatch::visit), 
                static_cast<void(lc::EntityDispatch::*)(lc::entity::MText_CSPtr)>(&lc::EntityDispatch::visit), 
                static_cast<void(lc::EntityDispatch::*)(lc::entity::Spline_CSPtr)>(&lc::EntityDispatch::visit), 
                static_cast<void(lc::EntityDispatch::*)(lc::entity::DimAligned_CSPtr)>(&lc::EntityDispatch::visit), 
                static_cast<void(lc::EntityDispatch::*)(lc::entity::DimAngular_CSPtr)>(&lc::EntityDispatch::visit), 
                static_cast<void(lc::EntityDispatch::*)(lc::entity::DimDiametric_CSPtr)>(&lc::EntityDispatch::visit), 
                static_cast<void(lc::EntityDispatch::*)(lc::entity::DimLinear_CSPtr)>(&lc::EntityDispatch::visit), 
                static_cast<void(lc::EntityDispatch::*)(lc::entity::DimRadial_CSPtr)>(&lc::EntityDispatch::visit), 
                static_cast<void(lc::EntityDispatch::*)(lc::entity::LWPolyline_CSPtr)>(&lc::EntityDispatch::visit), 
                static_cast<void(lc::EntityDispatch::*)(lc::entity::Image_CSPtr)>(&lc::EntityDispatch::visit)
                )
            );

    lcTable.new_usertype<lc::EntityCoordinate>(
            "EntityCoordinate",
            sol::constructors<lc::EntityCoordinate(const lc::geo::Coordinate &, int), lc::EntityCoordinate(const lc::EntityCoordinate &)>(),
            "coordinate", &lc::EntityCoordinate::coordinate, 
            "pointId", &lc::EntityCoordinate::pointId
            );

    lcTable.new_usertype<lc::SimpleSnapConstrain>("SimpleSnapConstrain",
            sol::constructors<lc::SimpleSnapConstrain(), lc::SimpleSnapConstrain(uint16_t, int, double)>(),
            "angle", &lc::SimpleSnapConstrain::angle,
            "constrain", &lc::SimpleSnapConstrain::constrain,
            "disableConstrain", &lc::SimpleSnapConstrain::disableConstrain,
            "divisions", &lc::SimpleSnapConstrain::divisions,
            "enableConstrain", &lc::SimpleSnapConstrain::enableConstrain,
            "hasConstrain", &lc::SimpleSnapConstrain::hasConstrain,
            "setAngle", &lc::SimpleSnapConstrain::setAngle,
            "setDivisions", &lc::SimpleSnapConstrain::setDivisions
            );

    lcTable.new_usertype<lc::EntityDistance>(
            "EntityDistance",
            sol::constructors<lc::EntityDistance(lc::entity::CADEntity_CSPtr, const lc::geo::Coordinate &)>(),
            "coordinate", &lc::EntityDistance::coordinate,
            "entity", &lc::EntityDistance::entity
            );
}
