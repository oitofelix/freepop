/*
  multi-room.c -- multi-room module;

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

#include "mininim.h"

ALLEGRO_BITMAP *cache;
bool con_caching;

struct multi_room mr;

struct pos *changed_pos = NULL;
size_t changed_pos_nmemb = 0;

void
register_changed_pos (struct pos *p)
{
  struct pos np; npos (p, &np);

  if (get_changed_pos (&np)) return;

  changed_pos =
    add_to_array (&np, 1, changed_pos, &changed_pos_nmemb,
                  changed_pos_nmemb, sizeof (np));

  qsort (changed_pos, changed_pos_nmemb, sizeof (np), (m_comparison_fn_t) cpos);
}

struct pos *
get_changed_pos (struct pos *p)
{
  return bsearch (p, changed_pos, changed_pos_nmemb, sizeof (* p),
                  (m_comparison_fn_t) cpos);
}

static void
destroy_multi_room (void)
{
  int x, y;

  if (mr.cell) {
    for (x = 0; x < mr.w; x++) {
      for (y = 0; y < mr.h; y++) destroy_bitmap (mr.cell[x][y].screen);
      al_free (mr.cell[x]);
      al_free (mr.last.cell[x]);
    }
    al_free (mr.cell);
    al_free (mr.last.cell);
  };

  destroy_bitmap (screen);
  destroy_bitmap (effect_buffer);
  destroy_bitmap (black_screen);
  destroy_bitmap (cache);
}

bool
set_multi_room (int w, int h)
{
  if (w == mr.w && h == mr.h) return true;

  int sw = ORIGINAL_WIDTH * w;
  int sh = ROOM_HEIGHT * h + 11;

  ALLEGRO_BITMAP *b = create_bitmap (sw, sh);
  if (! b) return false;
  destroy_bitmap (b);

  destroy_multi_room ();

  mr.h = h;
  mr.w = w;

  set_target_backbuffer (display);
  al_set_new_bitmap_flags (ALLEGRO_VIDEO_BITMAP);

  screen = create_bitmap (sw, sh);
  effect_buffer = create_bitmap (sw, sh);
  black_screen = create_bitmap (sw, sh);
  cache = create_bitmap (sw, sh);

  int x, y;
  mr.cell = xcalloc (w, sizeof (* mr.cell));
  mr.last.cell = xcalloc (w, sizeof (* mr.last.cell));
  for (x = 0; x < w; x++) {
    mr.cell[x] = xcalloc (h, sizeof (** mr.cell));
    mr.last.cell[x] = xcalloc (h, sizeof (** mr.last.cell));
    for (y = 0; y < h; y++) {
      int x0 = ORIGINAL_WIDTH * x;
      int y0 = ROOM_HEIGHT * y;
      int sw = ORIGINAL_WIDTH;
      int sh = ORIGINAL_HEIGHT - (y < h - 1 ? 8 : 0);
      mr.cell[x][y].screen = al_create_sub_bitmap (screen, x0, y0, sw, sh);
      mr.cell[x][y].cache = al_create_sub_bitmap (cache, x0, y0, sw, sh);
    }
  }

  return true;
}

void
clear_multi_room_cells (void)
{
  int x, y;
  for (x = 0; x < mr.w; x++)
    for (y = 0; y < mr.h; y++) {
      mr.cell[x][y].done = false;
      mr.cell[x][y].room = -1;
    }
}

void
mr_map_room_adj (int r, int x, int y)
{
  int rl = roomd (r, LEFT);
  int rr = roomd (r, RIGHT);
  int ra = roomd (r, ABOVE);
  int rb = roomd (r, BELOW);

  mr.cell[x][y].room = r;
  mr.cell[x][y].done = true;
  if (x > 0) mr.cell[x - 1][y].room = rl;
  if (x < mr.w - 1) mr.cell[x + 1][y].room = rr;
  if (y > 0) mr.cell[x][y - 1].room = ra;
  if (y < mr.h - 1) mr.cell[x][y + 1].room = rb;
}

bool
next_multi_room_cell (int *rx, int *ry)
{
  int x, y;
  for (x = 0; x < mr.w; x++)
    for (y = 0; y < mr.h; y++)
      if (mr.cell[x][y].room > 0
          && ! mr.cell[x][y].done) {
        *rx = x;
        *ry = y;
        return true;
      }

  return false;
}

void
mr_map_rooms (void)
{
  int x, y;
  clear_multi_room_cells ();
  mr_map_room_adj (mr.room, mr.x, mr.y);
  while (next_multi_room_cell (&x, &y))
    mr_map_room_adj (mr.cell[x][y].room, x, y);
  for (x = 0; x < mr.w; x++)
    for (y = 0; y < mr.h; y++) {
      if (mr.cell[x][y].room < 0) mr.cell[x][y].room = 0;
    }
}

int
mr_count_rooms (void)
{
  int x, y, c = 0;
  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++)
      c += (mr.cell[x][y].room > 0) ? 1 : 0;
  return c;
}

int
mr_count_no_rooms_above (void)
{
  int x, y, c = 0;
  for (y = 0; y < mr.h; y++) {
    for (x = 0; x < mr.w; x++)
      if (mr.cell[x][y].room > 0) return c;
    c++;
  }
  return c;
}

int
mr_count_no_rooms_below (void)
{
  int x, y, c = 0;
  for (y = mr.h - 1; y >= 0 ; y--) {
    for (x = 0; x < mr.w; x++)
      if (mr.cell[x][y].room > 0) return c;
    c++;
  }
  return c;
}

void
mr_center_room (int room)
{
  mr.room = room;

  int x, y, lc = 0, c = 0, ld = INT_MAX;
  int lx = mr.x;
  int ly = mr.y;
  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++) {
      mr.x = x;
      mr.y = y;
      mr_map_rooms ();
      c = mr_count_rooms ();
      int ca = mr_count_no_rooms_above ();
      int cb = mr_count_no_rooms_below ();
      int d = abs (ca - cb);
      if (c >= lc && (c > lc || d < ld)) {
        lx = x;
        ly = y;
        lc = c;
        ld = d;
      }
    }

  mr.x = lx;
  mr.y = ly;

  mr.select_cycles = SELECT_CYCLES;
}

void
mr_select_trans (enum dir d)
{
  int dx = +0, dy = +0;
  switch (d) {
  case LEFT: dx = -1; break;
  case RIGHT: dx = +1; break;
  case ABOVE: dy = -1; break;
  case BELOW: dy = +1; break;
  }

  int r = roomd (mr.room, d);
  if (r) {
    mr.room = r;
    nmr_coord (mr.x + dx, mr.y + dy, &mr.x, &mr.y);
  }

  mr.select_cycles = SELECT_CYCLES;
}

void
mr_view_trans (enum dir d)
{
  int x, y, dx = +0, dy = +0;

  mr.select_cycles = 0;

  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++) {
      int r = mr.cell[x][y].room;
      if (r <= 0) continue;
      r = roomd (r, d);
      if (r) {
       mr.room = r;
       mr.x = x;
       mr.y = y;
       return;
      }
    }

  switch (d) {
  case RIGHT:
    if (mr.x > 0) dx = -1; break;
  case LEFT:
    if (mr.x < mr.w - 1) dx = +1; break;
  case BELOW:
    if (mr.y > 0) dy = -1; break;
  case ABOVE:
    if (mr.y < mr.h - 1) dy = +1; break;
  }

  mr.x += dx;
  mr.y += dy;
}

bool
has_mr_view_changed (void)
{
  if (mr.w != mr.last.w || mr.h != mr.last.h) return true;

  int x, y;
  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++)
      if (mr.last.cell[x][y].room != mr.cell[x][y].room)
        return true;

  return false;
}

void
mr_update_last_settings (void)
{
  int x, y;
  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++)
      mr.last.cell[x][y].room = mr.cell[x][y].room;

  mr.last.w = mr.w;
  mr.last.h = mr.h;

  mr.last.level = level.number;
  mr.last.em = em;
  mr.last.vm = vm;
  mr.last.hgc = hgc;
  mr.last.hue = hue;
  mr.last.mouse_pos = mouse_pos;
}

void
draw_animated_background (ALLEGRO_BITMAP *bitmap, int room)
{
  int room_view_bkp = room_view;
  room_view = room;

  struct pos p;
  p.room = room_view;

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++) {
      draw_fire (bitmap, &p, vm);
      draw_balcony_stars (bitmap, &p, vm);
    }

  for (p.floor = FLOORS; p.floor >= 0; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++)
      draw_no_floor_selection (bitmap, &p);

  room_view = room_view_bkp;
}

void
draw_animated_foreground (ALLEGRO_BITMAP *bitmap, int room)
{
  int room_view_bkp = room_view;
  room_view = room;

  struct pos p;
  p.room = room_view;

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++) {
      if (con (&p)->fg != MIRROR) continue;
      update_mirror_bitmap (bitmap, &p);
      draw_mirror (bitmap, &p, em, vm);
    }

  /* loose_floor_fall_debug (); */

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++) {
      draw_falling_loose_floor (bitmap, &p, em, vm);
    }

  draw_anims (bitmap, em, vm);

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++) {
      draw_potion (bitmap, &p, em, vm);
      if (is_sword (&p)) draw_sword (bitmap, &p, vm);
    }

  room_view = room_view_bkp;
}

void
update_cache (enum em em, enum vm vm)
{
  int x, y;

  int room_view_bkp = room_view;

  clear_bitmap (cache, TRANSPARENT_COLOR);

  con_caching = true;

  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++) {
      room_view = mr.cell[x][y].room;
      mr.dx = x;
      mr.dy = y;
      draw_room (mr.cell[x][y].cache, room_view, em, vm);
    }

  con_caching = false;

  room_view = room_view_bkp;
}

void
update_cache_pos (struct pos *p, enum em em, enum vm vm)
{
  static bool recursive = false;

  int x, y;

  int room_view_bkp = room_view;

  struct pos pbl; prel (p, &pbl, +1, -1);
  struct pos pb; prel (p, &pb, +1, +0);
  struct pos pbr; prel (p, &pbr, +1, +1);

  struct pos pl; prel (p, &pl, +0, -1);
  struct pos pr; prel (p, &pr, +0, +1);

  struct pos pa; prel (p, &pa, -1, +0);
  struct pos pal; prel (p, &pal, -1, -1);
  struct pos par; prel (p, &par, -1, +1);

  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++)
      if (p->room && mr.cell[x][y].room == p->room) {
        room_view = p->room;
        mr.dx = x;
        mr.dy = y;

        int cx, cy, cw, ch;
        switch (con (p)->fg) {
        default:
          cx = PLACE_WIDTH * p->place;
          cy = PLACE_HEIGHT * p->floor - 10;
          cw = 2 * PLACE_WIDTH;
          ch = PLACE_HEIGHT + 3 + 10;
          break;
        }

        set_target_bitmap (mr.cell[x][y].cache);
        al_set_clipping_rectangle (cx, cy, cw, ch);
        al_clear_to_color (TRANSPARENT_COLOR);

        con_caching = true;

        draw_conbg (mr.cell[x][y].cache, &pbl, em, vm);
        draw_conbg (mr.cell[x][y].cache, &pb, em, vm);
        draw_conbg (mr.cell[x][y].cache, &pbr, em, vm);

        draw_conbg (mr.cell[x][y].cache, &pl, em, vm);
        draw_conbg (mr.cell[x][y].cache, p, em, vm);
        draw_conbg (mr.cell[x][y].cache, &pr, em, vm);

        draw_conbg (mr.cell[x][y].cache, &pal, em, vm);
        draw_conbg (mr.cell[x][y].cache, &pa, em, vm);
        draw_conbg (mr.cell[x][y].cache, &par, em, vm);

        draw_confg_right (mr.cell[x][y].cache, &pbl, em, vm, true);
        draw_confg_right (mr.cell[x][y].cache, &pb, em, vm, true);
        draw_confg_right (mr.cell[x][y].cache, &pl, em, vm, false);

        draw_confg (mr.cell[x][y].cache, p, em, vm, true);

        draw_confg_right (mr.cell[x][y].cache, &pal, em, vm, true);
        draw_confg (mr.cell[x][y].cache, &pa, em, vm, true);
        draw_confg_base (mr.cell[x][y].cache, &par, em, vm);
        draw_confg_left (mr.cell[x][y].cache, &par, em, vm, false);

        al_reset_clipping_rectangle ();
        al_hold_bitmap_drawing (false);
        con_caching = false;
      }

  /* printf ("%i,%i,%i\n", p->room, p->floor, p->place); */

  if (! recursive && p->place == -1) {
    struct pos p0;
    p0.room = roomd (p->room, LEFT);
    p0.floor = p->floor;
    p0.place = PLACES - 1;
    recursive = true;
    update_cache_pos (&p0, em, vm);
    recursive = false;
  }

  if (! recursive && p->floor == -1) {
    struct pos p0;
    p0.room = roomd (p->room, ABOVE);
    p0.floor = FLOORS - 1;
    p0.place = p->place;
    recursive = true;
    update_cache_pos (&p0, em, vm);
    recursive = false;
  }

  if (! recursive && p->place == PLACES - 1) {
    struct pos p0;
    p0.room = roomd (p->room, RIGHT);
    p0.floor = p->floor;
    p0.place = -1;
    recursive = true;
    update_cache_pos (&p0, em, vm);
    recursive = false;
  }

  if (! recursive && p->floor == FLOORS - 1) {
    struct pos p0;
    p0.room = roomd (p->room, BELOW);
    p0.floor = -1;
    p0.place = p->place;
    recursive = true;
    update_cache_pos (&p0, em, vm);
    recursive = false;
  }

  if (! recursive && p->floor == -1 && p->place == -1) {
    struct pos p0;
    p0.room = roomd (p->room, ABOVE);
    p0.room = roomd (p0.room, LEFT);
    p0.floor = FLOORS - 1;
    p0.place = PLACES - 1;
    recursive = true;
    update_cache_pos (&p0, em, vm);
    recursive = false;
  }

  if (! recursive && p->floor == -1 && p->place == PLACES - 1) {
    struct pos p0;
    p0.room = roomd (p->room, ABOVE);
    p0.room = roomd (p0.room, RIGHT);
    p0.floor = FLOORS - 1;
    p0.place = -1;
    recursive = true;
    update_cache_pos (&p0, em, vm);
    recursive = false;
  }

  if (! recursive && p->floor == FLOORS - 1 && p->place == -1) {
    struct pos p0;
    p0.room = roomd (p->room, LEFT);
    p0.room = roomd (p0.room, BELOW);
    p0.floor = -1;
    p0.place = PLACES - 1;
    recursive = true;
    update_cache_pos (&p0, em, vm);
    recursive = false;
  }

  if (! recursive && p->floor == FLOORS - 1 && p->place == PLACES - 1) {
    struct pos p0;
    p0.room = roomd (p->room, BELOW);
    p0.room = roomd (p0.room, RIGHT);
    p0.floor = -1;
    p0.place = -1;
    recursive = true;
    update_cache_pos (&p0, em, vm);
    recursive = false;
  }

  /* if (! recursive) printf ("----------------------------\n"); */

  room_view = room_view_bkp;
}

void
draw_multi_rooms (void)
{
  int x, y;

  mr_map_rooms ();

  if (em == PALACE && vm == VGA
      && (has_mr_view_changed ()
          || em != mr.last.em
          || vm != mr.last.vm))
    generate_wall_colors ();

  if (has_mr_view_changed ()) {
    generate_stars ();
    generate_mirrors_reflex ();
  }

  if (mouse_pos.room != mr.last.mouse_pos.room
      || mouse_pos.floor != mr.last.mouse_pos.floor
      || mouse_pos.place != mr.last.mouse_pos.place) {
    if (is_valid_pos (&mouse_pos))
      update_cache_pos (&mouse_pos, em, vm);
    if (is_valid_pos (&mr.last.mouse_pos))
      update_cache_pos (&mr.last.mouse_pos, em, vm);
  }

  if (anim_cycle == 0
      || has_mr_view_changed ()
      || em != mr.last.em
      || vm != mr.last.vm
      || hgc != mr.last.hgc
      || hue != mr.last.hue
      || level.number != mr.last.level) {
    update_cache (em, vm);
  }

  size_t i;
  for (i = 0; i < changed_pos_nmemb; i++) {
    /* printf ("%i,%i,%i\n", changed_pos[i].room, changed_pos[i].floor, changed_pos[i].place); */
    update_cache_pos (&changed_pos[i], em, vm);
  }
  destroy_array ((void **) &changed_pos, &changed_pos_nmemb);

  clear_bitmap (screen, BLACK);

  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++) {
      if (! mr.cell[x][y].room) continue;
      mr.dx = x;
      mr.dy = y;
      draw_animated_background (mr.cell[x][y].screen, mr.cell[x][y].room);
    }

  if (! no_room_drawing) draw_bitmap (cache, screen, 0, 0, 0);

  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++) {
      if (! mr.cell[x][y].room) continue;
      mr.dx = x;
      mr.dy = y;
      draw_animated_foreground (mr.cell[x][y].screen, mr.cell[x][y].room);
    }

  if (mr.select_cycles > 0) {
    al_hold_bitmap_drawing (false);

    int x0 = ORIGINAL_WIDTH * mr.x;
    int y0 = ROOM_HEIGHT * mr.y + 3;
    int x1 = x0 + ORIGINAL_WIDTH;
    int y1 = y0 + ROOM_HEIGHT;
    draw_rectangle (screen, x0, y0, x1, y1, RED, 1);

    mr.select_cycles--;
  }

  mr.dx = mr.dy = -1;

  mr_update_last_settings ();
}

bool
is_room_visible (int room)
{
  int x, y;
  for (x = 0; x < mr.w; x++)
    for (y = 0; y < mr.h; y++)
      if (mr.cell[x][y].room == room)
        return true;

  return false;
}

bool
is_kid_visible (void)
{
  struct anim *k = get_anim_by_id (current_kid_id);
  return is_room_visible (k->f.c.room);
}

void
nmr_coord (int x, int y, int *rx, int *ry)
{
  *rx = x;
  *ry = y;

  bool m;

  do {
    m = false;

    if (*ry < 0) {
      *ry += mr.h;
      m = true;
    } else if (*ry >= mr.h) {
      *ry -= mr.h;
      m = true;
    } else if (*rx < 0) {
      *rx += mr.w;
      m = true;
    } else if (*rx >= mr.w) {
      *rx -= mr.w;
      m = true;
    }
  } while (m);
}

bool
mr_coord (int room, enum dir dir, int *rx, int *ry)
{
  int x, y;
  for (x = 0; x < mr.w; x++)
    for (y = 0; y < mr.h; y++)
      if (mr.cell[x][y].room == room) {
        switch (dir) {
        case LEFT:
          nmr_coord (x - 1, y, rx, ry);
          return true;
        case RIGHT:
          nmr_coord (x + 1, y, rx, ry);
          return true;
        case ABOVE:
          nmr_coord (x, y - 1, rx, ry);
          return true;
        case BELOW:
          nmr_coord (x, y + 1, rx, ry);
          return true;
        default:
          *rx = x;
          *ry = y;
          return true;
        }
      }

  return false;
}

bool
ui_set_multi_room (int dw, int dh)
{
  char *text;

  if (mr.w + dw < 1 || mr.h + dh < 1) {
    xasprintf (&text, "MULTI-ROOM %ix%i", mr_w, mr_h);
    draw_bottom_text (NULL, text, 0);
    al_free (text);
    return false;
  }

  if (! set_multi_room (mr.w + dw, mr.h + dh)) {
    draw_bottom_text (NULL, "VIDEO CARD LIMIT REACHED", 0);
    return false;
  }

  mr_w = mr.w;
  mr_h = mr.h;
  mr_center_room (mr.room);
  xasprintf (&text, "MULTI-ROOM %ix%i", mr_w, mr_h);
  draw_bottom_text (NULL, text, 0);
  al_free (text);
  return true;
}
