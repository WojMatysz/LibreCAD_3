#include <cad/geometry/geocoordinate.h>
#include <cad/geometry/geobase.h>
#include <cad/geometry/geovector.h>
#include <cad/geometry/geobezierbase.h>
#include <cad/geometry/geobezier.h>
#include <cad/geometry/geoarc.h>
#include <cad/geometry/geobeziercubic.h>
#include <cad/interface/tangentable.h>
#include <cad/geometry/geoellipse.h>
#include <cad/geometry/geocircle.h>
#include <cad/geometry/geospline.h>
#include <cad/geometry/georegion.h>
#include "lc_geo.h"

void import_lc_geo_namespace(sol::state & luaVM) {
    sol::table lc = luaVM["lc"];
    lc["geo"] = luaVM.create_table();
    sol::table geo = lc["geo"];

    geo.new_usertype<lc::geo::Coordinate>(
            "Coordinate",
            sol::constructors<lc::geo::Coordinate(), lc::geo::Coordinate(double, double, double), 
            lc::geo::Coordinate(double, double), lc::geo::Coordinate(double), lc::geo::Coordinate(const lc::geo::Coordinate &)>(),
            "angle", &lc::geo::Coordinate::angle,
            "angleBetween", &lc::geo::Coordinate::angleBetween,
            "angleTo", &lc::geo::Coordinate::angleTo,
            "distanceTo", &lc::geo::Coordinate::distanceTo,
            "dot", sol::overload(
                static_cast<double(lc::geo::Coordinate::*)(const lc::geo::Coordinate &) const>(&lc::geo::Coordinate::dot), 
                static_cast<double(lc::geo::Coordinate::*)(const lc::geo::Coordinate &, const lc::geo::Coordinate &) const>(&lc::geo::Coordinate::dot)
                ),
            "flipXY", &lc::geo::Coordinate::flipXY,
            "magnitude", &lc::geo::Coordinate::magnitude,
            "mid", &lc::geo::Coordinate::mid,
            "mirror", &lc::geo::Coordinate::mirror,
            "move", &lc::geo::Coordinate::move,
            "moveTo", &lc::geo::Coordinate::moveTo,
            "norm", sol::overload(
                static_cast<lc::geo::Coordinate(lc::geo::Coordinate::*)() const>(&lc::geo::Coordinate::norm), 
                static_cast<lc::geo::Coordinate(lc::geo::Coordinate::*)(const double) const>(&lc::geo::Coordinate::norm)
                ),
            "rotate", sol::overload(
                    static_cast<lc::geo::Coordinate(lc::geo::Coordinate::*)(const lc::geo::Coordinate &) const>(&lc::geo::Coordinate::rotate), 
                    static_cast<lc::geo::Coordinate(lc::geo::Coordinate::*)(const double &) const>(&lc::geo::Coordinate::rotate), 
                    static_cast<lc::geo::Coordinate(lc::geo::Coordinate::*)(const lc::geo::Coordinate &, const lc::geo::Coordinate &) const>(&lc::geo::Coordinate::rotate), 
                    static_cast<lc::geo::Coordinate(lc::geo::Coordinate::*)(const lc::geo::Coordinate &, const double &) const>(&lc::geo::Coordinate::rotate)
                    ),
            "rotateByArcLength", &lc::geo::Coordinate::rotateByArcLength,
            "scale", sol::overload(
                    static_cast<lc::geo::Coordinate(lc::geo::Coordinate::*)(const double &) const>(&lc::geo::Coordinate::scale), 
                    static_cast<lc::geo::Coordinate(lc::geo::Coordinate::*)(const lc::geo::Coordinate &) const>(&lc::geo::Coordinate::scale), 
                    static_cast<lc::geo::Coordinate(lc::geo::Coordinate::*)(const lc::geo::Coordinate &, const lc::geo::Coordinate &) const>(&lc::geo::Coordinate::scale)
                    ),
            "squared", &lc::geo::Coordinate::squared,
            "transform2d", &lc::geo::Coordinate::transform2d,
            "x", &lc::geo::Coordinate::x,
            "y", &lc::geo::Coordinate::y,
            "z", &lc::geo::Coordinate::z,
            "multiply", [](lc::geo::Coordinate coordinate, double s) { return coordinate * s; },
            "add", [](lc::geo::Coordinate coordinate, lc::geo::Coordinate o) { return coordinate + o; },
            "sub", [](lc::geo::Coordinate coordinate, lc::geo::Coordinate o) { return coordinate - o; }
            );

    geo.new_usertype<lc::geo::Base>("Base");

    geo.new_usertype<lc::geo::Vector>(
            "Vector",
            sol::constructors<lc::geo::Vector(const lc::geo::Coordinate &, const lc::geo::Coordinate &), lc::geo::Vector(const lc::geo::Vector &)>(),
            sol::base_classes, sol::bases<lc::geo::Base, lc::Visitable>(),
            "Angle1", &lc::geo::Vector::Angle1,
            "Angle2", &lc::geo::Vector::Angle2,
            "accept", &lc::geo::Vector::accept,
            "end", &lc::geo::Vector::end,
            "equation", &lc::geo::Vector::equation,
            "nearestPointOnEntity", &lc::geo::Vector::nearestPointOnEntity,
            "nearestPointOnPath", &lc::geo::Vector::nearestPointOnPath,
            "start", &lc::geo::Vector::start
            );

    geo.new_usertype<lc::geo::Area>(
            "Area",
            sol::constructors<lc::geo::Area(const lc::geo::Coordinate &, const lc::geo::Coordinate &), lc::geo::Area(), 
            lc::geo::Area(const lc::geo::Coordinate &, double, double)>(),
            sol::base_classes, sol::bases<lc::geo::Base, lc::Visitable>(),
            "accept", &lc::geo::Area::accept,
            "bottom", &lc::geo::Area::bottom,
            "height", &lc::geo::Area::height,
            "inArea", static_cast<bool(lc::geo::Area::*)(const lc::geo::Coordinate &, double) const>(&lc::geo::Area::inArea), static_cast<bool(lc::geo::Area::*)(const lc::geo::Area &) const>(&lc::geo::Area::inArea),
            "increaseBy", &lc::geo::Area::increaseBy,
            "intersection", &lc::geo::Area::intersection,
            "left", &lc::geo::Area::left,
            "maxP", &lc::geo::Area::maxP,
            "merge", static_cast<lc::geo::Area(lc::geo::Area::*)(const lc::geo::Area &) const>(&lc::geo::Area::merge), static_cast<lc::geo::Area(lc::geo::Area::*)(const lc::geo::Coordinate &) const>(&lc::geo::Area::merge),
            "minP", &lc::geo::Area::minP,
            "numCornersInside", &lc::geo::Area::numCornersInside,
            "overlaps", &lc::geo::Area::overlaps,
            "right", &lc::geo::Area::right,
            "top", &lc::geo::Area::top,
            "width", &lc::geo::Area::width
            );

    geo.new_usertype<lc::geo::Arc>(
            "Arc",
            sol::constructors<lc::geo::Arc(lc::geo::Coordinate, double, double, double, bool), lc::geo::Arc(const lc::geo::Arc &)>(),
            sol::base_classes, sol::bases<lc::geo::Base, lc::Visitable>(),
            "CCW", &lc::geo::Arc::CCW,
            "accept", &lc::geo::Arc::accept,
            "angle", &lc::geo::Arc::angle,
            "boundingBox", &lc::geo::Arc::boundingBox,
            "bulge", &lc::geo::Arc::bulge,
            "center", &lc::geo::Arc::center,
            "createArc3P", &lc::geo::Arc::createArc3P,
            "createArcBulge", &lc::geo::Arc::createArcBulge,
            "endAngle", &lc::geo::Arc::endAngle,
            "endP", &lc::geo::Arc::endP,
            "equation", &lc::geo::Arc::equation,
            "isAngleBetween", &lc::geo::Arc::isAngleBetween,
            "length", &lc::geo::Arc::length,
            "nearestPointOnEntity", &lc::geo::Arc::nearestPointOnEntity,
            "nearestPointOnPath", &lc::geo::Arc::nearestPointOnPath,
            "radius", &lc::geo::Arc::radius,
            "startAngle", &lc::geo::Arc::startAngle,
            "startP", &lc::geo::Arc::startP
            );

    geo.new_usertype<lc::geo::BezierBase>(
            "BezierBase",
            sol::base_classes, sol::bases<lc::geo::Base, lc::Visitable>(),
            "CasteljauAt", &lc::geo::BezierBase::CasteljauAt,
            "Curve", &lc::geo::BezierBase::Curve,
            "DirectValueAt", &lc::geo::BezierBase::DirectValueAt,
            "accept", &lc::geo::BezierBase::accept,
            "boundingBox", &lc::geo::BezierBase::boundingBox,
            "getCP", &lc::geo::BezierBase::getCP,
            "length", &lc::geo::BezierBase::length,
            "mirror", &lc::geo::BezierBase::mirror,
            "move", &lc::geo::BezierBase::move,
            "nearestPointOnEntity", &lc::geo::BezierBase::nearestPointOnEntity,
            "nearestPointOnPath", &lc::geo::BezierBase::nearestPointOnPath,
            "nearestPointTValue", &lc::geo::BezierBase::nearestPointTValue,
            "normal", &lc::geo::BezierBase::normal,
            "offset", &lc::geo::BezierBase::offset,
            "returnCasesForNearestPoint", &lc::geo::BezierBase::returnCasesForNearestPoint,
            "rotate", &lc::geo::BezierBase::rotate,
            "scale", &lc::geo::BezierBase::scale,
            "splitAtT", &lc::geo::BezierBase::splitAtT,
            "splitHalf", &lc::geo::BezierBase::splitHalf,
            "tangent", &lc::geo::BezierBase::tangent
            );

    geo.new_usertype<lc::geo::Bezier>(
            "Bezier", 
            sol::constructors<lc::geo::Bezier(lc::geo::Coordinate, lc::geo::Coordinate, lc::geo::Coordinate), lc::geo::Bezier(const lc::geo::Bezier &)>(),
            sol::base_classes, sol::bases<lc::geo::BezierBase>(),
            "CasteljauAt", &lc::geo::Bezier::CasteljauAt,
            "Curve", &lc::geo::Bezier::Curve,
            "DirectValueAt", &lc::geo::Bezier::DirectValueAt,
            "boundingBox", &lc::geo::Bezier::boundingBox,
            "getCP", &lc::geo::Bezier::getCP,
            "length", &lc::geo::Bezier::length,
            "mirror", &lc::geo::Bezier::mirror,
            "move", &lc::geo::Bezier::move,
            "nearestPointOnEntity", &lc::geo::Bezier::nearestPointOnEntity,
            "nearestPointOnPath", &lc::geo::Bezier::nearestPointOnPath,
            "normal", &lc::geo::Bezier::normal,
            "offset", &lc::geo::Bezier::offset,
            "rotate", &lc::geo::Bezier::rotate,
            "scale", &lc::geo::Bezier::scale,
            "splitAtT", &lc::geo::Bezier::splitAtT,
            "splitHalf", &lc::geo::Bezier::splitHalf,
            "tangent", &lc::geo::Bezier::tangent
            );

    geo.new_usertype<lc::geo::CubicBezier>(
            "CubicBezier", 
            sol::constructors<lc::geo::CubicBezier(lc::geo::Coordinate, lc::geo::Coordinate, lc::geo::Coordinate, lc::geo::Coordinate), 
            lc::geo::CubicBezier(const lc::geo::CubicBezier &)>(),
            sol::base_classes, sol::bases<lc::geo::BezierBase>(),
            "CasteljauAt", &lc::geo::CubicBezier::CasteljauAt,
            "Curve", &lc::geo::CubicBezier::Curve,
            "DirectValueAt", &lc::geo::CubicBezier::DirectValueAt,
            "boundingBox", &lc::geo::CubicBezier::boundingBox,
            "getCP", &lc::geo::CubicBezier::getCP,
            "length", &lc::geo::CubicBezier::length,
            "mirror", &lc::geo::CubicBezier::mirror,
            "move", &lc::geo::CubicBezier::move,
            "nearestPointOnEntity", &lc::geo::CubicBezier::nearestPointOnEntity,
            "nearestPointOnPath", &lc::geo::CubicBezier::nearestPointOnPath,
            "normal", &lc::geo::CubicBezier::normal,
            "offset", &lc::geo::CubicBezier::offset,
            "rotate", &lc::geo::CubicBezier::rotate,
            "scale", &lc::geo::CubicBezier::scale,
            "splitAtT", &lc::geo::CubicBezier::splitAtT,
            "splitHalf", &lc::geo::CubicBezier::splitHalf,
            "tangent", &lc::geo::CubicBezier::tangent
            );

    geo.new_usertype<lc::geo::Circle>(
            "Circle",
            sol::base_classes, sol::bases<lc::geo::Base, lc::Visitable, lc::entity::Tangentable>(),
            "accept", &lc::geo::Circle::accept,
            "center", &lc::geo::Circle::center,
            "equation", &lc::geo::Circle::equation,
            "lineTangentPointsOnEntity", &lc::geo::Circle::lineTangentPointsOnEntity,
            "nearestPointOnEntity", &lc::geo::Circle::nearestPointOnEntity,
            "nearestPointOnPath", &lc::geo::Circle::nearestPointOnPath,
            "radius", &lc::geo::Circle::radius
            );

    geo.new_usertype<lc::geo::Ellipse>(
            "Ellipse",
            sol::constructors<lc::geo::Ellipse(lc::geo::Coordinate, lc::geo::Coordinate, double, double, double, bool)>(),
            sol::base_classes, sol::bases<lc::geo::Base, lc::Visitable>(),
            "accept", &lc::geo::Ellipse::accept,
            "center", &lc::geo::Ellipse::center,
            "endAngle", &lc::geo::Ellipse::endAngle,
            "endPoint", &lc::geo::Ellipse::endPoint,
            "equation", &lc::geo::Ellipse::equation,
            "findPotentialNearestPoints", &lc::geo::Ellipse::findPotentialNearestPoints,
            "georotate", &lc::geo::Ellipse::georotate,
            "geoscale", &lc::geo::Ellipse::geoscale,
            "getAngle", &lc::geo::Ellipse::getAngle,
            "getEllipseAngle", &lc::geo::Ellipse::getEllipseAngle,
            "getPoint", &lc::geo::Ellipse::getPoint,
            "isAngleBetween", &lc::geo::Ellipse::isAngleBetween,
            "isArc", &lc::geo::Ellipse::isArc,
            "isReversed", &lc::geo::Ellipse::isReversed,
            "majorP", &lc::geo::Ellipse::majorP,
            "majorRadius", &lc::geo::Ellipse::majorRadius,
            "minorRadius", &lc::geo::Ellipse::minorRadius,
            "nearestPointOnEntity", &lc::geo::Ellipse::nearestPointOnEntity,
            "nearestPointOnPath", &lc::geo::Ellipse::nearestPointOnPath,
            "ratio", &lc::geo::Ellipse::ratio,
            "startAngle", &lc::geo::Ellipse::startAngle,
            "startPoint", &lc::geo::Ellipse::startPoint
            );

    geo.new_usertype<lc::geo::Spline>(
            "Spline",
            sol::constructors<lc::geo::Spline(const std::vector<lc::geo::Coordinate> &, const std::vector<double> &, 
                const std::vector<lc::geo::Coordinate> &, int, bool, double, double, double, double, double, double, 
                double, double, double, double, enum lc::geo::Spline::splineflag)>(),
            sol::base_classes, sol::bases<lc::geo::Base, lc::Visitable>(),
            "accept", &lc::geo::Spline::accept,
            "beziers", &lc::geo::Spline::beziers,
            "closed", &lc::geo::Spline::closed,
            "controlPoints", &lc::geo::Spline::controlPoints,
            "degree", &lc::geo::Spline::degree,
            "endTanX", &lc::geo::Spline::endTanX,
            "endTanY", &lc::geo::Spline::endTanY,
            "endTanZ", &lc::geo::Spline::endTanZ,
            "fitPoints", &lc::geo::Spline::fitPoints,
            "fitTolerance", &lc::geo::Spline::fitTolerance,
            "flags", &lc::geo::Spline::flags,
            "knotPoints", &lc::geo::Spline::knotPoints,
            "nX", &lc::geo::Spline::nX,
            "nY", &lc::geo::Spline::nY,
            "nZ", &lc::geo::Spline::nZ,
            "nearestPointOnEntity", &lc::geo::Spline::nearestPointOnEntity,
            "nearestPointOnPath", &lc::geo::Spline::nearestPointOnPath,
            "populateCurve", &lc::geo::Spline::populateCurve,
            "startTanX", &lc::geo::Spline::startTanX,
            "startTanY", &lc::geo::Spline::startTanY,
            "startTanZ", &lc::geo::Spline::startTanZ,
            "trimAtPoint", &lc::geo::Spline::trimAtPoint
            );

    geo.new_usertype<lc::geo::Region>(
            "Region",
            sol::constructors<lc::geo::Region(), lc::geo::Region(std::vector<lc::entity::CADEntity_CSPtr>)>(),
            "Area", &lc::geo::Region::Area
            );
}
