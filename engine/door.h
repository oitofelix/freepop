/*
  door.h -- door module;

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

#ifndef MININIM_DOOR_H
#define MININIM_DOOR_H

#include "kernel/video.h"

#define DOOR_MAX_STEP 47
#define DOOR_WAIT SECS_TO_SCYCLES (5)

#define DOOR_CLIMB_LIMIT 40
#define DOOR_WAIT_LOOK 4

/* dungeon ega */
#define DE_DOOR_LEFT "data/door/de-left.png"
#define DE_DOOR_RIGHT "data/door/de-right.png"
#define DE_DOOR_POLE "data/door/de-pole.png"
#define DE_DOOR_TOP "data/door/de-top.png"
#define DE_DOOR_GRID "data/door/de-grid.png"
#define DE_DOOR_GRID_TIP "data/door/de-grid-tip.png"

/* palace ega */
#define PE_DOOR_LEFT "data/door/pe-left.png"
#define PE_DOOR_RIGHT "data/door/pe-right.png"
#define PE_DOOR_POLE "data/door/pe-pole.png"
#define PE_DOOR_TOP "data/door/pe-top.png"
#define PE_DOOR_GRID "data/door/pe-grid.png"
#define PE_DOOR_GRID_TIP "data/door/pe-grid-tip.png"

/* dungeon vga */
#define DV_DOOR_LEFT "data/door/dv-left.png"
#define DV_DOOR_RIGHT "data/door/dv-right.png"
#define DV_DOOR_POLE "data/door/dv-pole.png"
#define DV_DOOR_TOP "data/door/dv-top.png"
#define DV_DOOR_GRID "data/door/dv-grid.png"
#define DV_DOOR_GRID_TIP "data/door/dv-grid-tip.png"

/* palace vga */
#define PV_DOOR_LEFT "data/door/pv-left.png"
#define PV_DOOR_RIGHT "data/door/pv-right.png"
#define PV_DOOR_POLE "data/door/pv-pole.png"
#define PV_DOOR_TOP "data/door/pv-top.png"
#define PV_DOOR_GRID "data/door/pv-grid.png"
#define PV_DOOR_GRID_TIP "data/door/pv-grid-tip.png"

/* sounds */
#define DOOR_OPEN_SAMPLE "dat/digisnd1/door gate closing slow 1.ogg"
#define DOOR_CLOSE_SAMPLE "dat/digisnd1/door gate opening 1.ogg"
#define DOOR_END_SAMPLE "dat/digisnd1/drinking potion.ogg"
#define DOOR_ABRUPTLY_CLOSE_SAMPLE "dat/digisnd1/door gate closing fast.ogg"

/* types */
struct door {
  struct pos p;
  int i;
  enum {
    NO_DOOR_ACTION, OPEN_DOOR, CLOSE_DOOR, ABRUPTLY_CLOSE_DOOR,
  } action;
  int wait;
  bool noise;
};

void load_door (void);
void unload_door (void);
void load_door_samples (void);
void unload_door_samples (void);
void register_door (struct pos *p);
int compare_doors (const void *d0, const void *d1);
struct door *door_at_pos (struct pos *p);
void open_door (int e);
void close_door (int e);
void compute_doors (void);
int door_grid_tip_y (struct pos *p);
void sample_doors (void);
void draw_door (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em em, enum vm vm);
void draw_door_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm);
void draw_door_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm);
void draw_door_fg (ALLEGRO_BITMAP *bitmap, struct pos *p, struct frame *f,
                   enum em em, enum vm vm);
void draw_door_grid (ALLEGRO_BITMAP *bitmap, struct pos *p, int i,
                     enum em em, enum vm vm);
struct coord *door_grid_coord_base (struct pos *p, struct coord *c);
struct coord *door_grid_coord (struct pos *p, struct coord *c, int j, int i);
struct coord *door_grid_tip_coord (struct pos *p, struct coord *c, int i);
struct coord *door_pole_coord (struct pos *p, struct coord *c);
struct coord *door_left_coord (struct pos *p, struct coord *c);
struct coord *door_right_coord (struct pos *p, struct coord *c);
struct coord *door_top_coord (struct pos *p, struct coord *c);

#endif	/* MININIM_DOOR_H */
