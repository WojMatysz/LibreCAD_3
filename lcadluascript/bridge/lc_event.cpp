#include <cad/events/addlayerevent.h>
#include <cad/events/newwaitingcustomentityevent.h>
#include <cad/events/commitprocessevent.h>
#include <cad/events/addentityevent.h>
#include <cad/events/replacelayerevent.h>
#include <cad/events/removeentityevent.h>
#include <cad/events/replaceentityevent.h>
#include <cad/events/addlinepatternevent.h>
#include <cad/events/removelinepatternevent.h>
#include <cad/events/replacelinepatternevent.h>
#include <cad/events/removelayerevent.h>
#include <cad/primitive/insert.h>
#include "lc_event.h"

void import_lc_event_namespace(sol::state & luaVM) 
{
    sol::table lc = luaVM["lc"].get_or_create<sol::table>();
    sol::table event = lc["event"].get_or_create<sol::table>();

    event.new_usertype<lc::event::AddLayerEvent>(
            "AddLayerEvent",
            sol::call_constructor,
            sol::constructors<lc::event::AddLayerEvent(const lc::meta::Layer_CSPtr)>(),
            "layer", &lc::event::AddLayerEvent::layer
            );

    event.new_usertype<lc::event::RemoveLayerEvent>(
            "RemoveLayerEvent",
            sol::call_constructor,
            sol::constructors<lc::event::RemoveLayerEvent(const lc::meta::Layer_CSPtr)>(),
            "layer", &lc::event::RemoveLayerEvent::layer
            );

    event.new_usertype<lc::event::ReplaceLayerEvent>(
            "ReplaceLayerEvent",
            sol::call_constructor,
            sol::constructors<lc::event::ReplaceLayerEvent(const lc::meta::Layer_CSPtr, const lc::meta::Layer_CSPtr)>(),
            "newLayer", &lc::event::ReplaceLayerEvent::newLayer,
            "oldLayer", &lc::event::ReplaceLayerEvent::oldLayer
            );

    event.new_usertype<lc::event::NewWaitingCustomEntityEvent>(
            "NewWaitingCustomEntityEvent",
            sol::call_constructor,
            sol::constructors<lc::event::NewWaitingCustomEntityEvent(const lc::entity::Insert_CSPtr &)>(),
            "insert", &lc::event::NewWaitingCustomEntityEvent::insert
            );

    event.new_usertype<lc::event::CommitProcessEvent>(
            "CommitProcessEvent",
            sol::call_constructor,
            sol::constructors<lc::event::CommitProcessEvent(lc::operation::DocumentOperation_SPtr)>(),
            "operation", &lc::event::CommitProcessEvent::operation
            );

    event.new_usertype<lc::event::AddEntityEvent>(
            "AddEntityEvent",
            sol::call_constructor,
            sol::constructors<lc::event::AddEntityEvent(lc::entity::CADEntity_CSPtr)>(),
            "entity", &lc::event::AddEntityEvent::entity
            );

    event.new_usertype<lc::event::RemoveEntityEvent>(
            "RemoveEntityEvent",
            sol::call_constructor,
            sol::constructors<lc::event::RemoveEntityEvent(const lc::entity::CADEntity_CSPtr)>(),
            "entity", &lc::event::RemoveEntityEvent::entity
            );

    event.new_usertype<lc::event::ReplaceEntityEvent>(
            "ReplaceEntityEvent",
            sol::call_constructor,
            sol::constructors<lc::event::ReplaceEntityEvent(const lc::entity::CADEntity_CSPtr)>(),
            "entity", &lc::event::ReplaceEntityEvent::entity
            );

    event.new_usertype<lc::event::AddLinePatternEvent>(
            "AddLinePatternEvent",
            sol::call_constructor,
            sol::constructors<lc::event::AddLinePatternEvent(const lc::meta::DxfLinePatternByValue_CSPtr)>(),
            "linePattern", &lc::event::AddLinePatternEvent::linePattern
            );

    event.new_usertype<lc::event::RemoveLinePatternEvent>(
            "RemoveLinePatternEvent",
            sol::call_constructor,
            sol::constructors<lc::event::RemoveLinePatternEvent(const lc::meta::DxfLinePatternByValue_CSPtr)>(),
            "linePattern", &lc::event::RemoveLinePatternEvent::linePattern
            );

    event.new_usertype<lc::event::ReplaceLinePatternEvent>(
            "ReplaceLinePatternEvent",
            sol::call_constructor,
            sol::constructors<lc::event::ReplaceLinePatternEvent(const lc::meta::DxfLinePatternByValue_CSPtr, const lc::meta::DxfLinePatternByValue_CSPtr)>(),
            "newLinePattern", &lc::event::ReplaceLinePatternEvent::newLinePattern,
            "oldLinePattern", &lc::event::ReplaceLinePatternEvent::oldLinePattern
            );
}
