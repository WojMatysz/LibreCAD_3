#include "lc_entity.h"

#include <cad/interface/draggable.h>
#include <cad/interface/snapable.h>
#include <cad/interface/splitable.h>
#include <cad/base/cadentity.h>
#include <cad/primitive/point.h>
#include <cad/primitive/arc.h>
#include <cad/interface/tangentable.h>
#include <cad/primitive/circle.h>
#include <cad/primitive/dimension.h>
#include <cad/primitive/dimaligned.h>
#include <cad/primitive/dimangular.h>
#include <cad/primitive/dimdiametric.h>
#include <cad/primitive/dimlinear.h>
#include <cad/primitive/dimradial.h>
#include <cad/primitive/ellipse.h>
#include <cad/primitive/line.h>
#include <cad/primitive/lwpolyline.h>
#include <cad/primitive/spline.h>
#include <cad/primitive/text.h>
#include <cad/primitive/mtext.h>
#include <cad/primitive/image.h>
#include <cad/primitive/insert.h>
#include <cad/interface/unmanageddraggable.h>
#include <cad/primitive/customentity.h>

//Do split if possible
std::vector<lc::entity::CADEntity_CSPtr> splitHelper(lc::entity::CADEntity_CSPtr e, lc::geo::Coordinate& p) {
    std::vector<lc::entity::CADEntity_CSPtr> out;
    auto splitable = std::dynamic_pointer_cast<const lc::entity::Splitable>(e);
    if (splitable) {
        return splitable->splitEntity(p);
    }
    return out;
}

void import_lc_entity_namespace(sol::state & luaVM) 
{
    sol::table lc = luaVM["lc"];
    lc["entity"] = luaVM.create_table();
    sol::table entity = lc["entity"];

    entity.new_usertype<lc::entity::ID>(
            "ID",
            sol::constructors<lc::entity::ID(), lc::entity::ID(unsigned long)>(),
            "id", &lc::entity::ID::id,
            "setID", &lc::entity::ID::setID
            );

    entity.new_usertype<lc::entity::CADEntity>(
            "CADEntity",
            sol::base_classes, sol::bases<lc::entity::ID, lc::Visitable>(),
            "accept", &lc::entity::CADEntity::accept,
            "block", &lc::entity::CADEntity::block,
            "boundingBox", &lc::entity::CADEntity::boundingBox,
            "copy", &lc::entity::CADEntity::copy,
            "dispatch", &lc::entity::CADEntity::dispatch,
            "layer", &lc::entity::CADEntity::layer,
            "metaInfo", &lc::entity::CADEntity::metaInfo<lc::meta::EntityMetaType>,
            "mirror", &lc::entity::CADEntity::mirror,
            "modify", &lc::entity::CADEntity::modify,
            "move", &lc::entity::CADEntity::move,
            "rotate", &lc::entity::CADEntity::rotate,
            "scale", &lc::entity::CADEntity::scale
            );

    entity.new_usertype<lc::entity::Snapable>(
            "Snapable",
            "nearestPointOnPath", &lc::entity::Snapable::nearestPointOnPath,
            "remove_ifDistanceGreaterThen", &lc::entity::Snapable::remove_ifDistanceGreaterThen,
            "snapPoints", &lc::entity::Snapable::snapPoints,
            "snapPointsCleanup", &lc::entity::Snapable::snapPointsCleanup,
            );

    entity.new_usertype<lc::entity::Draggable>(
            "Draggable",
            "dragPoints", &lc::entity::Draggable::dragPoints,
            "setDragPoints", &lc::entity::Draggable::setDragPoints
            );

    entity.new_usertype<lc::entity::Splitable>(
            "Splitable",
            "splitEntity", &lc::entity::Splitable::splitEntity,
            "splitHelper", &splitHelper
            //Does splitEntity if possible
            );

    entity.new_usertype<lc::entity::Arc>(
            "Arc", 
            sol::base_classes, sol::bases<lc::entity::CADEntity, lc::geo::Arc, lc::entity::Snapable, lc::entity::Draggable, lc::entity::Splitable>(),
            "accept", &lc::entity::Arc::accept,
            "boundingBox", &lc::entity::Arc::boundingBox,
            "copy", &lc::entity::Arc::copy,
            "dispatch", &lc::entity::Arc::dispatch,
            "dragPoints", &lc::entity::Arc::dragPoints,
            "mirror", &lc::entity::Arc::mirror,
            "modify", &lc::entity::Arc::modify,
            "move", &lc::entity::Arc::move,
            "nearestPointOnPath", &lc::entity::Arc::nearestPointOnPath,
            "rotate", &lc::entity::Arc::rotate,
            "scale", &lc::entity::Arc::scale,
            "setDragPoints", &lc::entity::Arc::setDragPoints,
            "snapPoints", &lc::entity::Arc::snapPoints
            );

    //TODO: should be moved to geo
    entity.new_usertype<lc::entity::Tangentable>(
            "Tangentable",
            "lineTangentPointsOnEntity", &lc::entity::Tangentable::lineTangentPointsOnEntity,
            );

    entity.new_usertype<lc::entity::Circle>(
            "Circle",
            sol::base_classes, sol::bases<lc::entity::CADEntity, lc::geo::Circle, lc::entity::Snapable, lc::entity::Splitable>(),
            "accept", &lc::entity::Circle::accept,
            "boundingBox", &lc::entity::Circle::boundingBox,
            "copy", &lc::entity::Circle::copy,
            "dispatch", &lc::entity::Circle::dispatch,
            "mirror", &lc::entity::Circle::mirror,
            "modify", &lc::entity::Circle::modify,
            "move", &lc::entity::Circle::move,
            "nearestPointOnPath", &lc::entity::Circle::nearestPointOnPath,
            "rotate", &lc::entity::Circle::rotate,
            "scale", &lc::entity::Circle::scale,
            "snapPoints", &lc::entity::Circle::snapPoints
            );

    entity.new_usertype<lc::entity::Dimension>(
            "Dimension",
            "attachmentPoint", &lc::entity::Dimension::attachmentPoint,
            "definitionPoint", &lc::entity::Dimension::definitionPoint,
            "explicitValue", &lc::entity::Dimension::explicitValue,
            "lineSpacingFactor", &lc::entity::Dimension::lineSpacingFactor,
            "lineSpacingStyle", &lc::entity::Dimension::lineSpacingStyle,
            "middleOfText", &lc::entity::Dimension::middleOfText,
            "textAngle", &lc::entity::Dimension::textAngle
            );

    entity.new_usertype<lc::entity::Point>(
            "Point",
            sol::base_classes, sol::bases<lc::entity::CADEntity, lc::geo::Coordinate, lc::Visitable>(),
            "accept", &lc::entity::Point::accept,
            "boundingBox", &lc::entity::Point::boundingBox,
            "copy", &lc::entity::Point::copy,
            "dispatch", &lc::entity::Point::dispatch,
            "mirror", &lc::entity::Point::mirror,
            "modify", &lc::entity::Point::modify,
            "move", &lc::entity::Point::move,
            "rotate", &lc::entity::Point::rotate,
            "scale", &lc::entity::Point::scale
            );

    entity.new_usertype<lc::entity::DimAligned>(
            "DimAligned",
            sol::base_classes, sol::bases<lc::entity::CADEntity, lc::entity::Dimension, lc::Visitable, lc::entity::Draggable>(),
            "accept", &lc::entity::DimAligned::accept,
            "boundingBox", &lc::entity::DimAligned::boundingBox,
            "copy", &lc::entity::DimAligned::copy,
            "definitionPoint2", &lc::entity::DimAligned::definitionPoint2,
            "definitionPoint3", &lc::entity::DimAligned::definitionPoint3,
            "dispatch", &lc::entity::DimAligned::dispatch,
            "dragPoints", &lc::entity::DimAligned::dragPoints,
            "mirror", &lc::entity::DimAligned::mirror,
            "modify", &lc::entity::DimAligned::modify,
            "move", &lc::entity::DimAligned::move,
            "rotate", &lc::entity::DimAligned::rotate,
            "scale", &lc::entity::DimAligned::scale,
            "setDragPoints", &lc::entity::DimAligned::setDragPoints
            );

    entity.new_usertype<lc::entity::DimAngular>(
            "DimAngular",
            sol::base_classes, sol::bases<lc::entity::CADEntity, lc::entity::Dimension, lc::Visitable, lc::entity::Draggable>(),
            "accept", &lc::entity::DimAngular::accept,
            "boundingBox", &lc::entity::DimAngular::boundingBox,
            "copy", &lc::entity::DimAngular::copy,
            "defLine11", &lc::entity::DimAngular::defLine11,
            "defLine12", &lc::entity::DimAngular::defLine12,
            "defLine21", &lc::entity::DimAngular::defLine21,
            "defLine22", &lc::entity::DimAngular::defLine22,
            "dispatch", &lc::entity::DimAngular::dispatch,
            "dragPoints", &lc::entity::DimAngular::dragPoints,
            "mirror", &lc::entity::DimAngular::mirror,
            "modify", &lc::entity::DimAngular::modify,
            "move", &lc::entity::DimAngular::move,
            "rotate", &lc::entity::DimAngular::rotate,
            "scale", &lc::entity::DimAngular::scale,
            "setDragPoints", &lc::entity::DimAngular::setDragPoints
            );

    entity.new_usertype<lc::entity::DimDiametric>(
            "DimDiametric",
            sol::base_classes, sol::bases<lc::entity::CADEntity, lc::entity::Dimension, lc::Visitable, lc::entity::Draggable>(),
            "accept", &lc::entity::DimDiametric::accept,
            "boundingBox", &lc::entity::DimDiametric::boundingBox,
            "copy", &lc::entity::DimDiametric::copy,
            "definitionPoint2", &lc::entity::DimDiametric::definitionPoint2,
            "dispatch", &lc::entity::DimDiametric::dispatch,
            "dragPoints", &lc::entity::DimDiametric::dragPoints,
            "leader", &lc::entity::DimDiametric::leader,
            "mirror", &lc::entity::DimDiametric::mirror,
            "modify", &lc::entity::DimDiametric::modify,
            "move", &lc::entity::DimDiametric::move,
            "rotate", &lc::entity::DimDiametric::rotate,
            "scale", &lc::entity::DimDiametric::scale,
            "setDragPoints", &lc::entity::DimDiametric::setDragPoints,
            );

    entity.new_usertype<lc::entity::DimLinear>(
            "DimLinear",
            sol::base_classes, sol::bases<lc::entity::CADEntity, lc::entity::Dimension, lc::Visitable, lc::entity::Draggable>(),
            "accept", &lc::entity::DimLinear::accept,
            "angle", &lc::entity::DimLinear::angle,
            "boundingBox", &lc::entity::DimLinear::boundingBox,
            "copy", &lc::entity::DimLinear::copy,
            "definitionPoint2", &lc::entity::DimLinear::definitionPoint2,
            "definitionPoint3", &lc::entity::DimLinear::definitionPoint3,
            "dispatch", &lc::entity::DimLinear::dispatch,
            "dragPoints", &lc::entity::DimLinear::dragPoints,
            "mirror", &lc::entity::DimLinear::mirror,
            "modify", &lc::entity::DimLinear::modify,
            "move", &lc::entity::DimLinear::move,
            "oblique", &lc::entity::DimLinear::oblique,
            "rotate", &lc::entity::DimLinear::rotate,
            "scale", &lc::entity::DimLinear::scale,
            "setDragPoints", &lc::entity::DimLinear::setDragPoints
            );

    entity.new_usertype<lc::entity::DimRadial>(
            "DimRadial",
            sol::base_classes, sol::bases<lc::entity::CADEntity, lc::entity::Dimension, lc::Visitable, lc::entity::Draggable>(),
            "accept", &lc::entity::DimRadial::accept,
            "boundingBox", &lc::entity::DimRadial::boundingBox,
            "copy", &lc::entity::DimRadial::copy,
            "definitionPoint2", &lc::entity::DimRadial::definitionPoint2,
            "dispatch", &lc::entity::DimRadial::dispatch,
            "dragPoints", &lc::entity::DimRadial::dragPoints,
            "leader", &lc::entity::DimRadial::leader,
            "mirror", &lc::entity::DimRadial::mirror,
            "modify", &lc::entity::DimRadial::modify,
            "move", &lc::entity::DimRadial::move,
            "rotate", &lc::entity::DimRadial::rotate,
            "scale", &lc::entity::DimRadial::scale,
            "setDragPoints", &lc::entity::DimRadial::setDragPoints
            );

    entity.new_usertype<lc::entity::Ellipse>(
            "Ellipse",
            sol::base_classes, sol::bases<lc::entity::CADEntity, lc::geo::Ellipse, lc::entity::Snapable, lc::entity::Splitable>(),
            "accept", &lc::entity::Ellipse::accept,
            "boundingBox", &lc::entity::Ellipse::boundingBox,
            "copy", &lc::entity::Ellipse::copy,
            "dispatch", &lc::entity::Ellipse::dispatch,
            "findBoxPoints", &lc::entity::Ellipse::findBoxPoints,
            "mirror", &lc::entity::Ellipse::mirror,
            "modify", &lc::entity::Ellipse::modify,
            "move", &lc::entity::Ellipse::move,
            "nearestPointOnPath", &lc::entity::Ellipse::nearestPointOnPath,
            "rotate", &lc::entity::Ellipse::rotate,
            "scale", &lc::entity::Ellipse::scale,
            "snapPoints", &lc::entity::Ellipse::snapPoints
            );

    entity.new_usertype<lc::entity::Line>(
            "Line",
            sol::base_classes, sol::bases<lc::entity::CADEntity, lc::geo::Vector, lc::entity::Snapable, lc::entity::Draggable, lc::entity::Splitable>(),
            "accept", &lc::entity::Line::accept,
            "boundingBox", &lc::entity::Line::boundingBox,
            "copy", &lc::entity::Line::copy,
            "dispatch", &lc::entity::Line::dispatch,
            "dragPoints", &lc::entity::Line::dragPoints,
            "mirror", &lc::entity::Line::mirror,
            "modify", &lc::entity::Line::modify,
            "move", &lc::entity::Line::move,
            "nearestPointOnPath", &lc::entity::Line::nearestPointOnPath,
            "rotate", &lc::entity::Line::rotate,
            "scale", &lc::entity::Line::scale,
            "setDragPoints", &lc::entity::Line::setDragPoints,
            "snapPoints", &lc::entity::Line::snapPoints
            );

    entity.new_usertype<lc::entity::LWVertex2D>(
            "LWVertex2D",
            "bulge", &lc::entity::LWVertex2D::bulge,
            "endWidth", &lc::entity::LWVertex2D::endWidth,
            "location", &lc::entity::LWVertex2D::location,
            "move", &lc::entity::LWVertex2D::move,
            "rotate", &lc::entity::LWVertex2D::rotate,
            "scale", &lc::entity::LWVertex2D::scale,
            "startWidth", &lc::entity::LWVertex2D::startWidth,
            );

    entity.new_usertype<lc::entity::LWPolyline>(
            "LWPolyline",
            sol::base_classes, sol::bases<lc::entity::CADEntity, lc::entity::Snapable, lc::entity::Draggable>(),
            "accept", &lc::entity::LWPolyline::accept,
            "asEntities", &lc::entity::LWPolyline::asEntities,
            "boundingBox", &lc::entity::LWPolyline::boundingBox,
            "closed", &lc::entity::LWPolyline::closed,
            "copy", &lc::entity::LWPolyline::copy,
            "dispatch", &lc::entity::LWPolyline::dispatch,
            "dragPoints", &lc::entity::LWPolyline::dragPoints,
            "elevation", &lc::entity::LWPolyline::elevation,
            "extrusionDirection", &lc::entity::LWPolyline::extrusionDirection,
            "mirror", &lc::entity::LWPolyline::mirror,
            "modify", &lc::entity::LWPolyline::modify,
            "move", &lc::entity::LWPolyline::move,
            "nearestPointOnPath", &lc::entity::LWPolyline::nearestPointOnPath,
            "nearestPointOnPath2", &lc::entity::LWPolyline::nearestPointOnPath2,
            "rotate", &lc::entity::LWPolyline::rotate,
            "scale", &lc::entity::LWPolyline::scale,
            "setDragPoints", &lc::entity::LWPolyline::setDragPoints,
            "snapPoints", &lc::entity::LWPolyline::snapPoints,
            "tickness", &lc::entity::LWPolyline::tickness,
            "vertex", &lc::entity::LWPolyline::vertex,
            "width", &lc::entity::LWPolyline::width
                );

    entity.new_usertype<lc::entity::Spline>(
            "Spline",
            sol::base_classes, sol::bases<lc::entity::CADEntity, lc::geo::Spline, lc::entity::Snapable, lc::Visitable, lc::entity::Draggable>(),
            "accept", &lc::entity::Spline::accept,
            "boundingBox", &lc::entity::Spline::boundingBox,
            "copy", &lc::entity::Spline::copy,
            "dispatch", &lc::entity::Spline::dispatch,
            "dragPoints", &lc::entity::Spline::dragPoints,
            "mirror", &lc::entity::Spline::mirror,
            "modify", &lc::entity::Spline::modify,
            "move", &lc::entity::Spline::move,
            "nearestPointOnPath", &lc::entity::Spline::nearestPointOnPath,
            "rotate", &lc::entity::Spline::rotate,
            "scale", &lc::entity::Spline::scale,
            "setDragPoints", &lc::entity::Spline::setDragPoints,
            "snapPoints", &lc::entity::Spline::snapPoints
            );

    entity.new_usertype<lc::entity::TextBase>(
            "TextBase",
            "angle", &lc::entity::TextBase::angle,
            "height", &lc::entity::TextBase::height,
            "insertion_point", &lc::entity::TextBase::insertion_point,
            "style", &lc::entity::TextBase::style,
            "text_value", &lc::entity::TextBase::text_value,
            "textgeneration", &lc::entity::Text::textgeneration,
            "halign", &lc::entity::Text::halign,
            "valign", &lc::entity::Text::valign
            );

    entity.new_usertype<lc::entity::Text>(
            "Text",
            sol::base_classes, sol::bases<lc::entity::CADEntity, lc::entity::TextBase, lc::Visitable, lc::entity::Draggable>(),
            "accept", &lc::entity::Text::accept,
            "boundingBox", &lc::entity::Text::boundingBox,
            "copy", &lc::entity::Text::copy,
            "dispatch", &lc::entity::Text::dispatch,
            "dragPoints", &lc::entity::Text::dragPoints,
            "mirror", &lc::entity::Text::mirror,
            "modify", &lc::entity::Text::modify,
            "move", &lc::entity::Text::move,
            "rotate", &lc::entity::Text::rotate,
            "scale", &lc::entity::Text::scale,
            "setDragPoints", &lc::entity::Text::setDragPoints
            );

    entity.new_usertype<lc::entity::MText>(
            "MText",
            sol::base_classes, sol::bases<lc::entity::CADEntity, lc::entity::TextBase, lc::Visitable, lc::entity::Draggable>(),
            "accept", &lc::entity::MText::accept,
            "boundingBox", &lc::entity::MText::boundingBox,
            "copy", &lc::entity::MText::copy,
            "dispatch", &lc::entity::MText::dispatch,
            "dragPoints", &lc::entity::MText::dragPoints,
            "mirror", &lc::entity::MText::mirror,
            "modify", &lc::entity::MText::modify,
            "move", &lc::entity::MText::move,
            "rotate", &lc::entity::MText::rotate,
            "scale", &lc::entity::MText::scale,
            "setDragPoints", &lc::entity::MText::setDragPoints
            );

    entity.new_usertype<lc::entity::Image>(
            "Image",
            sol::base_classes, sol::bases<lc::entity::CADEntity, lc::entity::Snapable, lc::Visitable>(),
            "accept", &lc::entity::Image::accept,
            "base", &lc::entity::Image::base,
            "boundingBox", &lc::entity::Image::boundingBox,
            "brightness", &lc::entity::Image::brightness,
            "contrast", &lc::entity::Image::contrast,
            "copy", &lc::entity::Image::copy,
            "dispatch", &lc::entity::Image::dispatch,
            "fade", &lc::entity::Image::fade,
            "height", &lc::entity::Image::height,
            "mirror", &lc::entity::Image::mirror,
            "modify", &lc::entity::Image::modify,
            "move", &lc::entity::Image::move,
            "name", &lc::entity::Image::name,
            "nearestPointOnPath", &lc::entity::Image::nearestPointOnPath,
            "rotate", &lc::entity::Image::rotate,
            "scale", &lc::entity::Image::scale,
            "snapPoints", &lc::entity::Image::snapPoints,
            "uv", &lc::entity::Image::uv,
            "vv", &lc::entity::Image::vv,
            "width", &lc::entity::Image::width
                );

    entity.new_usertype<lc::entity::Insert>(
            "Insert",
            sol::base_classes, sol::bases<lc::entity::CADEntity, lc::entity::Snapable, lc::entity::Draggable>(),
            "boundingBox", &lc::entity::Insert::boundingBox,
            "copy", &lc::entity::Insert::copy,
            "dispatch", &lc::entity::Insert::dispatch,
            "displayBlock", &lc::entity::Insert::displayBlock,
            "document", &lc::entity::Insert::document,
            "dragPoints", &lc::entity::Insert::dragPoints,
            "mirror", &lc::entity::Insert::mirror,
            "modify", &lc::entity::Insert::modify,
            "move", &lc::entity::Insert::move,
            "nearestPointOnPath", &lc::entity::Insert::nearestPointOnPath,
            "position", &lc::entity::Insert::position,
            "rotate", &lc::entity::Insert::rotate,
            "scale", &lc::entity::Insert::scale,
            "setDragPoints", &lc::entity::Insert::setDragPoints,
            "snapPoints", &lc::entity::Insert::snapPoints
            );

    entity.new_usertype<lc::entity::UnmanagedDraggable>(
            "UnmanagedDraggable",
            "onDragPointClick", &lc::entity::UnmanagedDraggable::onDragPointClick,
            "onDragPointRelease", &lc::entity::UnmanagedDraggable::onDragPointRelease,
            "setDragPoint", &lc::entity::UnmanagedDraggable::setDragPoint
            );

    entity.new_usertype<lc::entity::CustomEntity>(
            "CustomEntity",
            sol::base_classes, sol::bases<lc::entity::Insert, lc::entity::UnmanagedDraggable>(),
            "copy", &lc::entity::CustomEntity::copy,
            "dragPoints", &lc::entity::CustomEntity::dragPoints,
            "mirror", &lc::entity::CustomEntity::mirror,
            "modify", &lc::entity::CustomEntity::modify,
            "move", &lc::entity::CustomEntity::move,
            "nearestPointOnPath", &lc::entity::CustomEntity::nearestPointOnPath,
            "rotate", &lc::entity::CustomEntity::rotate,
            "scale", &lc::entity::CustomEntity::scale,
            "setDragPoints", &lc::entity::CustomEntity::setDragPoints,
            "snapPoints", &lc::entity::CustomEntity::snapPoints
            );
}
