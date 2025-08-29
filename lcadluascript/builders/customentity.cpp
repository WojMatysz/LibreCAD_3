#include "customentity.h"
#include "../primitive/customentity.h"

using namespace lc;
using namespace builder;

entity::LuaCustomEntity_CSPtr CustomEntityBuilder::build()
    //{ return std::make_shared<entity::LuaCustomEntity>(*this); }
    //{ return std::shared_ptr<entity::LuaCustomEntity>(new entity::LuaCustomEntity(*this)); }
{ return entity::LuaCustomEntity_CSPtr(new entity::LuaCustomEntity(*this)); }
