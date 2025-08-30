#include <cad/events/newwaitingcustomentityevent.h>
#include <cad/storage/documentlist.h>
#include <cad/meta/customentitystorage.h>
#include <cad/primitive/insert.h>
#include "luacustomentitymanager.h"

using namespace lc::lua;

LuaCustomEntityManager::LuaCustomEntityManager() {
    storage::DocumentList::getInstance().newWaitingCustomEntityEvent().connect<LuaCustomEntityManager, &LuaCustomEntityManager::onNewWaitingEntity>(this);
}

LuaCustomEntityManager::~LuaCustomEntityManager() {
    m_plugins.clear();
    storage::DocumentList::getInstance().newWaitingCustomEntityEvent().disconnect<LuaCustomEntityManager, &LuaCustomEntityManager::onNewWaitingEntity>(this);
}


void LuaCustomEntityManager::onNewWaitingEntity(const lc::event::NewWaitingCustomEntityEvent& event) {
    auto block = event.insert()->displayBlock();

    auto ces = std::static_pointer_cast<const lc::meta::CustomEntityStorage>(block);
    if(!ces) {
        return;
    }

    auto it = m_plugins.find(ces->pluginName());
    if(it == m_plugins.end()) {
        return;
    }

    it->second(event.insert());
}

void LuaCustomEntityManager::registerPlugin(const std::string& name, sol::function onNewWaitingEntityFunction) 
{

    if(!onNewWaitingEntityFunction.valid()) 
    {
        std::cerr << "sol::function sent to registration process is not valid, name: " << name << "\n";
        return;
    }

    m_plugins[name] = onNewWaitingEntityFunction;

    for(auto entity : storage::DocumentList::getInstance().waitingCustomEntities(name)) 
    {
        try
        {
            onNewWaitingEntityFunction(entity);
        }
        catch(const sol::error & err)
        {
            std::cerr << "Fail to register plugin: " << name << "\n"
                      << err.what() << "\n";
        }
    }
}

void LuaCustomEntityManager::removePlugins() {
    m_plugins.clear();
}
