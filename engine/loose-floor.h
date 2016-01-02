/*
  loose-floor.h -- loose floor module;

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

#ifndef MININIM_LOOSE_FLOOR_H
#define MININIM_LOOSE_FLOOR_H

#include "kernel/audio.h"

/* constants */
#define LOOSE_FLOOR_RESISTENCE 0

/* dungeon cga */
#define DC_LOOSE_FLOOR_BASE_01 "data/loose-floor/dc-base-01.png"
#define DC_LOOSE_FLOOR_LEFT_01 "data/loose-floor/dc-left-01.png"
#define DC_LOOSE_FLOOR_RIGHT_01 "data/loose-floor/dc-right-01.png"
#define DC_LOOSE_FLOOR_BASE_02 "data/loose-floor/dc-base-02.png"
#define DC_LOOSE_FLOOR_LEFT_02 "data/loose-floor/dc-left-02.png"
#define DC_LOOSE_FLOOR_RIGHT_02 "data/loose-floor/dc-right-02.png"

/* palace cga */
#define PC_LOOSE_FLOOR_BASE_01 "data/loose-floor/pc-base-01.png"
#define PC_LOOSE_FLOOR_LEFT_01 "data/loose-floor/pc-left-01.png"
#define PC_LOOSE_FLOOR_RIGHT_01 "data/loose-floor/pc-right-01.png"
#define PC_LOOSE_FLOOR_BASE_02 "data/loose-floor/pc-base-02.png"
#define PC_LOOSE_FLOOR_LEFT_02 "data/loose-floor/pc-left-02.png"
#define PC_LOOSE_FLOOR_RIGHT_02 "data/loose-floor/pc-right-02.png"

/* dungeon ega */
#define DE_LOOSE_FLOOR_BASE_01 "data/loose-floor/de-base-01.png"
#define DE_LOOSE_FLOOR_LEFT_01 "data/loose-floor/de-left-01.png"
#define DE_LOOSE_FLOOR_RIGHT_01 "data/loose-floor/de-right-01.png"
#define DE_LOOSE_FLOOR_BASE_02 "data/loose-floor/de-base-02.png"
#define DE_LOOSE_FLOOR_LEFT_02 "data/loose-floor/de-left-02.png"
#define DE_LOOSE_FLOOR_RIGHT_02 "data/loose-floor/de-right-02.png"

/* palace ega */
#define PE_LOOSE_FLOOR_BASE_01 "data/loose-floor/pe-base-01.png"
#define PE_LOOSE_FLOOR_LEFT_01 "data/loose-floor/pe-left-01.png"
#define PE_LOOSE_FLOOR_RIGHT_01 "data/loose-floor/pe-right-01.png"
#define PE_LOOSE_FLOOR_BASE_02 "data/loose-floor/pe-base-02.png"
#define PE_LOOSE_FLOOR_LEFT_02 "data/loose-floor/pe-left-02.png"
#define PE_LOOSE_FLOOR_RIGHT_02 "data/loose-floor/pe-right-02.png"

/* dungeon vga */
#define DV_LOOSE_FLOOR_BASE_01 "data/loose-floor/dv-base-01.png"
#define DV_LOOSE_FLOOR_LEFT_01 "data/loose-floor/dv-left-01.png"
#define DV_LOOSE_FLOOR_RIGHT_01 "data/loose-floor/dv-right-01.png"
#define DV_LOOSE_FLOOR_BASE_02 "data/loose-floor/dv-base-02.png"
#define DV_LOOSE_FLOOR_LEFT_02 "data/loose-floor/dv-left-02.png"
#define DV_LOOSE_FLOOR_RIGHT_02 "data/loose-floor/dv-right-02.png"

/* palace vga */
#define PV_LOOSE_FLOOR_BASE_01 "data/loose-floor/pv-base-01.png"
#define PV_LOOSE_FLOOR_LEFT_01 "data/loose-floor/pv-left-01.png"
#define PV_LOOSE_FLOOR_RIGHT_01 "data/loose-floor/pv-right-01.png"
#define PV_LOOSE_FLOOR_BASE_02 "data/loose-floor/pv-base-02.png"
#define PV_LOOSE_FLOOR_LEFT_02 "data/loose-floor/pv-left-02.png"
#define PV_LOOSE_FLOOR_RIGHT_02 "data/loose-floor/pv-right-02.png"

/* sounds */
#define LOOSE_FLOOR_01_SAMPLE "dat/digisnd1/tile moving 1.ogg"
#define LOOSE_FLOOR_02_SAMPLE "dat/digisnd1/tile moving 2.ogg"
#define LOOSE_FLOOR_03_SAMPLE "dat/digisnd1/door tile pressed 1.ogg"
#define BROKEN_FLOOR_SAMPLE "dat/digisnd1/tile crashing into the floor.ogg"

/* types */
struct loose_floor {
  struct pos p;
  int i;
  int resist;
  int state;
  bool cant_fall;

  enum {
    NO_LOOSE_FLOOR_ACTION,
    SHAKE_LOOSE_FLOOR,
    RELEASE_LOOSE_FLOOR,
    FALL_LOOSE_FLOOR,
  } action;

  struct frame f;
};

/* variables */
extern struct loose_floor *loose_floor;
extern size_t loose_floor_nmemb;

/* functions */
void load_loose_floor (void);
void unload_loose_floor (void);
void load_loose_floor_samples (void);
void unload_loose_floor_samples (void);
ALLEGRO_BITMAP *create_loose_floor_02_bitmap (enum em em, enum vm vm);
void register_loose_floor (struct pos *p);
void sort_loose_floors (void);
int compare_loose_floors (const void *l0, const void *l1);
struct loose_floor * loose_floor_at_pos (struct pos *p);
void remove_loose_floor (struct loose_floor *l);
void release_loose_floor (struct pos *p);
void compute_loose_floors (void);
void compute_loose_floor_shake (struct loose_floor *l);
void compute_loose_floor_release (struct loose_floor *l);
void compute_loose_floor_fall (struct loose_floor *l);
void sample_loose_floors (void);
void shake_loose_floor_row (struct pos *p);
void sample_random_loose_floor (void);
ALLEGRO_BITMAP *get_correct_falling_loose_floor_bitmap (ALLEGRO_BITMAP *b);
void draw_loose_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                       enum em em, enum vm vm);
void draw_loose_floor_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                            enum em em, enum vm vm);
void draw_loose_floor_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                            enum em em, enum vm vm);
void draw_loose_floor_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                             enum em em, enum vm vm);
void draw_falling_loose_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                               enum em em, enum vm vm);
void draw_loose_floor_01 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                          enum em em, enum vm vm);
void draw_loose_floor_01_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                               enum em em, enum vm vm);
void draw_loose_floor_01_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                               enum em em, enum vm vm);
void draw_loose_floor_01_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                enum em em, enum vm vm);
void draw_loose_floor_02 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                          enum em em, enum vm vm);
void draw_loose_floor_02_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                               enum em em, enum vm vm);
void draw_loose_floor_02_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                               enum em em, enum vm vm);
void draw_loose_floor_02_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                enum em em, enum vm vm);
struct coord *loose_floor_left_coord (struct pos *p, struct coord *c);
struct coord *loose_floor_right_coord (struct pos *p, struct coord *c);

#endif	/* MININIM_LOOSE_FLOOR_H */
