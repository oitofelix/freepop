/*
  constants.h -- constant definitions;

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

#ifndef MININIM_CONSTANTS_H
#define MININIM_CONSTANTS_H

#define WINDOW_TITLE "MININIM"
#define LOGO_ICON "data/icons/mininim.png"
#define SMALL_LOGO_ICON "data/icons/mininim-small.png"
#define OITOFELIX_FACE "data/icons/oitofelix-face.png"
#define JOYSTICK_BIG_ICON "data/icons/joystick-big.png"

#define INVALID (INT_MIN)

#define APPEND_MENU_MAX_DEPTH 10
#define ENV_OPTION_PREFIX "MININIM_"
#define MENU_CAPTION_MAX_CHARS 30

#define GUARD_STYLES_NMEMB 8
#define FELLOW_SHADOW_NMEMB 9
#define FELLOW_SHADOW_DEATH_WAIT_CYCLES 12
#define IS_ON_BACK_THRESHOLD 24

#define VOLUME_RANGE_MIN 0.0
#define VOLUME_RANGE_LOW 0.3
#define VOLUME_RANGE_MEDIUM 0.6
#define VOLUME_RANGE_MAX 1.0

#define VOLUME_OFF 0.0
#define VOLUME_LOW 0.2
#define VOLUME_MEDIUM 0.5
#define VOLUME_HIGH 1.0

#define CLIPPING_RECTANGLE_STACK_NMEMB_MAX 10
#define DRAWN_RECTANGLE_STACK_NMEMB_MAX 10

#define MIGNORE (INT_MIN)

#define FLOORS 3
#define PLACES 10

#define LEVENTS 256
#define LROOMS 24
#define LGUARDS LROOMS
#define LFLOORS 3
#define LPLACES 10

#define INITIAL_PLACE_WIDTH 32
#define INITIAL_PLACE_HEIGHT 63

#define PLACE_WIDTH 32
#define PLACE_HEIGHT 63

#define CUTSCENE_WIDTH 320
#define CUTSCENE_HEIGHT 200

#define ORIGINAL_WIDTH (PLACE_WIDTH * PLACES)
#define ORIGINAL_HEIGHT (PLACE_HEIGHT * FLOORS + BOTTOM_ROW_HEIGHT)
#define BOTTOM_ROW_HEIGHT 11
#define DISPLAY_WIDTH (ORIGINAL_WIDTH * 2)
#define DISPLAY_HEIGHT (ORIGINAL_HEIGHT * 2)

#define ROOM_HEIGHT (PLACE_HEIGHT * FLOORS)

#define LOOSE_FLOOR_STEPS 2
#define LOOSE_FLOOR_FASES 1
#define LOOSE_FLOOR_TOTAL (LOOSE_FLOOR_STEPS * LOOSE_FLOOR_FASES)

#define SPIKES_STEPS 10
#define SPIKES_FASES 1
#define SPIKES_TOTAL (SPIKES_STEPS * SPIKES_FASES)

#define DOOR_STEPS 48
#define DOOR_FASES 1
#define DOOR_TOTAL (DOOR_STEPS * DOOR_FASES)

#define LEVEL_DOOR_STEPS 44
#define LEVEL_DOOR_FASES 2
#define LEVEL_DOOR_TOTAL (LEVEL_DOOR_STEPS * LEVEL_DOOR_FASES)
#define LEVEL_DOOR_OPEN_MAX_STEP 9

#define CHOMPER_STEPS 6
#define CHOMPER_FASES 2
#define CHOMPER_TOTAL (CHOMPER_STEPS * CHOMPER_FASES)

#define KID_INITIAL_TOTAL_HP 3
#define KID_INITIAL_CURRENT_HP 3
#define KID_MINIMUM_HP_TO_BLINK 1
#define GUARD_MINIMUM_HP_TO_BLINK 1
#define BOTTOM_TEXT_DURATION (SEC2CYC (3))
#define BOTTOM_TEXT_MAX_LENGTH (CUTSCENE_WIDTH / 8)

#define TIME_LIMIT (3600 * DEFAULT_HZ)
#define START_TIME 0

#define INITIAL_KCA -1
#define INITIAL_KCD -1

#define FLOAT_TIMER_MAX 192
#define REFLOAT_MENU_THRESHOLD (FLOAT_TIMER_MAX / 2)

#define MAX_HP 10

#define SEC2CYC(x) (anim_freq > 0 ? (x) * anim_freq : (x) * DEFAULT_HZ)
#define CYC2SEC(x) (anim_freq > 0 ? (x) / anim_freq : (x) / DEFAULT_HZ)
#define SEC2EFF(x) ((x) * EFFECT_HZ)
#define DEFAULT_HZ 12
#define EFFECT_HZ 30
#define UNLIMITED_HZ 10000
#define REPLAY_STUCK_THRESHOLD 204

#define MENU_PERIOD 0.1
#define IUP_PERIOD 0.05

#define SELECT_CYCLES (SEC2CYC (3))

#define WALL_FG_WIDTH 12
#define DOOR_FG_WIDTH 14
#define CARPET_FG_WIDTH 12
#define CARPET_DESIGNS 6

#define CHOMPER_WAIT 10
#define CHOMPER_FG_WIDTH 14

#define DOOR_WAIT (5 * DEFAULT_HZ)
#define DOOR_CLIMB_LIMIT 40
#define DOOR_WAIT_LOOK 4

#define INVERSION_RANGE 22
#define HIT_RANGE 50
#define ATTACK_RANGE (HIT_RANGE + 10)
#define FIGHT_RANGE (ATTACK_RANGE + 10)
#define FOLLOW_RANGE (2 * ORIGINAL_WIDTH)

#define FIRE_RANDOM_SEED_0 234423479
#define FIRE_RANDOM_SEED_1 501477214

#define LOOSE_FLOOR_RESISTENCE 0

#define SPIKES_WAIT (1 * DEFAULT_HZ)

#define STARS_RANDOM_SEED 78234782
#define STARS 7

#define ESCAPE_KEY 27
#define BACKSPACE_KEY 8

#define ENEMY_REFRACTION_TIME 36

#define RESERVED_AUDIO_SAMPLES 16

#define DOOR_GRID_TIP_THRESHOLD 8

#define SHOCKWAVE_RADIUS PLACES

#define NO_FAKE -1
#define FULL_WIDTH -1

#define OPTIMIZE_CHANGED_POS_THRESHOLD ((1 * FLOORS * PLACES) / 3)

#define COLLISION_FRONT_LEFT_NORMAL -4
#define COLLISION_FRONT_RIGHT_NORMAL +5
#define COLLISION_BACK_LEFT_NORMAL +11
#define COLLISION_BACK_RIGHT_NORMAL +2

#define COLLISION_FRONT_LEFT_FIGHT -4
#define COLLISION_FRONT_RIGHT_FIGHT 0
#define COLLISION_BACK_LEFT_FIGHT 0
#define COLLISION_BACK_RIGHT_FIGHT -4

#define EDITOR_CYCLES_NONE 0
#define EDITOR_CYCLES_0 1
#define EDITOR_CYCLES_1 12
#define EDITOR_CYCLES_2 18
#define EDITOR_CYCLES_3 24

#define REPLAY_FILE_SIGNATURE "MININIM REPLAY"
#define REPLAY_FILE_FORMAT_VERSION 1

#define PACKED_GAMEPAD_STATE_UP_BIT (1 << 0)
#define PACKED_GAMEPAD_STATE_DOWN_BIT (1 << 1)
#define PACKED_GAMEPAD_STATE_LEFT_BIT (1 << 2)
#define PACKED_GAMEPAD_STATE_RIGHT_BIT (1 << 3)
#define PACKED_GAMEPAD_STATE_SHIFT_BIT (1 << 4)
#define PACKED_GAMEPAD_STATE_ENTER_BIT (1 << 5)
#define PACKED_GAMEPAD_STATE_CTRL_BIT (1 << 6)
#define PACKED_GAMEPAD_STATE_ALT_BIT (1 << 7)

#define PACKED_CONFIG_MIRROR_LEVEL_BIT (1 << 0)
#define PACKED_CONFIG_IMMORTAL_MODE_BIT (1 << 1)

#define HLINE_STR "===============================================================================\n"
#define HLINE printf (HLINE_STR); fflush (stdout);

#define JOYSTICK_MAX_HAPTIC_EFFECTS 10

#define KID_HAPTIC_STEP 1.0
#define KID_HAPTIC_SWORD 3.0
#define KID_HAPTIC_COLLISION 2.0
#define KID_HAPTIC_STRONG_COLLISION 3.0
#define KID_HAPTIC_HARM 6.0
#define KID_HAPTIC_DEATH 10.0
#define KID_HAPTIC_HANG 3.0
#define KID_HAPTIC_RAISE_SWORD 12.0
#define KID_HAPTIC_DRINK 6.0
#define KID_HAPTIC_STRONG_DRINK 12.0
#define KID_HAPTIC_SUCCESS 12.0
#define KID_HAPTIC_CHOMPER 5.0
#define KID_HAPTIC_SPIKES 5.0
#define KID_HAPTIC_LOOSE_FLOOR_BREAKING 5.0
#define KID_HAPTIC_PRESS_ANY_KEY 3.0
#define KID_HAPTIC_CROSS_MIRROR 6.0
#define KID_HAPTIC_LEGACY_COUCHING_START 6.0
#define KID_HAPTIC_LEGACY_MIRROR_APPEAR 6.0
#define KID_HAPTIC_SCREAM 12.0

#endif	/* MININIM_CONSTANTS_H */
