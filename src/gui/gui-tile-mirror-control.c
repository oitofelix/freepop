/*
  gui-tile-mirror-control.c -- editor GUI tile mirror control module;

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

#include "mininim.h"

static int _update_cb (Ihandle *ih);
static int toggle_action_cb (Ihandle *ih, int state);
static int button_action_cb (Ihandle *ih);

static void apply_to_sel_ring (void (*f) (struct pos *, char *),
                               struct sel_ring *sr, char *desc);
static void vh_room_tiles_links (struct pos *p, char *desc);
static void vh_room_tiles (struct pos *p, char *desc);
static void vh_room_links (struct pos *p, char *desc);
static void v_room_tiles_links (struct pos *p, char *desc);
static void v_room_tiles (struct pos *p, char *desc);
static void v_room_links (struct pos *p, char *desc);
static void h_room_tiles_links (struct pos *p, char *desc);
static void h_room_tiles (struct pos *p, char *desc);
static void h_room_links (struct pos *p, char *desc);


Ihandle *
gui_create_tile_mirror_control (struct pos *p, struct sel_ring *sr,
                                char *norm_group)
{
  Ihandle *ih, *vbox, *sel_ring_toggle, *radio, *tiles_toggle, *links_toggle,
    *place_toggle, *room_toggle, *level_toggle;

  Ihandle *gridbox, *v_button, *vh_button, *h_button, *r_button;

  v_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = V_ICON,"
      "TIP = \"Vertical\""),
     "ACTION", button_action_cb,
     NULL);

  vh_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = VH_ICON,"
      "TIP = \"Vertical+Horizontal\""),
     "ACTION", button_action_cb,
     NULL);

  h_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = H_ICON,"
      "TIP = \"Horizontal\""),
     "ACTION", button_action_cb,
     NULL);

  r_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = SHUFFLE_ICON,"
      "TIP = \"Random\""),
     "ACTION", button_action_cb,
     NULL);

  gridbox = IupSetAttributes
    (IupGridBox
     (v_button,
      vh_button,
      h_button,
      r_button,
      NULL),
     "ORIENTATION = HORIZONTAL,"
     "NUMDIV = 4,"
     "SIZECOL = -1,"
     "SIZELIN = -1,"
     "NORMALIZESIZE = BOTH,");

  sel_ring_toggle = IupSetAttributes
    (IupToggle ("S", NULL),
     "VALUE = YES,"
     "TIP = \"Apply to selection set\",");

  place_toggle = IupSetCallbacks
    (IupSetAttributes
     (IupToggle ("P", NULL),
      "TIP = \"Place scope\""),
     "ACTION", (Icallback) toggle_action_cb,
     NULL);

  room_toggle = IupSetCallbacks
    (IupSetAttributes
     (IupToggle ("R", NULL),
      "TIP = \"Room scope\""),
     "ACTION", (Icallback) toggle_action_cb,
     NULL);

  level_toggle = IupSetCallbacks
    (IupSetAttributes
     (IupToggle ("L", NULL),
      "TIP = \"Level scope\""),
     "ACTION", (Icallback) toggle_action_cb,
     NULL);

  radio = IupRadio
    (IupHbox
     (place_toggle,
      room_toggle,
      level_toggle,
      NULL));

  tiles_toggle = IupSetCallbacks
    (IupSetAttributes
     (IupToggle ("T", NULL),
      "VALUE = YES,"
      "ACTIVE = NO,"
      "TIP = \"Tiles\""),
     "ACTION", (Icallback) toggle_action_cb,
     NULL);

  links_toggle = IupSetCallbacks
    (IupSetAttributes
     (IupToggle ("L", NULL),
      "VALUE = YES,"
      "ACTIVE = NO,"
      "TIP = \"Links\""),
     "ACTION", (Icallback) toggle_action_cb,
     NULL);

  ih = IupSetCallbacks
    (IupSetAttributes
     (IupFrame
      (vbox = IupSetAttributes
       (IupVbox
        (IupFill (),
         gridbox,
         sel_ring_toggle,
         radio,
         IupHbox
         (tiles_toggle,
          links_toggle,
          NULL),
         IupFill (),
         NULL),
        "ALIGNMENT = ACENTER")),
      "TITLE = Mirror"),
     "_UPDATE_CB", _update_cb,
     NULL);

  IupSetAttribute (vbox, "NORMALIZERGROUP", norm_group);

  IupSetAttribute (ih, "_V_BUTTON", (void *) v_button);
  IupSetAttribute (ih, "_VH_BUTTON", (void *) vh_button);
  IupSetAttribute (ih, "_H_BUTTON", (void *) h_button);
  IupSetAttribute (ih, "_R_BUTTON", (void *) r_button);
  IupSetAttribute (ih, "_SEL_RING_TOGGLE", (void *) sel_ring_toggle);
  IupSetAttribute (ih, "_RADIO", (void *) radio);
  IupSetAttribute (ih, "_PLACE_TOGGLE", (void *) place_toggle);
  IupSetAttribute (ih, "_ROOM_TOGGLE", (void *) room_toggle);
  IupSetAttribute (ih, "_LEVEL_TOGGLE", (void *) level_toggle);
  IupSetAttribute (ih, "_TILES_TOGGLE", (void *) tiles_toggle);
  IupSetAttribute (ih, "_LINKS_TOGGLE", (void *) links_toggle);
  IupSetAttribute (ih, "_POS", (void *) p);
  IupSetAttribute (ih, "_SEL_RING", (void *) sr);

  return ih;
}

int
_update_cb (Ihandle *ih)
{
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

  Ihandle *radio = (void *) IupGetAttribute (ih, "_RADIO");
  Ihandle *room_toggle = (void *) IupGetAttribute (ih, "_ROOM_TOGGLE");
  Ihandle *scope = (void *) IupGetAttribute (radio, "VALUE_HANDLE");

  struct pos *p = (void *) IupGetAttribute (ih, "_POS");
  struct sel_ring *sr = (void *) IupGetAttribute (ih, "_SEL_RING");
  gui_control_active (ih, is_valid_pos (p)
                      || (scope == room_toggle && sel_ring_ss_c_nmemb (sr)));

  return IUP_DEFAULT;
}

int
toggle_action_cb (Ihandle *ih, int state)
{
  Ihandle *tiles_toggle = (void *) IupGetAttribute (ih, "_TILES_TOGGLE");
  Ihandle *links_toggle = (void *) IupGetAttribute (ih, "_LINKS_TOGGLE");
  Ihandle *place_toggle =
    (void *) IupGetAttribute (ih, "_PLACE_TOGGLE");

  if (ih == tiles_toggle && ! state)
    IupSetInt (links_toggle, "VALUE", true);
  else if (ih == links_toggle && ! state)
    IupSetInt (tiles_toggle, "VALUE", true);
  else {
    gui_control_active (tiles_toggle, ih != place_toggle);
    gui_control_active (links_toggle, ih != place_toggle);
  }

  return IUP_DEFAULT;
}

void
apply_to_sel_ring (void (*f) (struct pos *, char *),
                        struct sel_ring *sr, char *desc)
{
  struct pos p; new_pos (&p, sr->ss[0].rs[0].level, -1, 0, 0);
  for (p.room = 1; p.room < p.l->room_nmemb; p.room++) {
    if (! is_room_in_sel_ring (sr, p.room)) continue;
    f (&p, NULL);
  }
  end_undo_set (&undo, desc);
}

void
vh_room_tiles_links (struct pos *p, char *desc)
{
  register_h_room_mirror_tile_undo
    (&undo, p->room, NULL);
  register_v_room_mirror_tile_undo
    (&undo, p->room, NULL);
  struct room_linking *rlink =
    copy_array (p->l->rlink, p->l->room_nmemb, NULL, sizeof (*rlink));
  editor_mirror_link (rlink, p->l->room_nmemb, p->room, LEFT, RIGHT);
  editor_mirror_link (rlink, p->l->room_nmemb, p->room, ABOVE, BELOW);
  register_link_undo (&undo, rlink, desc);
  destroy_array ((void **) &rlink, NULL);
}

void
vh_room_tiles (struct pos *p, char *desc)
{
  register_h_room_mirror_tile_undo
    (&undo, p->room, NULL);
  register_v_room_mirror_tile_undo
    (&undo, p->room, desc);
}

void
vh_room_links (struct pos *p, char *desc)
{
  struct room_linking *rlink =
    copy_array (p->l->rlink, p->l->room_nmemb, NULL, sizeof (*rlink));
  editor_mirror_link (rlink, p->l->room_nmemb, p->room, LEFT, RIGHT);
  editor_mirror_link (rlink, p->l->room_nmemb, p->room, ABOVE, BELOW);
  register_link_undo (&undo, rlink, desc);
  destroy_array ((void **) &rlink, NULL);
}

void
v_room_tiles_links (struct pos *p, char *desc)
{
  register_v_room_mirror_tile_undo (&undo, p->room, NULL);
  struct room_linking *rlink =
    copy_array (p->l->rlink, p->l->room_nmemb, NULL, sizeof (*rlink));
  editor_mirror_link (rlink, p->l->room_nmemb, p->room, ABOVE, BELOW);
  register_link_undo (&undo, rlink, desc);
  destroy_array ((void **) &rlink, NULL);
}

void
v_room_tiles (struct pos *p, char *desc)
{
  register_v_room_mirror_tile_undo
    (&undo, p->room, desc);
}

void
v_room_links (struct pos *p, char *desc)
{
  struct room_linking *rlink =
    copy_array (p->l->rlink, p->l->room_nmemb, NULL, sizeof (*rlink));
  editor_mirror_link (rlink, p->l->room_nmemb, p->room, ABOVE, BELOW);
  register_link_undo (&undo, rlink, desc);
  destroy_array ((void **) &rlink, NULL);
}

void
h_room_tiles_links (struct pos *p, char *desc)
{
  register_h_room_mirror_tile_undo (&undo, p->room, NULL);
  struct room_linking *rlink =
    copy_array (p->l->rlink, p->l->room_nmemb, NULL, sizeof (*rlink));
  editor_mirror_link (rlink, p->l->room_nmemb, p->room, LEFT, RIGHT);
  register_link_undo (&undo, rlink, desc);
  destroy_array ((void **) &rlink, NULL);
}

void
h_room_tiles (struct pos *p, char *desc)
{
  register_h_room_mirror_tile_undo
    (&undo, p->room, "ROOM MIRROR TILES H.");
}

void
h_room_links (struct pos *p, char *desc)
{
  struct room_linking *rlink =
    copy_array (p->l->rlink, p->l->room_nmemb, NULL, sizeof (*rlink));
  editor_mirror_link (rlink, p->l->room_nmemb, p->room, LEFT, RIGHT);
  register_link_undo (&undo, rlink, desc);
  destroy_array ((void **) &rlink, NULL);
}


int
button_action_cb (Ihandle *ih)
{
  Ihandle *v_button = (void *) IupGetAttribute (ih, "_V_BUTTON");
  Ihandle *vh_button = (void *) IupGetAttribute (ih, "_VH_BUTTON");
  Ihandle *h_button = (void *) IupGetAttribute (ih, "_H_BUTTON");
  Ihandle *r_button = (void *) IupGetAttribute (ih, "_R_BUTTON");

  Ihandle *radio = (void *) IupGetAttribute (ih, "_RADIO");
  Ihandle *place_toggle = (void *) IupGetAttribute (ih, "_PLACE_TOGGLE");
  Ihandle *room_toggle = (void *) IupGetAttribute (ih, "_ROOM_TOGGLE");
  Ihandle *level_toggle = (void *) IupGetAttribute (ih, "_LEVEL_TOGGLE");
  Ihandle *tiles_toggle = (void *) IupGetAttribute (ih, "_TILES_TOGGLE");
  Ihandle *links_toggle = (void *) IupGetAttribute (ih, "_LINKS_TOGGLE");

  Ihandle *scope = (void *) IupGetAttribute (radio, "VALUE_HANDLE");

  bool v = ih == v_button || ih == vh_button;
  bool h = ih == h_button || ih == vh_button;
  bool r = ih == r_button;
  bool place = scope == place_toggle;
  bool room = scope == room_toggle;
  bool level = scope == level_toggle;
  bool tiles = IupGetInt (tiles_toggle, "VALUE");
  bool links = IupGetInt (links_toggle, "VALUE");

  struct pos *p = (void *) IupGetAttribute (ih, "_POS");
  struct pos p0;

  struct sel_ring *sr = (void *) IupGetAttribute (ih, "_SEL_RING");

  if (v && h) {
    if (place) {
      reflect_pos_h (p, &p0);
      reflect_pos_v (&p0, &p0);
      register_mirror_pos_undo (&undo, p, &p0, true, "MIRROR TILE H+V.");
    } else if (room) {
      if (tiles && links) {
        if (sel_ring_ss_c_nmemb (sr))
          apply_to_sel_ring
            (vh_room_tiles_links, sr, "ROOM MIRROR TILES+LINKS H+V. (SEL. RING)");
        else vh_room_tiles_links (p, "ROOM MIRROR TILES+LINKS H+V.");
      } else if (tiles) {
        if (sel_ring_ss_c_nmemb (sr))
          apply_to_sel_ring
            (vh_room_tiles, sr, "ROOM MIRROR TILES H+V. (SEL. RING)");
        else vh_room_tiles (p, "ROOM MIRROR TILES H+V.");
      } else if (links) {
        if (sel_ring_ss_c_nmemb (sr))
          apply_to_sel_ring
            (vh_room_links, sr, "ROOM MIRROR LINKS H+V. (SEL. RING)");
        else vh_room_links (p, "ROOM MIRROR LINKS H+V.");
      }
    } else if (level) {
      if (tiles && links) {
        for (int i = 1; i < p->l->room_nmemb; i++) {
          register_h_room_mirror_tile_undo (&undo, i, NULL);
          register_v_room_mirror_tile_undo (&undo, i, NULL);
          struct room_linking *rlink =
            copy_array (p->l->rlink, p->l->room_nmemb, NULL, sizeof (*rlink));
          mirror_link (rlink, p->l->room_nmemb, i, LEFT, RIGHT);
          mirror_link (rlink, p->l->room_nmemb, i, ABOVE, BELOW);
          register_link_undo (&undo, rlink, NULL);
          destroy_array ((void **) &rlink, NULL);
        }
        end_undo_set (&undo, "LEVEL MIRROR TILES+LINKS H+V.");
      } else if (tiles) {
        for (int i = 1; i < p->l->room_nmemb; i++) {
          register_h_room_mirror_tile_undo (&undo, i, NULL);
          register_v_room_mirror_tile_undo (&undo, i, NULL);
        }
        end_undo_set (&undo, "LEVEL MIRROR TILES H+V.");
      } else if (links) {
        for (int i = 1; i < p->l->room_nmemb; i++) {
          struct room_linking *rlink =
            copy_array (p->l->rlink, p->l->room_nmemb, NULL, sizeof (*rlink));
          mirror_link (rlink, p->l->room_nmemb, i, LEFT, RIGHT);
          mirror_link (rlink, p->l->room_nmemb, i, ABOVE, BELOW);
          register_link_undo (&undo, rlink, NULL);
          destroy_array ((void **) &rlink, NULL);
        }
        end_undo_set (&undo, "LEVEL MIRROR LINKS H+V.");
      }
    }
  } else if (v) {
    if (place) {
      reflect_pos_v (p, &p0);
      register_mirror_pos_undo (&undo, p, &p0, false, "MIRROR TILE V.");
    } else if (room) {
      if (tiles && links) {
        if (sel_ring_ss_c_nmemb (sr))
          apply_to_sel_ring
            (v_room_tiles_links, sr, "ROOM MIRROR TILES+LINKS V. (SEL. RING)");
        else v_room_tiles_links (p, "ROOM MIRROR TILES+LINKS V.");
      } else if (tiles) {
        if (sel_ring_ss_c_nmemb (sr))
          apply_to_sel_ring
            (v_room_tiles, sr, "ROOM MIRROR TILES V. (SEL. RING)");
        else v_room_tiles (p, "ROOM MIRROR TILES V.");
      } else if (links) {
        if (sel_ring_ss_c_nmemb (sr))
          apply_to_sel_ring
            (v_room_links, sr, "ROOM MIRROR LINKS V. (SEL. RING)");
        else v_room_links (p, "ROOM MIRROR LINKS V.");
      }
    } else if (level) {
      if (tiles && links) {
        for (int i = 1; i < p->l->room_nmemb; i++) {
          register_v_room_mirror_tile_undo (&undo, i, NULL);
          struct room_linking *rlink =
            copy_array (p->l->rlink, p->l->room_nmemb, NULL, sizeof (*rlink));
          mirror_link (rlink, p->l->room_nmemb, i, ABOVE, BELOW);
          register_link_undo (&undo, rlink, NULL);
          destroy_array ((void **) &rlink, NULL);
        }
        end_undo_set (&undo, "LEVEL MIRROR TILES+LINKS V.");
      } else if (tiles) {
        for (int i = 1; i < p->l->room_nmemb; i++)
          register_v_room_mirror_tile_undo (&undo, i, NULL);
        end_undo_set (&undo, "LEVEL MIRROR TILES V.");
      } else if (links) {
        for (int i = 1; i < p->l->room_nmemb; i++) {
          struct room_linking *rlink =
            copy_array (p->l->rlink, p->l->room_nmemb, NULL, sizeof (*rlink));
          mirror_link (rlink, p->l->room_nmemb, i, ABOVE, BELOW);
          register_link_undo (&undo, rlink, NULL);
          destroy_array ((void **) &rlink, NULL);
        }
        end_undo_set (&undo, "LEVEL MIRROR LINKS V.");
      }
    }
  } else if (h) {
    if (place) {
      reflect_pos_h (p, &p0);
      register_mirror_pos_undo (&undo, p, &p0, true, "MIRROR TILE H.");
    } else if (room) {
      if (tiles && links) {
        if (sel_ring_ss_c_nmemb (sr))
          apply_to_sel_ring
            (h_room_tiles_links, sr, "ROOM MIRROR TILES+LINKS H. (SEL. RING)");
        else h_room_tiles_links (p, "ROOM MIRROR TILES+LINKS H.");
      } else if (tiles) {
        if (sel_ring_ss_c_nmemb (sr))
          apply_to_sel_ring
            (h_room_tiles, sr, "ROOM MIRROR TILES H. (SEL. RING)");
        else h_room_tiles (p, "ROOM MIRROR TILES H.");
      } else if (links) {
        if (sel_ring_ss_c_nmemb (sr))
          apply_to_sel_ring
            (h_room_links, sr, "ROOM MIRROR LINKS H. (SEL. RING)");
        else h_room_links (p, "ROOM MIRROR LINKS H.");
      }
    } else if (level) {
      if (tiles && links) {
        for (int i = 1; i < p->l->room_nmemb; i++) {
          register_h_room_mirror_tile_undo (&undo, i, NULL);
          struct room_linking *rlink =
            copy_array (p->l->rlink, p->l->room_nmemb, NULL, sizeof (*rlink));
          mirror_link (rlink, p->l->room_nmemb, i, LEFT, RIGHT);
          register_link_undo (&undo, rlink, NULL);
          destroy_array ((void **) &rlink, NULL);
        }
        end_undo_set (&undo, "LEVEL MIRROR TILES+LINKS H.");
      } else if (tiles) {
        for (int i = 1; i < p->l->room_nmemb; i++)
          register_h_room_mirror_tile_undo (&undo, i, NULL);
        end_undo_set (&undo, "LEVEL MIRROR TILES H.");
      } else if (links) {
        for (int i = 1; i < p->l->room_nmemb; i++) {
          struct room_linking *rlink =
            copy_array (p->l->rlink, p->l->room_nmemb, NULL, sizeof (*rlink));
          mirror_link (rlink, p->l->room_nmemb, i, LEFT, RIGHT);
          register_link_undo (&undo, rlink, NULL);
          destroy_array ((void **) &rlink, NULL);
        }
        end_undo_set (&undo, "LEVEL MIRROR LINKS H.");
      }
    }
  } else if (r) {
    if (place) {
      random_pos (p->l, &p0);
      p0.room = p->room;
      register_mirror_pos_undo (&undo, p, &p0, false, "MIRROR TILE R.");
    } else if (room) {
      if (tiles && links) {
        register_random_room_mirror_tile_undo
          (&undo, p->room, false, NULL);
        struct room_linking *rlink =
          copy_array (p->l->rlink, p->l->room_nmemb, NULL, sizeof (*rlink));
        editor_mirror_link (rlink, p->l->room_nmemb, p->room, random_dir (),
                            random_dir ());
        register_link_undo (&undo, rlink, "ROOM MIRROR TILES+LINKS R.");
        destroy_array ((void **) &rlink, NULL);
      } else if (tiles)
        register_random_room_mirror_tile_undo
          (&undo, p->room, false, "ROOM MIRROR TILES R.");
      else if (links) {
        struct room_linking *rlink =
          copy_array (p->l->rlink, p->l->room_nmemb, NULL, sizeof (*rlink));
        editor_mirror_link (rlink, p->l->room_nmemb, p->room, random_dir (),
                            random_dir ());
        register_link_undo (&undo, rlink, "ROOM MIRROR LINKS R.");
        destroy_array ((void **) &rlink, NULL);
      }
    } else if (level) {
      if (tiles && links) {
        for (int i = 1; i < p->l->room_nmemb; i++) {
          register_random_room_mirror_tile_undo
            (&undo, i, false, NULL);
          struct room_linking *rlink =
            copy_array (p->l->rlink, p->l->room_nmemb, NULL, sizeof (*rlink));
          mirror_link (rlink, p->l->room_nmemb, i, random_dir (),
                       random_dir ());
          register_link_undo (&undo, rlink, NULL);
          destroy_array ((void **) &rlink, NULL);
        }
        end_undo_set (&undo, "LEVEL MIRROR TILES+LINKS R.");
      } else if (tiles) {
        for (int i = 1; i < p->l->room_nmemb; i++)
          register_random_room_mirror_tile_undo
            (&undo, i, false, NULL);
        end_undo_set (&undo, "LEVEL MIRROR TILES R.");
      } else if (links) {
        for (int i = 1; i < p->l->room_nmemb; i++) {
          struct room_linking *rlink =
            copy_array (p->l->rlink, p->l->room_nmemb, NULL, sizeof (*rlink));
          mirror_link (rlink, p->l->room_nmemb, i, random_dir (),
                       random_dir ());
          register_link_undo (&undo, rlink, NULL);
          destroy_array ((void **) &rlink, NULL);
        }
        end_undo_set (&undo, "LEVEL MIRROR LINKS R.");
      }
    }
  }

  return IUP_DEFAULT;
}
