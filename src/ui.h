/*
  ui.h -- user interface module;

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

#ifndef MININIM_UI_H
#define MININIM_UI_H

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
#define CARD_ICON "data/icons/card.png"
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

enum main_menu {
  BASE_MID = 0,

  /* real menu itens follow */
  GAME_MID,

  LOAD_MID,
  LOAD_GAME_MID,
  LOAD_CONFIG_MID,
  LOAD_LEVEL_FILE_MID,

  SAVE_MID,
  SAVE_GAME_MID,
  SAVE_CONFIG_MID,
  SAVE_LEVEL_FILE_MID,

  RESTART_GAME_MID,
  START_GAME_MID,
  QUIT_GAME_MID,

  REPLAY_MID,
  PLAY_REPLAY_MID,
  PREVIOUS_REPLAY_MID,
  NEXT_REPLAY_MID,
  RECORD_REPLAY_MID,
  TOGGLE_PAUSE_GAME_MID,
  NEXT_FRAME_MID,
  INCREASE_TIME_FREQUENCY_MID,
  DECREASE_TIME_FREQUENCY_MID,
  TOGGLE_TIME_FREQUENCY_CONSTRAINT_MID,

  VIEW_MID,
  HUE_MODE_MID,
  HUE_ORIGINAL_MID,
  HUE_NONE_MID,
  HUE_GREEN_MID,
  HUE_GRAY_MID,
  HUE_YELLOW_MID,
  HUE_BLUE_MID,
  ENVIRONMENT_MODE_MID,
  ORIGINAL_ENV_MID,
  DUNGEON_MID,
  PALACE_MID,
  VIDEO_MODE_MID,
  VGA_MID,
  EGA_MID,
  CGA_MID,
  HGC_MID,
  FULL_SCREEN_MID,
  FLIP_SCREEN_MID,
  FLIP_SCREEN_VERTICAL_MID,
  FLIP_SCREEN_HORIZONTAL_MID,
  SCREENSHOT_MID,

  HELP_MID,
  ABOUT_MID,

  /* Used for pure caption items  */
  NO_MID,
};

ALLEGRO_BITMAP *load_icon (char *filename);
ALLEGRO_BITMAP *micon (ALLEGRO_BITMAP *bitmap);
ALLEGRO_BITMAP *micon_flags (ALLEGRO_BITMAP *bitmap, int flags);
void load_icons (void);
void unload_icons (void);

void create_main_menu (void);
void game_menu (void);
void load_menu (void);
void save_menu (void);

void view_menu (void);
ALLEGRO_BITMAP *hue_icon (enum hue hue);
void hue_mode_menu (void);
ALLEGRO_BITMAP *em_icon (enum em em);
void environment_mode_menu (void);
ALLEGRO_BITMAP *vm_icon (enum vm vm);
void video_mode_menu (void);
void screen_flip_menu (void);
void replay_menu (void);
void help_menu (void);

void show_menu (void);
void hide_menu (void);
bool is_showing_menu (void);
void toggle_menu_visibility (void);
void enable_menu (bool enable);

void menu_mid (intptr_t mid);
void anim_key_bindings (void);

void ui_editor (void);

void ui_load_game (void);
void ui_load_config (void);
void ui_restart_game (void);
void ui_start_game (void);
void ui_quit_game (void);

void ui_full_screen (void);
void ui_hue_mode (enum hue new_hue);
void ui_environment_mode (enum em new_em);
void ui_video_mode (enum vm new_vm);
void ui_flip_screen (int flags, bool correct_mouse);
void ui_screenshot (void);

void ui_play_replay (void);
void ui_previous_replay (void);
void ui_next_replay (void);
void ui_record_replay (void);
void ui_decrease_time_frequency (void);
void ui_increase_time_frequency (void);
void ui_toggle_time_frequency_constraint (void);
void ui_change_anim_freq (int f);
void ui_toggle_pause_game (void);
void ui_next_frame (void);
void print_game_paused (int priority);
void ui_version (void);

error_t ui_save_setting (char *key, char *value);

#endif	/* MININIM_UI_H */
