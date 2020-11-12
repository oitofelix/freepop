/*
  native-level.h -- native level module;

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

#ifndef MININIM_NATIVE_LEVEL_H
#define MININIM_NATIVE_LEVEL_H

/* functions */
struct level *next_native_level (struct level *l, int n);
struct level *load_native_level (struct level *l, int n);
bool save_native_level (struct level *l, char *filename);
char *get_tile_ext_str (struct pos *p);

/* variables */
extern char *tile_fg_str[];
extern char *tile_bg_str[];
extern char *tile_item_str[];

#endif	/* MININIM_NATIVE_LEVEL_H */
