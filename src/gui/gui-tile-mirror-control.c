/*
  gui-tile-mirror-control.c -- editor GUI tile mirror control module;

  Copyright (C) 2015, 2016, 2017 Bruno Félix Rezende Ribeiro
  <oitofelix@gnu.org>

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


Ihandle *
gui_create_tile_mirror_control (struct pos *p, char *norm_group)
{
  Ihandle *ih, *vbox, *mirror_radio, *tiles_toggle, *links_toggle,
    *mirror_place_toggle, *mirror_room_toggle, *mirror_level_toggle;

  Ihandle *mirror_label, *mirror_v_button, *mirror_vh_button,
    *mirror_h_button, *mirror_r_button;

  ih = IupSetCallbacks
    (IupSetAttributes
     (IupFrame
      (vbox = IupSetAttributes
       (IupVbox
        (IupFill (),

         IupSetAttributes
         (IupGridBox
          (mirror_label = IupLabel (NULL),
           mirror_v_button = IupSetAttributes
           (IupButton (NULL, NULL),
            "TIP = \"Mirror tile vertically\""),
           mirror_vh_button = IupSetAttributes
           (IupButton (NULL, NULL),
            "TIP = \"Mirror tile in both directions\""),
           mirror_h_button = IupSetAttributes
           (IupButton (NULL, NULL),
            "TIP = \"Mirror tile horizontally\""),
           mirror_r_button = IupSetAttributes
           (IupButton (NULL, NULL),
            "TIP = \"Mirror tile randomly\""),
           NULL),
          "ORIENTATION = HORIZONTAL,"
          "NUMDIV = 5,"
          "SIZECOL = -1,"
          "SIZELIN = -1,"
          "NORMALIZESIZE = BOTH,"),

         mirror_radio = IupRadio
         (IupHbox
          (IupSetCallbacks
           (IupSetAttributes
            (mirror_place_toggle = IupToggle ("P", NULL),
             "TIP = \"Place mirroring scope\""),
            "ACTION", (Icallback) toggle_action_cb,
            NULL),
           IupSetCallbacks
           (IupSetAttributes
            (mirror_room_toggle = IupToggle ("R", NULL),
             "TIP = \"Room mirroring scope\""),
            "ACTION", (Icallback) toggle_action_cb,
            NULL),
           IupSetCallbacks
           (IupSetAttributes
            (mirror_level_toggle = IupToggle ("L", NULL),
             "TIP = \"Level mirroring scope\""),
            "ACTION", (Icallback) toggle_action_cb,
            NULL),
           NULL)),

         IupHbox
         (tiles_toggle = IupSetCallbacks
          (IupSetAttributes
           (IupToggle ("Tiles", NULL),
            "VALUE = YES,"
            "ACTIVE = NO,"
            "TIP = \"Mirror tiles\""),
           "ACTION", (Icallback) toggle_action_cb,
           NULL),
          links_toggle = IupSetCallbacks
          (IupSetAttributes
           (IupToggle ("Links", NULL),
            "VALUE = YES,"
            "ACTIVE = NO,"
            "TIP = \"Mirror links\""),
           "ACTION", (Icallback) toggle_action_cb,
           NULL),
          NULL),

         IupFill (),

         NULL),
        "ALIGNMENT = ACENTER")),
      "TITLE = Mirror"),
     "_UPDATE_CB", _update_cb,
     NULL);

  ALLEGRO_BITMAP *mirror_icon = load_memory_bitmap (MIRROR_ICON);
  gui_set_image (mirror_label, mirror_icon, NULL);
  al_destroy_bitmap (mirror_icon);

  ALLEGRO_BITMAP *v_icon = load_memory_bitmap (VERTICAL_ICON);
  gui_set_image (mirror_v_button, v_icon, NULL);
  al_destroy_bitmap (v_icon);

  ALLEGRO_BITMAP *vh_icon = load_memory_bitmap (VERTICAL_HORIZONTAL_ICON);
  gui_set_image (mirror_vh_button, vh_icon, NULL);
  al_destroy_bitmap (vh_icon);

  ALLEGRO_BITMAP *h_icon = load_memory_bitmap (HORIZONTAL_ICON);
  gui_set_image (mirror_h_button, h_icon, NULL);
  al_destroy_bitmap (h_icon);

  ALLEGRO_BITMAP *shuffle_icon = load_memory_bitmap (SHUFFLE_ICON);
  gui_set_image (mirror_r_button, shuffle_icon, NULL);
  al_destroy_bitmap (shuffle_icon);

  IupSetAttribute (vbox, "NORMALIZERGROUP", norm_group);

  IupSetAttribute (ih, "_MIRROR_RADIO", (void *) mirror_radio);
  IupSetAttribute (ih, "_MIRROR_PLACE_TOGGLE", (void *) mirror_place_toggle);
  IupSetAttribute (ih, "_MIRROR_ROOM_TOGGLE", (void *) mirror_room_toggle);
  IupSetAttribute (ih, "_MIRROR_LEVEL_TOGGLE", (void *) mirror_level_toggle);
  IupSetAttribute (ih, "_TILES_TOGGLE", (void *) tiles_toggle);
  IupSetAttribute (ih, "_LINKS_TOGGLE", (void *) links_toggle);
  IupSetAttribute (ih, "_POS", (void *) p);

  return ih;
}

int
_update_cb (Ihandle *ih)
{
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

  struct pos *p = (void *) IupGetAttribute (ih, "_POS");
  gui_control_active (ih, is_valid_pos (p));

  return IUP_DEFAULT;
}

int
toggle_action_cb (Ihandle *ih, int state)
{
  Ihandle *tiles_toggle = (void *) IupGetAttribute (ih, "_TILES_TOGGLE");
  Ihandle *links_toggle = (void *) IupGetAttribute (ih, "_LINKS_TOGGLE");
  Ihandle *mirror_place_toggle =
    (void *) IupGetAttribute (ih, "_MIRROR_PLACE_TOGGLE");

  if (ih == tiles_toggle && ! state)
    IupSetInt (links_toggle, "VALUE", true);
  else if (ih == links_toggle && ! state)
    IupSetInt (tiles_toggle, "VALUE", true);
  else {
    gui_control_active (tiles_toggle, ih != mirror_place_toggle);
    gui_control_active (links_toggle, ih != mirror_place_toggle);
  }

  return IUP_DEFAULT;
}
