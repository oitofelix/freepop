/*
  kid-sword-hit.h -- kid sword walk forward module;

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

#ifndef MININIM_KID_SWORD_HIT_H
#define MININIM_KID_SWORD_HIT_H

/* bitmaps */
#define KID_SWORD_HIT_01 "dat/kid/hanging and falling/frame13.png"
#define KID_SWORD_HIT_02 "dat/kid/hanging and falling/frame14.png"
#define KID_SWORD_HIT_03 "dat/kid/hanging and falling/frame15.png"
#define KID_SWORD_HIT_04 "dat/kid/sword attacking/frame06.png"
#define KID_SWORD_HIT_05 "dat/kid/sword attacking/frame07.png"

#define KID_SWORD_HIT_FRAMESET_NMEMB 5
extern struct frameset kid_sword_hit_frameset[KID_SWORD_HIT_FRAMESET_NMEMB];

void load_kid_sword_hit (void);
void unload_kid_sword_hit (void);
void kid_sword_hit (struct anim *k);

extern ALLEGRO_BITMAP *kid_sword_hit_01, *kid_sword_hit_02,
  *kid_sword_hit_03, *kid_sword_hit_04, *kid_sword_hit_05;

#endif	/* MININIM_KID_SWORD_HIT_H */
