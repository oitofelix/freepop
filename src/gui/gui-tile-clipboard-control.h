/*
  gui-tile-clipboard-control.c -- editor GUI tile clipboard control module;

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

#ifndef MININIM_GUI_TILE_CLIPBOARD_CONTROL_H
#define MININIM_GUI_TILE_CLIPBOARD_CONTROL_H

Ihandle *gui_create_tile_clipboard_control
(struct pos *p, struct sel_ring *sr, char *norm_group);

#endif	/* MININIM_GUI_TILE_CLIPBOARD_CONTROL */
