/*
  L_mininim.settings.c -- mininim.settings script module;

  Copyright (C) Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mininim.h"

static DEFUN (__eq);
static DEFUN (__index);
static DEFUN (__newindex);
static DEFUN (__tostring);

void
define_L_mininim_settings (lua_State *L)
{
  luaL_newmetatable(L, L_MININIM_SETTINGS);

  lua_pushstring (L, "__eq");
  lua_pushcfunction (L, __eq);
  lua_rawset (L, -3);

  lua_pushstring (L, "__index");
  lua_pushcfunction (L, __index);
  lua_rawset (L, -3);

  lua_pushstring (L, "__newindex");
  lua_pushcfunction (L, __newindex);
  lua_rawset (L, -3);

  lua_pushstring (L, "__tostring");
  lua_pushcfunction (L, __tostring);
  lua_rawset (L, -3);

  lua_pop (L, 1);
}

DEFUN (__eq)
{
  lua_pushboolean (L, true);
  return 1;
}

DEFUN (__index)
{
  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "audio_mode")) {
      lua_pushstring (L, audio_mode);
      return 1;
    } else if (! strcasecmp (key, "video_mode")) {
      lua_pushstring (L, video_mode);
      return 1;
    } else if (! strcasecmp (key, "env_mode")) {
      lua_pushstring (L, env_mode);
      return 1;
    } else if (! strcasecmp (key, "hue_mode")) {
      lua_pushstring (L, hue_mode);
      return 1;
    } else if (! strcasecmp (key, "repl_priority")) {
      lua_pushnumber (L, repl_priority);
      return 1;
    } else break;
  default: break;
  }

  return 0;
}

DEFUN (__newindex)
{
  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "audio_mode")) {
      L_set_string_var (L, 3, &audio_mode);
      return 0;
    } else if (! strcasecmp (key, "video_mode")) {
      char *mode = (char *) lua_tostring (L, 3);
      lock_lua ();
      ui_vm (mode);
      unlock_lua ();
      return 0;
    } else if (! strcasecmp (key, "env_mode")) {
      L_set_string_var (L, 3, &env_mode);
      return 0;
    } else if (! strcasecmp (key, "hue_mode")) {
      L_set_string_var (L, 3, &hue_mode);
      return 0;
    } else if (! strcasecmp (key, "repl_priority")) {
      repl_priority = lua_tonumber (L, 3);
      return 0;
    } else break;
  default: break;
  }

  return 0;
}

DEFUN (__tostring)
{
  lua_pushstring (L, "MININIM SETTINGS INTERFACE");
  return 1;
}
