/*
  editor.h -- editor module;

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

#ifndef MININIM_EDITOR_H
#define MININIM_EDITOR_H

/* variables */
extern enum edit edit;
extern enum edit last_edit;
extern uint64_t editor_register;
extern bool selection_locked;
extern bool editor_reciprocal_links, editor_locally_unique_links,
  editor_globally_unique_links;

/* functions */
void change_tile_fg (struct pos *p, enum tile_fg f);
void change_tile_fake (struct pos *p, enum tile_fg f);
void change_tile_bg (struct pos *p, enum tile_bg b);
void change_tile_ext (struct pos *p, int e);
void select_pos (struct mr *mr, struct pos *p);

bool can_edit (void);
void editor (void);
int enter_editor (Ihandle *ih);
int exit_editor (Ihandle *ih);
void editor_msg (char *m, uint64_t cycles);
void ui_place_kid (struct actor *k, struct pos *p);
void ui_place_guard (struct actor *g, struct pos *p);
void editor_link (struct room_linking *rlink, size_t room_nmemb,
                  int room0, int room1, enum dir dir);
void closure_link_room (struct room_linking *rlink, size_t room_nmemb,
                        int room0, int room1, enum dir dir);
void editor_mirror_link (struct room_linking *rlink, size_t room_nmemb,
                         int room, enum dir dir0, enum dir dir1);
int editor_new_room (int room, enum dir d);

#endif	/* MININIM_EDITOR_H */
