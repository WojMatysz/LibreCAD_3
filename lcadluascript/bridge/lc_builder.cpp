#include <cad/builders/layer.h>
#include <cad/builders/cadentity.h>
#include <cad/builders/arc.h>
#include <cad/builders/circle.h>
#include <cad/builders/dimension.h>
#include <cad/builders/dimaligned.h>
#include <cad/builders/point.h>
#include <cad/builders/dimangular.h>
#include <cad/builders/dimdiametric.h>
#include <cad/builders/dimradial.h>
#include <cad/builders/ellipse.h>
#include <cad/builders/line.h>
#include <cad/builders/spline.h>
#include <cad/builders/text.h>
#include <cad/builders/mtext.h>
#include <cad/builders/textbase.h>
#include <cad/builders/insert.h>
#include <cad/primitive/insert.h>
#include <cad/primitive/mtext.h>
#include "lc_builder.h"

void import_lc_builder_namespace(sol::state & luaVM) 
{
    sol::table lc = luaVM["lc"];
    lc["builder"] = luaVM.create_table();
    sol::table builder = lc["builder"];

    builder.new_usertype<lc::builder::LinePatternBuilder>(
            "LinePatternBuilder",
            sol::constructors<lc::builder::LinePatternBuilder()>(),
            "addElement", &lc::builder::LinePatternBuilder::addElement,
            "build", &lc::builder::LinePatternBuilder::build,
            "checkValues", &lc::builder::LinePatternBuilder::checkValues,
            "description", &lc::builder::LinePatternBuilder::description,
            "name", &lc::builder::LinePatternBuilder::name,
            "path", &lc::builder::LinePatternBuilder::path,
            "setDescription", &lc::builder::LinePatternBuilder::setDescription,
            "setName", &lc::builder::LinePatternBuilder::setName,
            "setPath", &lc::builder::LinePatternBuilder::setPath
            );



    builder.new_usertype<lc::builder::LayerBuilder>(
            "LayerBuilder",
            sol::constructors<lc::builder::LayerBuilder()>(),
            "build", &lc::builder::LayerBuilder::build,
            "color", &lc::builder::LayerBuilder::color,
            "isFrozen", &lc::builder::LayerBuilder::isFrozen,
            "linePattern", &lc::builder::LayerBuilder::linePattern,
            "lineWidth", &lc::builder::LayerBuilder::lineWidth,
            "name", &lc::builder::LayerBuilder::name,
            "setColor", &lc::builder::LayerBuilder::setColor,
            "setIsFrozen", &lc::builder::LayerBuilder::setIsFrozen,
            "setLinePattern", &lc::builder::LayerBuilder::setLinePattern,
            "setLineWidth", &lc::builder::LayerBuilder::setLineWidth,
            "setName", &lc::builder::LayerBuilder::setName
            );



    builder.new_usertype<lc::builder::CADEntityBuilder>(
            "CADEntityBuilder",
            sol::constructors<lc::builder::CADEntityBuilder()>(),
            "block", &lc::builder::CADEntityBuilder::block,
            "checkValues", &lc::builder::CADEntityBuilder::checkValues,
            "id", &lc::builder::CADEntityBuilder::id,
            "layer", &lc::builder::CADEntityBuilder::layer,
            "metaInfo", &lc::builder::CADEntityBuilder::metaInfo,
            "newID", &lc::builder::CADEntityBuilder::newID,
            "setID", &lc::builder::CADEntityBuilder::setID,
            "setBlock", static_cast<void(lc::builder::CADEntityBuilder::*)(const lc::meta::Block_CSPtr &)>(&lc::builder::CADEntityBuilder::setBlock),
            "setLayer", static_cast<void(lc::builder::CADEntityBuilder::*)(const lc::meta::Layer_CSPtr &)>(&lc::builder::CADEntityBuilder::setLayer),
            "setMetaInfo", static_cast<void(lc::builder::CADEntityBuilder::*)(const lc::meta::MetaInfo_CSPtr &)>(&lc::builder::CADEntityBuilder::setMetaInfo)
            );

    builder.new_usertype<lc::builder::ArcBuilder>(
            "ArcBuilder", 
            sol::base_classes, sol::bases<lc::builder::CADEntityBuilder>(),
            sol::constructors<lc::builder::ArcBuilder()>(),
            "build", &lc::builder::ArcBuilder::build,
            "center", &lc::builder::ArcBuilder::center,
            "endAngle", &lc::builder::ArcBuilder::endAngle,
            "isCCW", &lc::builder::ArcBuilder::isCCW,
            "radius", &lc::builder::ArcBuilder::radius,
            "setCenter", &lc::builder::ArcBuilder::setCenter,
            "setEndAngle", &lc::builder::ArcBuilder::setEndAngle,
            "setIsCCW", &lc::builder::ArcBuilder::setIsCCW,
            "setRadius", &lc::builder::ArcBuilder::setRadius,
            "setStartAngle", &lc::builder::ArcBuilder::setStartAngle,
            "startAngle", &lc::builder::ArcBuilder::startAngle
            );



    builder.new_usertype<lc::builder::CircleBuilder>("CircleBuilder", 
            sol::base_classes, sol::bases<lc::builder::CADEntityBuilder>(),
            sol::constructors<lc::builder::CircleBuilder()>(),
            "build", &lc::builder::CircleBuilder::build,
            "center", &lc::builder::CircleBuilder::center,
            "radius", &lc::builder::CircleBuilder::radius,
            "setCenter", &lc::builder::CircleBuilder::setCenter,
            "setRadius", &lc::builder::CircleBuilder::setRadius,
            "threeTanConstructor", &lc::builder::CircleBuilder::threeTanConstructor,
            "twoTanConstructor", &lc::builder::CircleBuilder::twoTanConstructor,
            "twoTanCircleCenters", &lc::builder::CircleBuilder::twoTanCircleCenters,
            "modifyForTempEntity", &lc::builder::CircleBuilder::modifyForTempEntity
            );



    builder.new_usertype<lc::builder::DimensionBuilder>(
            "DimensionBuilder",
            sol::base_classes, sol::bases<lc::builder::CADEntityBuilder>(),
            "attachmentPoint", &lc::builder::DimensionBuilder::attachmentPoint,
            "definitionPoint", &lc::builder::DimensionBuilder::definitionPoint,
            "explicitValue", &lc::builder::DimensionBuilder::explicitValue,
            "lineSpacingFactor", &lc::builder::DimensionBuilder::lineSpacingFactor,
            "lineSpacingStyle", &lc::builder::DimensionBuilder::lineSpacingStyle,
            "middleOfText", &lc::builder::DimensionBuilder::middleOfText,
            "setAttachmentPoint", &lc::builder::DimensionBuilder::setAttachmentPoint,
            "setDefinitionPoint", &lc::builder::DimensionBuilder::setDefinitionPoint,
            "setExplicitValue", &lc::builder::DimensionBuilder::setExplicitValue,
            "setLineSpacingFactor", &lc::builder::DimensionBuilder::setLineSpacingFactor,
            "setLineSpacingStyle", &lc::builder::DimensionBuilder::setLineSpacingStyle,
            "setMiddleOfText", &lc::builder::DimensionBuilder::setMiddleOfText,
            "setTextAngle", &lc::builder::DimensionBuilder::setTextAngle,
            "textAngle", &lc::builder::DimensionBuilder::textAngle
            );

    builder.new_usertype<lc::builder::PointBuilder>(
            "PointBuilder",
            sol::base_classes, sol::bases<lc::builder::CADEntityBuilder>(),
            sol::constructors<lc::builder::PointBuilder()>(),
            "build", &lc::builder::PointBuilder::build,
            "coordinate", &lc::builder::PointBuilder::coordinate,
            "setCoordinate", &lc::builder::PointBuilder::setCoordinate
            );

    builder.new_usertype<lc::builder::DimAlignedBuilder>(
            "DimAlignedBuilder",
            sol::base_classes, sol::bases<lc::builder::DimensionBuilder>(),
            sol::constructors<lc::builder::DimAlignedBuilder()>(),
            "build", &lc::builder::DimAlignedBuilder::build,
            "definitionPoint2", &lc::builder::DimAlignedBuilder::definitionPoint2,
            "definitionPoint3", &lc::builder::DimAlignedBuilder::definitionPoint3,
            "dimAuto", &lc::builder::DimAlignedBuilder::dimAuto,
            "setDefinitionPoint2", &lc::builder::DimAlignedBuilder::setDefinitionPoint2,
            "setDefinitionPoint3", &lc::builder::DimAlignedBuilder::setDefinitionPoint3
            );

    builder.new_usertype<lc::builder::DimAngularBuilder>(
            "DimAngularBuilder",
            sol::base_classes, sol::bases<lc::builder::DimensionBuilder>(),
            sol::constructors<lc::builder::DimAngularBuilder()>(),
            "build", &lc::builder::DimAngularBuilder::build,
            "defLine11", &lc::builder::DimAngularBuilder::defLine11,
            "defLine12", &lc::builder::DimAngularBuilder::defLine12,
            "defLine21", &lc::builder::DimAngularBuilder::defLine21,
            "defLine22", &lc::builder::DimAngularBuilder::defLine22,
            "dimAuto", &lc::builder::DimAngularBuilder::dimAuto,
            "setDefLine11", &lc::builder::DimAngularBuilder::setDefLine11,
            "setDefLine12", &lc::builder::DimAngularBuilder::setDefLine12,
            "setDefLine21", &lc::builder::DimAngularBuilder::setDefLine21,
            "setDefLine22", &lc::builder::DimAngularBuilder::setDefLine22
            );



    builder.new_usertype<lc::builder::DimDiametricBuilder>(
            "DimDiametricBuilder",
            sol::base_classes, sol::bases<lc::builder::DimensionBuilder>(),
            sol::constructors<lc::builder::DimDiametricBuilder()>(),
            "build", &lc::builder::DimDiametricBuilder::build,
            "definitionPoint2", &lc::builder::DimDiametricBuilder::definitionPoint2,
            "leader", &lc::builder::DimDiametricBuilder::leader,
            "setDefinitionPoint2", &lc::builder::DimDiametricBuilder::setDefinitionPoint2,
            "setLeader", &lc::builder::DimDiametricBuilder::setLeader
            );



    builder.new_usertype<lc::builder::DimLinearBuilder>(
            "DimLinearBuilder",,
            sol::base_classes, sol::bases<lc::builder::DimensionBuilder>(),
            sol::constructors<lc::builder::DimLinearBuilder()>(),
            "angle", &lc::builder::DimLinearBuilder::angle,
            "build", &lc::builder::DimLinearBuilder::build,
            "definitionPoint2", &lc::builder::DimLinearBuilder::definitionPoint2,
            "definitionPoint3", &lc::builder::DimLinearBuilder::definitionPoint3,
            "oblique", &lc::builder::DimLinearBuilder::oblique,
            "setAngle", &lc::builder::DimLinearBuilder::setAngle,
            "setDefinitionPoint2", &lc::builder::DimLinearBuilder::setDefinitionPoint2,
            "setDefinitionPoint3", &lc::builder::DimLinearBuilder::setDefinitionPoint3,
            "setOblique", &lc::builder::DimLinearBuilder::setOblique
            );




    builder.new_usertype<lc::builder::DimRadialBuilder>(
            "DimRadialBuilder",
            sol::base_classes, sol::bases<lc::builder::DimensionBuilder>(),
            sol::constructors<lc::builder::DimRadialBuilder()>(),
            "build", &lc::builder::DimRadialBuilder::build,
            "definitionPoint2", &lc::builder::DimRadialBuilder::definitionPoint2,
            "dimAuto", &lc::builder::DimRadialBuilder::dimAuto,
            "leader", &lc::builder::DimRadialBuilder::leader,
            "setDefinitionPoint2", &lc::builder::DimRadialBuilder::setDefinitionPoint2,
            "setLeader", &lc::builder::DimRadialBuilder::setLeader
            );





    builder.new_usertype<lc::builder::EllipseBuilder>(
            "EllipseBuilder",
            sol::base_classes, sol::bases<lc::builder::CADEntityBuilder>(),
            sol::constructors<lc::builder::EllipseBuilder()>(),
            "build", &lc::builder::EllipseBuilder::build,
            "center", &lc::builder::EllipseBuilder::center,
            "endAngle", &lc::builder::EllipseBuilder::endAngle,
            "isReversed", &lc::builder::EllipseBuilder::isReversed,
            "majorPoint", &lc::builder::EllipseBuilder::majorPoint,
            "minorRadius", &lc::builder::EllipseBuilder::minorRadius,
            "setCenter", &lc::builder::EllipseBuilder::setCenter,
            "setEndAngle", &lc::builder::EllipseBuilder::setEndAngle,
            "setIsReversed", &lc::builder::EllipseBuilder::setIsReversed,
            "setMajorPoint", &lc::builder::EllipseBuilder::setMajorPoint,
            "setMinorRadius", &lc::builder::EllipseBuilder::setMinorRadius,
            "setStartAngle", &lc::builder::EllipseBuilder::setStartAngle,
            "startAngle", &lc::builder::EllipseBuilder::startAngle
            );



    builder.new_usertype<lc::builder::LineBuilder>(
            "LineBuilder", 
            sol::base_classes, sol::bases<lc::builder::CADEntityBuilder>(),
            sol::constructors<lc::builder::LineBuilder()>(),
            "build", &lc::builder::LineBuilder::build,
            "endPoint", &lc::builder::LineBuilder::end,
            "setEndPoint", &lc::builder::LineBuilder::setEnd,
            "setStartPoint", &lc::builder::LineBuilder::setStart,
            "startPoint", &lc::builder::LineBuilder::start,
            "getLineAngle", &lc::builder::LineBuilder::getLineAngle
            );



    builder.new_usertype<lc::builder::SplineBuilder>(
            "SplineBuilder", 
            sol::base_classes, sol::bases<lc::builder::CADEntityBuilder>(),
            sol::constructors<lc::builder::SplineBuilder()>(),
            "addControlPoint", &lc::builder::SplineBuilder::addControlPoint,
            "addFitPoint", &lc::builder::SplineBuilder::addFitPoint,
            "addKnotPoint", &lc::builder::SplineBuilder::addKnotPoint,
            "build", &lc::builder::SplineBuilder::build,
            "closed", &lc::builder::SplineBuilder::closed,
            "controlPoints", &lc::builder::SplineBuilder::controlPoints,
            "degree", &lc::builder::SplineBuilder::degree,
            "endTangent", &lc::builder::SplineBuilder::endTangent,
            "fitPoints", &lc::builder::SplineBuilder::fitPoints,
            "fitTolerance", &lc::builder::SplineBuilder::fitTolerance,
            "flags", &lc::builder::SplineBuilder::flags,
            "knotPoints", &lc::builder::SplineBuilder::knotPoints,
            "normalVector", &lc::builder::SplineBuilder::normalVector,
            "removeControlPoint", &lc::builder::SplineBuilder::removeControlPoint,
            "removeFitPoint", &lc::builder::SplineBuilder::removeFitPoint,
            "removeKnotPoint", &lc::builder::SplineBuilder::removeKnotPoint,
            "setClosed", &lc::builder::SplineBuilder::setClosed,
            "setControlPoints", &lc::builder::SplineBuilder::setControlPoints,
            "setDegree", &lc::builder::SplineBuilder::setDegree,
            "setEndTangent", &lc::builder::SplineBuilder::setEndTangent,
            "setFitPoints", &lc::builder::SplineBuilder::setFitPoints,
            "setFitTolerance", &lc::builder::SplineBuilder::setFitTolerance,
            "setFlags", &lc::builder::SplineBuilder::setFlags,
            "setKnotPoints", &lc::builder::SplineBuilder::setKnotPoints,
            "setNormalVector", &lc::builder::SplineBuilder::setNormalVector,
            "setStartTangent", &lc::builder::SplineBuilder::setStartTangent,
            "startTangent", &lc::builder::SplineBuilder::startTangent
                );

    builder.new_usertype<lc::builder::LWPolylineBuilder>(
            "LWPolylineBuilder",
            sol::base_classes, sol::bases<lc::builder::CADEntityBuilder>(),
            sol::constructors<lc::builder::LWPolylineBuilder()>(),
            "addLineVertex", &lc::builder::LWPolylineBuilder::addLineVertex,
            "addArcVertex", &lc::builder::LWPolylineBuilder::addArcVertex,
            "modifyLastVertex", &lc::builder::LWPolylineBuilder::modifyLastVertex,
            "modifyLastVertexArc", &lc::builder::LWPolylineBuilder::modifyLastVertexArc,
            "modifyLastVertexLine", &lc::builder::LWPolylineBuilder::modifyLastVertexLine,
            "removeVertex", &lc::builder::LWPolylineBuilder::removeVertex,
            "getVertices", &lc::builder::LWPolylineBuilder::getVertices,
            "setWidth", &lc::builder::LWPolylineBuilder::setWidth,
            "build", &lc::builder::LWPolylineBuilder::build
            );

    builder.new_usertype<lc::builder::InsertBuilder>(
            "InsertBuilder", 
            sol::base_classes, sol::bases<lc::builder::CADEntityBuilder>(),
            sol::constructors<lc::builder::InsertBuilder()>(),
            "build", &lc::builder::InsertBuilder::build,
            "checkValues", &lc::builder::InsertBuilder::checkValues,
            "coordinate", &lc::builder::InsertBuilder::coordinate,
            "displayBlock", &lc::builder::InsertBuilder::displayBlock,
            "document", &lc::builder::InsertBuilder::document,
            "setCoordinate", &lc::builder::InsertBuilder::setCoordinate,
            "setDisplayBlock", &lc::builder::InsertBuilder::setDisplayBlock,
            "setDocument", &lc::builder::InsertBuilder::setDocument
            );

    builder.new_usertype<lc::builder::TextBaseBuilder>(
            "TextBaseBuilder",
            sol::base_classes, sol::bases<lc::builder::CADEntityBuilder>(),
            sol::constructors<lc::builder::TextBaseBuilder()>(),
            "insertionPoint", &lc::builder::TextBaseBuilder::insertionPoint,
            "setInsertionPoint", &lc::builder::TextBaseBuilder::setInsertionPoint,
            "textValue", &lc::builder::TextBaseBuilder::textValue,
            "setTextValue", &lc::builder::TextBaseBuilder::setTextValue,
            "height", &lc::builder::TextBaseBuilder::height,
            "setHeight", &lc::builder::TextBaseBuilder::setHeight,
            "angle", &lc::builder::TextBaseBuilder::angle,
            "setAngle", &lc::builder::TextBaseBuilder::setAngle
            );

    builder.new_usertype<lc::builder::TextBuilder>("TextBuilder", 
            sol::base_classes, sol::bases<lc::builder::TextBaseBuilder>(),
            sol::constructors<lc::builder::TextBuilder()>(),
            "build", &lc::builder::TextBuilder::build,
            "copy", &lc::builder::TextBuilder::copy
            );

    builder.new_usertype<lc::builder::MTextBuilder>("MTextBuilder", 
            sol::base_classes, sol::bases<lc::builder::TextBaseBuilder>(),
            sol::constructors<lc::builder::MTextBuilder()>(),
            "build", &lc::builder::MTextBuilder::build,
            "copy", &lc::builder::MTextBuilder::copy
            );
}
