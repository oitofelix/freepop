/*
  pos.h -- position module;

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

#ifndef MININIM_POSITION_H
#define MININIM_POSITION_H

extern bool coord_wa;

/* types */
int typed_int (int i, int n, int f, int *nr, int *nf);
int room_val (size_t room_nmemb, int room);

int *roomd_ptr (struct room_linking *rlink, size_t room_nmemb,
                int room, enum dir dir);
int roomd (struct room_linking *rlink, size_t room_nmemb, int room,
           enum dir dir);
int roomd_n0 (struct room_linking *rlink, size_t room_nmemb,
              int room, enum dir dir);
bool is_room_adjacent (struct room_linking *rlink, size_t room_nmemb,
                       int room0, int room1);
void link_room (struct room_linking *rlink, size_t room_nmemb,
                int room0, int room1, enum dir dir);
void mirror_link (struct room_linking *rlink, size_t room_nmemb,
                  int room, enum dir dir0, enum dir dir1);
int room_dist (struct room_linking *rlink, size_t room_nmemb,
               int r0, int r1, int max);
int min_room_dist (size_t room_nmemb;
                   struct room_dist room[room_nmemb],
                   size_t room_nmemb, int *dmax);
bool is_room_accessible (struct room_linking *rlink, size_t room_nmemb,
                         int room_from, int room_to);
bool is_room_accessible_from_kid_start (struct level *l, int room);
struct coord *new_coord (struct coord *c, struct level *l, int room,
                         lua_Number x, lua_Number y);
struct coord *invalid_coord (struct coord *c);
bool is_valid_coord (struct coord *c);
bool coord_eq (struct coord *_c0, struct coord *_c1);
struct coord *ncoord (struct coord *c, struct coord *nc);
struct pos *new_pos (struct pos *p, struct level *l,
                     int room, int floor, int place);
struct pos *invalid_pos (struct pos *p);
bool is_valid_pos (struct pos *p);
struct pos *npos (struct pos *p, struct pos *np);
struct coord *nframe (struct frame *f, struct coord *c);
struct pos *pos2room (struct pos *p, int room, struct pos *pv);
struct coord *coord2room (struct coord *c, int room, struct coord *cv);
struct coord *frame2room (struct frame *f, int room, struct coord *cv);
struct rect *new_rect (struct rect *r, int room, lua_Number x, lua_Number y,
                       lua_Number w, lua_Number h);
struct rect *coord2rect (struct coord *c, struct rect *r);
bool rect_eq (struct rect *r0, struct rect *r1);
struct coord *rect2room (struct rect *r, int room, struct coord *cv);
bool coord4draw (struct coord *c, int room, struct coord *cv);
int pos_mod (struct pos *p0, struct pos *p1);
int coord_mod (struct coord *c);
struct pos *pos_gen (struct coord *c, struct pos *p,
                     lua_Number dx, lua_Number dy);
struct pos *posb (struct coord *c, struct pos *p);
struct pos *pos (struct coord *c, struct pos *p);
struct pos *posf (struct coord *c, struct pos *p);
struct pos *prel (struct pos *p, struct pos *pr, int floor, int place);
int cpos (struct pos *p0, struct pos *p1);
int cpos_by_room (struct pos *p0, struct pos *p1);
int ccoord (struct coord *c0, struct coord *c1);
bool peq (struct pos *p0, struct pos *p1);
bool peqr (struct pos *p0, struct pos *p1, int floor, int place);
struct pos *reflect_pos_h (struct pos *p0, struct pos *p1);
struct pos *reflect_pos_v (struct pos *p0, struct pos *p1);
struct pos *random_pos (struct level *l, struct pos *p);
struct pos *get_new_rel_pos (struct pos *old_src, struct pos *old_dest,
                             struct pos *new_src, struct pos *new_dest);
enum dir random_dir (void);
lua_Number dist_coord (struct coord *a, struct coord *b);
struct frame *place_frame (struct frame *f, struct frame *nf,
                           ALLEGRO_BITMAP *b, struct pos *p,
                           lua_Number dx, lua_Number dy);
coord_f opposite_cf (coord_f cf);
enum dir opposite_dir (enum dir dir);
enum dir perpendicular_dir (enum dir dir, int n);
void dir_dx_dy (enum dir d, int *dx, int *dy);

struct coord *
tile_coord (struct pos *p, coord_f cf, struct coord *c);

struct dim *dim (struct frame *f, struct dim *d);
struct coord *_m (struct frame *f, struct coord *c);
struct coord *_mt (struct frame *f, struct coord *c);
struct coord *_mbo (struct frame *f, struct coord *c);
struct coord *_ml (struct frame *f, struct coord *c);
struct coord *_mr (struct frame *f, struct coord *c);
struct coord *_mf (struct frame *f, struct coord *c);
struct coord *_mba (struct frame *f, struct coord *c);
struct coord *_tl (struct frame *f, struct coord *c);
struct coord *_tr (struct frame *f, struct coord *c);
struct coord *_tf (struct frame *f, struct coord *c);
struct coord *_tb (struct frame *f, struct coord *c);
struct coord *_bl (struct frame *f, struct coord *c);
struct coord *_br (struct frame *f, struct coord *c);
struct coord *_bf (struct frame *f, struct coord *c);
struct coord *_bb (struct frame *f, struct coord *c);

coord_f str2coord_f (const char *name);

struct coord *
cf_rel (coord_f cf, struct frame *f, struct coord *c,
        lua_Number dx, lua_Number dy);
int cf_inc (struct frame *f, coord_f cf);

void
survey (coord_f cf, pos_f pf, struct frame *f,
        struct coord *c, struct pos *p, struct pos *np);
void
surveyo (coord_f cf, lua_Number dx, lua_Number dy, pos_f pf, struct frame *f,
         struct coord *c, struct pos *p, struct pos *np);

struct link_survey *link_survey (struct link_survey *ls,
                                 struct room_linking *rlink,
                                 size_t room_nmemb, int room);
struct pos_survey *pos_survey (struct pos_survey *ps, struct pos *p);

bool is_pixel_transparent (ALLEGRO_BITMAP *bitmap, int x, int y);
int compare_bitmap_rcoords (const void *_c0, const void *_c1);
struct bitmap_rcoord *get_cached_bitmap_rcoord (ALLEGRO_BITMAP *b,
                                                struct bitmap_rcoord *c);
struct bitmap_rcoord *bitmap_rcoord (ALLEGRO_BITMAP *b,
                                     struct bitmap_rcoord *c);

struct coord *place_on_the_ground_alternative (struct frame *f,
                                               struct coord *c,
                                               lua_Number dy);
struct coord *place_on_the_ground (struct frame *f, struct coord *c);
struct coord *place_at_distance (struct frame *f0, coord_f cf0,
                                 struct frame *f1, coord_f cf1,
                                 int d, enum dir dir, struct coord *c);
struct coord *
place_at_pos (struct frame *f, coord_f cf, struct pos *p, struct coord *c);

#endif	/* MININIM_POSITION_H */
