--[[
   sblast.lua -- Sound Blaster audio mode;

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
--]]

-- header
local P = {package_type = "AUDIO MODE", package_name = "SBLAST",
           package_file = MININIM.debugger.src}

-- imports
local MININIM = MININIM
local common = require "script/common"

local function p (filename)
   return common.load_sample (P, filename)
end

local function t (filename)
   return common.load_stream (P, filename)
end

-- body
local _ENV = P

function load ()
   MININIM.lua.audio_mode[package_name] =
      {SUSPENSE = p "audio/suspense.ogg"}
   return P
end

-- end
return P
