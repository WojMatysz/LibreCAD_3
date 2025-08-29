#include <builders/customentity.h>
#include "customentity.h"

using namespace lc;
using namespace entity;

LuaCustomEntity::LuaCustomEntity(const lc::builder::CustomEntityBuilder& builder) :
    CustomEntity(builder),
    m_snapPoints(builder.snapFunction()),
    m_nearestPoint(builder.nearestPointFunction()),
    m_dragPoints(builder.dragPointsFunction()),
    m_newDragPoint(builder.newDragPointFunction()),
    m_dragPointClick(builder.dragPointsClickedFunction()),
    m_dragPointRelease(builder.dragPointsReleasedFunction()) {
}

LuaCustomEntity::LuaCustomEntity(const Insert_CSPtr& insert, const LuaCustomEntity_CSPtr& customEntity, bool sameID) :
    CustomEntity(insert, sameID),
    m_snapPoints(customEntity->m_snapPoints),
    m_nearestPoint(customEntity->m_nearestPoint),
    m_dragPoints(customEntity->m_dragPoints),
    m_newDragPoint(customEntity->m_newDragPoint),
    m_dragPointClick(customEntity->m_dragPointClick),
    m_dragPointRelease(customEntity->m_dragPointRelease) {

}

std::vector<EntityCoordinate> LuaCustomEntity::snapPoints(const geo::Coordinate& coord,
        const SimpleSnapConstrain& simpleSnapConstrain,
        double minDistanceToSnap,
        int maxNumberOfSnapPoints) const {
    auto snapPointsDupl = m_snapPoints; //Dirty hack to call Lua function from a const function
    auto points = snapPointsDupl.call<std::vector<EntityCoordinate>>(shared_from_this(), coord, simpleSnapConstrain, minDistanceToSnap, maxNumberOfSnapPoints);
    Snapable::snapPointsCleanup(points, coord, maxNumberOfSnapPoints, minDistanceToSnap);

    return points;
}

geo::Coordinate LuaCustomEntity::nearestPointOnPath(const geo::Coordinate& coord) const {
    auto nearestPointDupl = m_nearestPoint;
    return nearestPointDupl.call<geo::Coordinate>(shared_from_this(), coord);
}

CADEntity_CSPtr LuaCustomEntity::modifyInsert(const CADEntity_CSPtr& insert) const {
    auto i = std::dynamic_pointer_cast<const Insert>(insert);

    if(!i) {
        return shared_from_this();
    }

    return std::make_shared<LuaCustomEntity>(i, std::static_pointer_cast<const LuaCustomEntity>(shared_from_this()), true);
}

CADEntity_CSPtr LuaCustomEntity::move(const geo::Coordinate& offset) const {
    return modifyInsert(Insert::move(offset));
}

CADEntity_CSPtr LuaCustomEntity::copy(const geo::Coordinate& offset) const {
    return modifyInsert(Insert::copy(offset));
}

CADEntity_CSPtr LuaCustomEntity::rotate(const geo::Coordinate& rotation_center, const double rotation_angle) const {
    return modifyInsert(Insert::rotate(rotation_center, rotation_angle));
}

CADEntity_CSPtr LuaCustomEntity::scale(const geo::Coordinate& scale_center, const geo::Coordinate& scale_factor) const {
    return modifyInsert(Insert::scale(scale_center, scale_factor));
}

CADEntity_CSPtr LuaCustomEntity::mirror(const geo::Coordinate& axis1, const geo::Coordinate& axis2) const {
    return modifyInsert(Insert::mirror(axis1, axis2));
}

CADEntity_CSPtr LuaCustomEntity::modify(meta::Layer_CSPtr layer, const meta::MetaInfo_CSPtr metaInfo, meta::Block_CSPtr block) const {
    return modifyInsert(Insert::modify(layer, metaInfo, block));
}

std::map<unsigned int, geo::Coordinate> LuaCustomEntity::dragPoints() const {
    auto dragPointsDupl = m_dragPoints;
    return dragPointsDupl.call<std::map<unsigned int, geo::Coordinate>>(shared_from_this());
}

void LuaCustomEntity::onDragPointClick(lc::operation::Builder_SPtr builder, unsigned int point) const {
    auto dragPointClickDupl = m_dragPointClick;
    dragPointClickDupl(shared_from_this(), builder, point);
}

void LuaCustomEntity::onDragPointRelease(lc::operation::Builder_SPtr builder) const {
    auto dragPointReleaseDupl = m_dragPointRelease;
    dragPointReleaseDupl(shared_from_this(), builder);
}

void LuaCustomEntity::setDragPoint(lc::geo::Coordinate position) const {
    auto newDragPointDupl = m_newDragPoint;
    newDragPointDupl(shared_from_this(), position);
}
