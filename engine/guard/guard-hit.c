/*
  guard-hit.c -- guard hit module;

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

#include "prince.h"
#include "kernel/video.h"
#include "kernel/keyboard.h"
#include "engine/anim.h"
#include "engine/physics.h"
#include "engine/door.h"
#include "engine/potion.h"
#include "engine/sword.h"
#include "engine/loose-floor.h"
#include "guard.h"

struct frameset guard_hit_frameset[GUARD_HIT_FRAMESET_NMEMB];
struct frameset fat_guard_hit_frameset[GUARD_HIT_FRAMESET_NMEMB];
struct frameset vizier_hit_frameset[GUARD_HIT_FRAMESET_NMEMB];
struct frameset skeleton_hit_frameset[GUARD_HIT_FRAMESET_NMEMB];

static void init_guard_hit_frameset (void);
static void init_fat_guard_hit_frameset (void);
static void init_vizier_hit_frameset (void);
static void init_skeleton_hit_frameset (void);
static bool flow (struct anim *g);
static bool physics_in (struct anim *g);
static void physics_out (struct anim *g);

/* guard */
ALLEGRO_BITMAP *guard_hit_01, *guard_hit_02,
  *guard_hit_03, *guard_hit_04, *guard_hit_05;

/* fat guard */
ALLEGRO_BITMAP *fat_guard_hit_01, *fat_guard_hit_02,
  *fat_guard_hit_03, *fat_guard_hit_04, *fat_guard_hit_05;

/* vizier */
ALLEGRO_BITMAP *vizier_hit_01, *vizier_hit_02,
  *vizier_hit_03, *vizier_hit_04, *vizier_hit_05;

/* skeleton */
ALLEGRO_BITMAP *skeleton_hit_01, *skeleton_hit_02,
  *skeleton_hit_03, *skeleton_hit_04, *skeleton_hit_05;

static void
init_guard_hit_frameset (void)
{
  struct frameset frameset[GUARD_HIT_FRAMESET_NMEMB] =
    {{guard_hit_01,+0,0},{guard_hit_02,+0,0},
     {guard_hit_03,+4,0},{guard_hit_04,+8,0},
     {guard_hit_05,+8,0}};

  memcpy (&guard_hit_frameset, &frameset,
          GUARD_HIT_FRAMESET_NMEMB * sizeof (struct frameset));
}

static void
init_fat_guard_hit_frameset (void)
{
  struct frameset frameset[GUARD_HIT_FRAMESET_NMEMB] =
    {{fat_guard_hit_01,+0,0},{fat_guard_hit_02,+0,0},
     {fat_guard_hit_03,+4,0},{fat_guard_hit_04,+8,0},
     {fat_guard_hit_05,+8,0}};

  memcpy (&fat_guard_hit_frameset, &frameset,
          GUARD_HIT_FRAMESET_NMEMB * sizeof (struct frameset));
}

static void
init_vizier_hit_frameset (void)
{
  struct frameset frameset[GUARD_HIT_FRAMESET_NMEMB] =
    {{vizier_hit_01,+0,0},{vizier_hit_02,+0,0},
     {vizier_hit_03,+4,0},{vizier_hit_04,+8,0},
     {vizier_hit_05,+8,0}};

  memcpy (&vizier_hit_frameset, &frameset,
          GUARD_HIT_FRAMESET_NMEMB * sizeof (struct frameset));
}

static void
init_skeleton_hit_frameset (void)
{
  struct frameset frameset[GUARD_HIT_FRAMESET_NMEMB] =
    {{skeleton_hit_01,+0,0},{skeleton_hit_02,+0,0},
     {skeleton_hit_03,+4,0},{skeleton_hit_04,+8,0},
     {skeleton_hit_05,+8,0}};

  memcpy (&skeleton_hit_frameset, &frameset,
          GUARD_HIT_FRAMESET_NMEMB * sizeof (struct frameset));
}

struct frameset *
get_guard_hit_frameset (enum anim_type t)
{
  switch (t) {
  case GUARD: default: return guard_hit_frameset;
  case FAT_GUARD: return fat_guard_hit_frameset;
  case VIZIER: return vizier_hit_frameset;
  case SKELETON: return skeleton_hit_frameset;
  }
}

void
load_guard_hit (void)
{
  /* guard */
  guard_hit_01 = load_bitmap (GUARD_HIT_01);
  guard_hit_02 = load_bitmap (GUARD_HIT_02);
  guard_hit_03 = load_bitmap (GUARD_HIT_03);
  guard_hit_04 = load_bitmap (GUARD_HIT_04);
  guard_hit_05 = load_bitmap (GUARD_HIT_05);

  /* fat guard */
  fat_guard_hit_01 = load_bitmap (FAT_GUARD_HIT_01);
  fat_guard_hit_02 = load_bitmap (FAT_GUARD_HIT_02);
  fat_guard_hit_03 = load_bitmap (FAT_GUARD_HIT_03);
  fat_guard_hit_04 = load_bitmap (FAT_GUARD_HIT_04);
  fat_guard_hit_05 = load_bitmap (FAT_GUARD_HIT_05);

  /* vizier */
  vizier_hit_01 = load_bitmap (VIZIER_HIT_01);
  vizier_hit_02 = load_bitmap (VIZIER_HIT_02);
  vizier_hit_03 = load_bitmap (VIZIER_HIT_03);
  vizier_hit_04 = load_bitmap (VIZIER_HIT_04);
  vizier_hit_05 = load_bitmap (VIZIER_HIT_05);

  /* skeleton */
  skeleton_hit_01 = load_bitmap (SKELETON_HIT_01);
  skeleton_hit_02 = load_bitmap (SKELETON_HIT_02);
  skeleton_hit_03 = load_bitmap (SKELETON_HIT_03);
  skeleton_hit_04 = load_bitmap (SKELETON_HIT_04);
  skeleton_hit_05 = load_bitmap (SKELETON_HIT_05);

  /* frameset */
  init_guard_hit_frameset ();
  init_fat_guard_hit_frameset ();
  init_vizier_hit_frameset ();
  init_skeleton_hit_frameset ();
}

void
unload_guard_hit (void)
{
  /* guard */
  al_destroy_bitmap (guard_hit_01);
  al_destroy_bitmap (guard_hit_02);
  al_destroy_bitmap (guard_hit_03);
  al_destroy_bitmap (guard_hit_04);
  al_destroy_bitmap (guard_hit_05);

  /* fat guard */
  al_destroy_bitmap (fat_guard_hit_01);
  al_destroy_bitmap (fat_guard_hit_02);
  al_destroy_bitmap (fat_guard_hit_03);
  al_destroy_bitmap (fat_guard_hit_04);
  al_destroy_bitmap (fat_guard_hit_05);

  /* vizier */
  al_destroy_bitmap (vizier_hit_01);
  al_destroy_bitmap (vizier_hit_02);
  al_destroy_bitmap (vizier_hit_03);
  al_destroy_bitmap (vizier_hit_04);
  al_destroy_bitmap (vizier_hit_05);

  /* skeleton */
  al_destroy_bitmap (skeleton_hit_01);
  al_destroy_bitmap (skeleton_hit_02);
  al_destroy_bitmap (skeleton_hit_03);
  al_destroy_bitmap (skeleton_hit_04);
  al_destroy_bitmap (skeleton_hit_05);
}

void
guard_hit (struct anim *g)
{
  g->oaction = g->action;
  g->action = guard_hit;
  g->f.flip = (g->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (g)) return;
  if (! physics_in (g)) return;
  next_frame (&g->f, &g->f, &g->fo);
  physics_out (g);
}

static bool
flow (struct anim *g)
{
  if (g->oaction != guard_hit) {
    g->i = -1;
    g->j = 0;
  }

  if (g->i == 4) {
    guard_vigilant (g);
    return false;
  }

  struct frameset *frameset = get_guard_hit_frameset (g->type);
  select_frame (g, frameset, g->i + 1);

  if (g->i == 0) g->j = 28;
  if (g->i == 1) g->j = 32;
  if (g->i == 3) g->j = 7;
  if (g->i == 4) g->j = 3;

  select_xframe (&g->xf, sword_frameset, g->j);

  if (g->i == 0) g->xf.dx = -12, g->xf.dy = +4;
  if (g->i == 2) g->xf.b = NULL;
  if (g->i == 3) g->xf.dx = +0, g->xf.dy = +23;

  if (g->type == SKELETON) g->xf.dy += -3;

  return true;
}

static bool
physics_in (struct anim *g)
{
  struct coord nc; struct pos np, pbf, pmbo, pbb;

  /* collision */
  uncollide (&g->f, &g->fo, &g->fo, +8, true, &g->ci);

  /* fall */
  survey (_bf, pos, &g->f, &nc, &pbf, &np);
  survey (_mbo, pos, &g->f, &nc, &pmbo, &np);
  survey (_bb, pos, &g->f, &nc, &pbb, &np);
  if (is_strictly_traversable (&pbf)
      || is_strictly_traversable (&pmbo)
      || is_strictly_traversable (&pbb)) {
    g->xf.b = NULL;
    guard_fall (g);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *g)
{
  /* depressible floors */
  if (g->i == 3) update_depressible_floor (g, -3, -33);
  else if (g->i == 4) update_depressible_floor (g, -2, -24);
  else keep_depressible_floor (g);
}
