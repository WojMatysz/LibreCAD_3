#pragma once

#include "sol.hpp"

void luaOpenGUIBridge(sol::state & luaVM);

void addLCBindings(sol::state & luaVM);
void addLuaGUIAPIBindings(sol::state & luaVM);
