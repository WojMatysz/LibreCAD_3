#pragma once

#include <lclua.h>
#include <cad/builders/insert.h>

#include "sol.hpp"

namespace lc 
{

namespace entity {

class LuaCustomEntity;
DECLARE_SHORT_SHARED_PTR(LuaCustomEntity)

}

namespace builder 
{

class CustomEntityBuilder 
    :public InsertBuilder 
{
public:
    void setSnapFunction(const sol::function & snapFunction) 
    { m_snapFunction = validateFunction(snapFunction, "Snap function"); }

    void setNearestPointFunction(const sol::function & nearestPointFunction) 
    { m_nearestPointFunction = validateFunction(nearestPointFunction, "Nearest point function"); }

    void setDragPointsFunction(const sol::function & dragPointsFunction) 
    { m_dragPointsFunction = validateFunction(dragPointsFunction, "Drag points function"); }

    void setNewDragPointFunction(const sol::function & newDragPointFunction) 
    { m_newDragPointFunction = validateFunction(newDragPointFunction, "New drag point function"); }

    void setDragPointsClickedFunction(const sol::function & dragPointsClickedFunction) 
    { m_dragPointsClickedFunction = validateFunction(dragPointsClickedFunction, "Drag points clicked function"); }

    void setDragPointsReleasedFunction(const sol::function & dragPointsReleasedFunction) 
    { m_dragPointsReleasedFunction = validateFunction(dragPointsReleasedFunction, "Drag points released function"); }

    entity::LuaCustomEntity_CSPtr build();

    // Expose const references for external use
    const sol::function & snapFunction() const { return m_snapFunction; }
    const sol::function & nearestPointFunction() const { return m_nearestPointFunction; }
    const sol::function & dragPointsFunction() const { return m_dragPointsFunction; }
    const sol::function & newDragPointFunction() const { return m_newDragPointFunction; }
    const sol::function & dragPointsClickedFunction() const { return m_dragPointsClickedFunction; }
    const sol::function & dragPointsReleasedFunction() const { return m_dragPointsReleasedFunction; }

private:
    sol::function validateFunction(const sol::function & function, const std::string & name) const {
        if(!function.valid()) {
            throw std::runtime_error(name + " callback MUST be a function");
        }
        return function;
    }

    sol::function m_snapFunction;
    sol::function m_nearestPointFunction;
    sol::function m_dragPointsFunction;
    sol::function m_newDragPointFunction;
    sol::function m_dragPointsClickedFunction;
    sol::function m_dragPointsReleasedFunction;
};

} // namespace builder
} // namespace lc
