/*
  guard-fall.h -- guard fall module;

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

#ifndef MININIM_GUARD_FALL_H
#define MININIM_GUARD_FALL_H

/* bitmaps */
#define GUARD_FALL_01 "data/guard/fall/01.png"
#define GUARD_FALL_02 "data/guard/fall/02.png"
#define GUARD_FALL_03 "data/guard/fall/03.png"

#define GUARD_FALL_FRAMESET_NMEMB 3
extern struct frameset guard_fall_frameset[GUARD_FALL_FRAMESET_NMEMB];

void load_guard_fall (void);
void unload_guard_fall (void);
void guard_fall (struct anim *g);
bool is_guard_fall (struct frame *f);

extern ALLEGRO_BITMAP *guard_fall_01, *guard_fall_02, *guard_fall_03;

#endif	/* MININIM_GUARD_FALL_H */
