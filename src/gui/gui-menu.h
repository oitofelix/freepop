/*
  gui-menu.h -- graphical user interface menu module;

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

#ifndef MININIM_GUI_MENU_H
#define MININIM_GUI_MENU_H

#define OPEN_ICON "data/icons/open.png"
#define SAVE_ICON "data/icons/save.png"
#define RELOAD_ICON "data/icons/reload.png"
#define QUIT_ICON "data/icons/quit.png"
#define FULL_SCREEN_ICON "data/icons/full-screen.png"
#define WINDOWS_ICON "data/icons/windows.png"
#define CAMERA_ICON "data/icons/camera.png"
#define PLAY_ICON "data/icons/play.png"
#define RECORD_ICON "data/icons/record.png"
#define STOP_ICON "data/icons/stop.png"
#define EJECT_ICON "data/icons/eject.png"
#define BACKWARD_ICON "data/icons/backward.png"
#define FORWARD_ICON "data/icons/forward.png"
#define PAUSE_ICON "data/icons/pause.png"
#define PREVIOUS_ICON "data/icons/previous.png"
#define NEXT_ICON "data/icons/next.png"
#define SCREEN_ICON "data/icons/screen.png"
#define RIGHT_ICON "data/icons/right.png"
#define DUNGEON_ICON "data/icons/dungeon.png"
#define PALACE_ICON "data/icons/palace.png"
#define GREEN_ICON "data/icons/green.png"
#define GRAY_ICON "data/icons/gray.png"
#define YELLOW_ICON "data/icons/yellow.png"
#define BLUE_ICON "data/icons/blue.png"
#define BLACK_ICON "data/icons/black.png"
#define VGA_ICON "data/icons/vga.png"
#define EGA_ICON "data/icons/ega.png"
#define CGA_ICON "data/icons/cga.png"
#define HGC_ICON "data/icons/hgc.png"
#define VERTICAL_ICON "data/icons/vertical.png"
#define HORIZONTAL_ICON "data/icons/horizontal.png"
#define KEYBOARD_ICON "data/icons/keyboard.png"
#define JOYSTICK_ICON "data/icons/joystick.png"
#define CANCEL_ICON "data/icons/cancel.png"
#define CLOCK_ICON "data/icons/clock.png"
#define EDIT_ICON "data/icons/edit.png"
#define JOYSTICK2_ICON "data/icons/joystick2.png"
#define UNDO_ICON "data/icons/undo.png"
#define REDO_ICON "data/icons/redo.png"
#define SCREENSAVER_ICON "data/icons/screensaver.png"
#define JOYSTICK3_ICON "data/icons/joystick3.png"
#define VOLUME_OFF_ICON "data/icons/volume-off.png"
#define VOLUME_LOW_ICON "data/icons/volume-low.png"
#define VOLUME_MEDIUM_ICON "data/icons/volume-medium.png"
#define VOLUME_HIGH_ICON "data/icons/volume-high.png"
#define GAME_ICON "data/icons/game.png"
#define SETTINGS_ICON "data/icons/settings.png"
#define ZOOM_NONE_ICON "data/icons/zoom-none.png"
#define ZOOM_STRETCH_ICON "data/icons/zoom-stretch.png"
#define ZOOM_RATIO_ICON "data/icons/zoom-ratio.png"
#define VERTICAL_HORIZONTAL_ICON "data/icons/vertical-horizontal.png"
#define ZOOM_OUT_ICON "data/icons/zoom-out.png"
#define ZOOM_IN_ICON "data/icons/zoom-in.png"
#define HEADING_ICON "data/icons/heading.png"
#define ZOOM_ICON "data/icons/zoom.png"
#define NAVIGATION_ICON "data/icons/navigation.png"
#define NAV_SELECT_ICON "data/icons/nav-select.png"
#define NAV_ROW_ICON "data/icons/nav-row.png"
#define NAV_PAGE_ICON "data/icons/nav-page.png"
#define NAV_LEFT_ICON "data/icons/nav-left.png"
#define NAV_RIGHT_ICON "data/icons/nav-right.png"
#define NAV_ABOVE_ICON "data/icons/nav-above.png"
#define NAV_BELOW_ICON "data/icons/nav-below.png"
#define NAV_HOME_ICON "data/icons/nav-home.png"
#define REPEAT_ICON "data/icons/repeat.png"
#define COMPASS_ICON "data/icons/compass.png"
#define COMPASS2_ICON "data/icons/compass2.png"
#define DRAWING_ICON "data/icons/drawing.png"
#define FIRST_ICON "data/icons/first.png"
#define LAST_ICON "data/icons/last.png"
#define JUMP_ICON "data/icons/jump.png"
#define ORIGINAL_ICON "data/icons/original.png"
#define GUARD_ICON "data/icons/guard.png"
#define FAT_ICON "data/icons/fat.png"
#define VIZIER_ICON "data/icons/vizier.png"
#define SKELETON_ICON "data/icons/skeleton.png"
#define SHADOW_ICON "data/icons/shadow.png"
#define RESURRECT_ICON "data/icons/resurrect.png"
#define DEATH_ICON "data/icons/death.png"
#define FEATHER_ICON "data/icons/feather.png"
#define ANGEL_ICON "data/icons/angel.png"
#define HP_EMPTY_ICON "data/icons/hp-empty.png"
#define HP_FULL_ICON "data/icons/hp-full.png"
#define SKILLS_ICON "data/icons/skills.png"
#define TIME_ICON "data/icons/time.png"
#define TIME_ADD_ICON "data/icons/time-add.png"
#define TIME_SUB_ICON "data/icons/time-sub.png"
#define ATTACK_ICON "data/icons/attack.png"
#define ATTACK_ADD_ICON "data/icons/attack-add.png"
#define ATTACK_SUB_ICON "data/icons/attack-sub.png"
#define DEFENSE_ICON "data/icons/defense.png"
#define DEFENSE_ADD_ICON "data/icons/defense-add.png"
#define DEFENSE_SUB_ICON "data/icons/defense-sub.png"
#define COUNTER_ATTACK_ICON "data/icons/counter-attack.png"
#define COUNTER_ATTACK_ADD_ICON "data/icons/counter-attack-add.png"
#define COUNTER_ATTACK_SUB_ICON "data/icons/counter-attack-sub.png"
#define COUNTER_DEFENSE_ICON "data/icons/counter-defense.png"
#define COUNTER_DEFENSE_ADD_ICON "data/icons/counter-defense-add.png"
#define COUNTER_DEFENSE_SUB_ICON "data/icons/counter-defense-sub.png"
#define SHADOW_FACE_ICON "data/icons/shadow-face.png"
#define HEART_ICON "data/icons/heart.png"
#define PLUS_ICON "data/icons/plus.png"
#define MINUS_ICON "data/icons/minus.png"
#define LOCK_ICON "data/icons/lock.png"
#define UNLOCK_ICON "data/icons/unlock.png"

#define JUMP_TO_LEVEL_MID_NMEMB 21
#define REPLAY_FAVORITE_MID_NMEMB 10

#define MENU_ID_MIN 1
#define MENU_ID_MAX ((1 << 16) - 1)
#define MAIN_MENU_ID_MIN MENU_ID_MIN
#define MAIN_MENU_ID_MAX (MENU_ID_MAX / 2)
#define AUX_MENU_ID_MIN (MAIN_MENU_ID_MAX + 1)
#define AUX_MENU_ID_MAX MENU_ID_MAX

/* functions */
void load_icons (void);
void unload_icons (void);

ALLEGRO_EVENT_SOURCE *main_menu_event_source (void);
void main_menu (void);

ALLEGRO_EVENT_SOURCE *aux_menu_event_source (void);
void show_aux_menu (void);
void aux_menu (void);

void dispatch_menu_event (ALLEGRO_EVENT *event);

void show_menu (void);
void hide_menu (void);
bool is_showing_menu (void);
void toggle_menu_visibility (void);

bool main_menu_enabled;

#endif	/* MININIM_GUI_MENU_H */