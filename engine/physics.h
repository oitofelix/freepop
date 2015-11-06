/*
  physics.h -- physics module;

  Copyright (C) 2015, 2016 Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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

#ifndef FREEPOP_PHYSICS_H
#define FREEPOP_PHYSICS_H

#include "prince.h"

/* functions */
struct construct construct (struct pos pos);
struct construct construct_rel (struct pos pos, int floor, int place);
void set_construct_fg (struct pos p, enum construct_fg fg);
bool is_colliding (struct anim anim);
int dist_collision (struct anim anim);
int dist_fall (struct anim anim);
bool is_falling (struct anim anim);
void to_collision_edge (struct anim *anim);
void to_fall_edge (struct anim *anim);
bool is_on_loose_floor (struct anim a);
int dist_loose_floor (struct anim a);
void to_loose_floor_edge (struct anim *a);
bool is_hangable (struct anim a);
int dist_next_place (struct anim a);
void to_next_place_edge (struct anim *a);
bool is_hangable_pos (struct pos p, enum dir dir);
void to_prev_place_edge (struct anim *a);
int dist_prev_place (struct anim a);
bool is_visible (struct anim a);
void apply_physics (struct anim *a, ALLEGRO_BITMAP *frame,
                    int dx, int dy);

extern struct pos hang_pos;
extern struct pos loose_floor_pos;

#endif	/* FREEPOP_PHYSICS_H */
