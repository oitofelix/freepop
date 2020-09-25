/*
  mininim.c -- MININIM main module;

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

int exit_code = EXIT_SUCCESS;

enum level_module level_module;

struct config_info {
  enum {
    CI_CONFIGURATION_FILE, CI_ENVIRONMENT_VARIABLES,
    CI_COMMAND_LINE,
  } type;
  char *filename;
};

bool ignore_main_config, ignore_environment;
bool mirror_level;

static char **argv;
static size_t argc;
static char **eargv;
static size_t eargc;
static char **cargv;
static size_t cargc;

char *resources_dir,
  *temp_dir,
  *user_home_dir,
  *user_documents_dir,
  *user_data_dir,
  *user_settings_dir,
  *system_data_dir,
  *data_dir,
  *exe_filename,
  *config_filename,
  *history_filename,
  *levels_dat_filename = "data/dat-levels/LEVELS.DAT";

char *levels_dat_compat_filename;

ALLEGRO_THREAD *load_config_dialog_thread, *save_game_dialog_thread,
  *save_picture_dialog_thread, *message_box_thread_id;

struct dialog load_config_dialog = {
  .title = "Load game/configuration file",
  .patterns = "*.mcf;*.MCF;*.msv;*.MSV",
  .mode = ALLEGRO_FILECHOOSER_FILE_MUST_EXIST
  | ALLEGRO_FILECHOOSER_MULTIPLE,
};

struct dialog save_game_dialog = {
  .title = "Save game",
  .patterns = "*.msv;*.MSV",
  .mode = ALLEGRO_FILECHOOSER_SAVE,
};

struct dialog save_picture_dialog = {
  .title = "Save picture",
  .patterns = "*.png;*.PNG",
  .mode = ALLEGRO_FILECHOOSER_SAVE
  | ALLEGRO_FILECHOOSER_PICTURES,
};

struct message_box about_dialog = {
  .title = "About",
  .heading = "MININIM " VERSION,
  .text =
  "Copyright (C) 2015-2017 " PACKAGE_COPYRIGHT_HOLDER "\n\n"

  "MININIM is free software under GPLv3+.\n"
  "You are free to change and redistribute it.\n"
  "There is NO WARRANTY, to the extent permitted by law.\n\n"

  "Please, support MININIM development!\n"
  "http://oitofelix.github.io/funding.html",
  .buttons = NULL,
  .flags = 0,
};

uint64_t play_time;
bool play_time_stopped;
enum em em = DUNGEON;
enum gpm gpm = JOYSTICK;
bool force_em = false;
enum hue hue = HUE_NONE;
bool force_hue = false;
enum gm gm = ORIGINAL_GM;
char *audio_mode;
char *video_mode;
char *env_mode;
char *hue_mode;
bool immortal_mode;
int initial_total_hp = KID_INITIAL_TOTAL_HP, total_hp;
int initial_current_hp = KID_INITIAL_CURRENT_HP, current_hp;
int start_level = 1;
struct pos start_pos = {NULL, -1,-1,-1};
uint64_t time_limit = TIME_LIMIT;
uint64_t start_time = START_TIME;
uint64_t start_level_time;
enum semantics semantics;
enum movements movements;
bool title_demo;
enum rendering rendering = BOTH_RENDERING;
bool inhibit_screensaver = true;
int play_game_counter;
enum mr_fit_mode mr_fit_mode;

/* screams */
bool scream;
bool kid_scream;
bool guard_scream;
bool fat_scream;
bool shadow_scream;
bool skeleton_scream;
bool vizier_scream;
bool princess_scream;
bool mouse_scream;

struct skill skill = {.counter_attack_prob = INITIAL_KCA,
                      .counter_defense_prob = INITIAL_KCD};
static bool replay_info;
static bool skip_title;
static bool level_module_given;
static int start_replay_favorite = -1;
volatile static sig_atomic_t quitting_in_progress = 0;

static void quit_game (void);
static void quit_game_sighandler (int signum);
static error_t parser (int key, char *arg, struct argp_state *state);
static void print_paths (void);
static void print_display_modes (void);
static char *command_line2env_option_name (const char *option_name);
static char *env2command_line_option_name (const char *option_name);
static char *command_line2config_option_name (const char *option_name);
static char *config2command_line_option_name (const char *option_name);
static void get_paths (void);
static bool is_valid_option (char *option);
static void config_str2key_value (const char *str, char **key, char **value);
static void get_env_args (size_t *eargc, char ***eargv,
                          struct argp_option *options);
static error_t get_config_args (size_t *cargc, char ***cargv,
                                struct argp_option *options,
                                char *filename, enum file_type *ret_file_type);
static error_t
pre_parser (int key, char *arg, struct argp_state *state);
static void give_dat_compat_preference (void);

static struct argp_option options[] = {
  /* Configuration */
  {NULL, 0, NULL, 0, "Configuration:", 0},

  {"load-config", LOAD_CONFIG_OPTION, "FILE", 0, "Load configuration file FILE.  The options set in FILE have the same precedence as the equivalent command line options given at its place of occurrence.  This can be done in-game using the CTRL+L key binding.", 0},

  {"load-game", LOAD_CONFIG_OPTION, NULL, OPTION_ALIAS, NULL, 0},

  {"ignore-main-config", IGNORE_MAIN_CONFIG_OPTION, NULL, 0, "Ignore main configuration file.  The default is to parse it at the very beginning of each run.", 0},

  {"ignore-environment", IGNORE_ENVIRONMENT_OPTION, NULL, 0, "Ignore environment variables.  The default is to parse them after the main configuration file.", 0},

  {NULL, 0, NULL, OPTION_DOC, "There are three methods of configuration: command line options, environment variables and configuration files.  For every command line option of the form 'x-y' there is an equivalent environment variable option 'MININIM_X_Y' and an equivalent configuration file option 'x y'.  The multiple methods of configuration are cumulative but command line options override any other, while environment variables override the main configuration file.  In any method applicable later options override earlier ones in case their effects are totally or partially conflicting.  The option '--print-paths' shows, among other things, the expected file name of the main configuration file.", 0},

  {NULL, 0, NULL, OPTION_DOC, "Notice that save files (CTRL+G) are a particular case of configuration files and should be loaded the same way.  The option '--load-game' is provided as a mnemonic alias.  When loaded in-game, configuration files take effect immediately.", 0},

  /* Level */
  {NULL, 0, NULL, 0, "Level:", 0},
  {"level-module", LEVEL_MODULE_OPTION, "LEVEL-MODULE", 0, "Select level module.  A level module determines a way to generate consecutive levels for use by the engine.  Valid values for LEVEL-MODULE are: NATIVE, LEGACY, PLV, DAT and CONSISTENCY.  NATIVE is the module designed to read the native format that supports all features.  LEGACY is the module designed to read the original PoP 1 raw level files.  PLV is the module designed to read the original PoP 1 PLV extended level files.  DAT is the module designed to read the original PoP 1 LEVELS.DAT file.  CONSISTENCY is the module designed to generate random-corrected levels for accessing the engine robustness.  The default is NATIVE.", 0},
  {"convert-levels", CONVERT_LEVELS_OPTION, NULL, OPTION_NO_USAGE, "Batch convert levels 0 to 15 accessible by the current level module to the native format and exit.  The levels are saved in the user data directory, where they take precedence over levels in every other location.  When using this option there is no point in using any other options besides '--level-module' and '--mirror-level', both of which must occur before this to take effect.  You can accomplish a similar result in-game on a per level basis by using the 'E>LS' command.  Notice that in that case any changes made to the level by special events (or otherwise) before you trigger the save command will be retained.", 0},
  {"start-level", START_LEVEL_OPTION, "N", 0, "Make the kid start at level N.  The default is 1.  Valid integers range from 0 to INT_MAX.  This can be changed in-game using the SHIFT+L and SHIFT+M key bindings.", 0},
  {"start-pos", START_POS_OPTION, "R,F,P", 0, "Make the kid start at room R, floor F and place P. The default is to let this decision to the level module.  R is an integer ranging from 1 to INT_MAX, F is an integer ranging from 0 to 2 and P is an integer ranging from 0 to 9.  This option has no effect on replays.", 0},
  {"mirror-level", MIRROR_LEVEL_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Enable/disable level mirroring.  This option causes every level to be fully mirrored (tiles+links) in the horizontal direction after they have been loaded by the active level module.  The default is FALSE.  You can accomplish a similar result in-game on a per level basis by using the 'E>LMBH' command.  See also the '--mirror-mode' option.", 0},
  {NULL, 0, NULL, OPTION_DOC, "If the option '--level-module' is not given and there is a LEVELS.DAT file in the working directory, the DAT level module is automatically used to load that file.  This is a compatibility measure for applications which depend upon this legacy behavior.", 0},

  /* Time */
  {NULL, 0, NULL, 0, "Time:", 0},
  {"time-limit", TIME_LIMIT_OPTION, "N", 0, "Set the time limit to complete the game to N cycles.  The default is 43200 (1 hour at 12 Hz).  Valid integers range from 1 to INT_MAX.  This can be changed in-game using the + and - key bindings.", 0},
  {"start-time", START_TIME_OPTION, "N", 0, "Set the play time counter to N cycles.  The default is 0.  Valid integers range from 0 to INT_MAX.", 0},
  {"time-frequency", TIME_FREQUENCY_OPTION, "N", 0, "Set nominal time frequency to N Hz in case N > 0, or disable time frequency constraint for N = 0.  The default is 12Hz.  Valid integers range from 0 to UNLIMITED_HZ (usually 10000).  Notice that if N is too large for the system to keep up with, timed events may present incorrect duration.  Use 0 for fastest verification of replay chains.  This can be changed in-game using the ( and ) key bindings.", 0},

  /* Skills */
  {NULL, 0, NULL, 0, "Skills:", 0},
  {"total-hp", TOTAL_HP_OPTION, "N", 0, "Make the kid start with N total HP.  The default is 3.  Valid integers range from 1 to INT_MAX.  This can be changed in-game using the SHIFT+T key binding.  Notice that only a maximum of 10 HP are displayed in the bottom line.", 0},
  {"kca", KCA_OPTION, "N", 0, "Set kid's counter attack skill to N.  The default is 0 (zero).  Valid integers range from 0 to 100.  This can be changed in-game using the CTRL+= and CTRL+- key bindings.", 0},
  {"kcd", KCD_OPTION, "N", 0, "Set kid's counter defense skill to N.  The default is 0 (zero).  Valid integers range from 0 to 100.  This can be changed in-game using the ALT+= and ALT+- key bindings.", 0},
  {"immortal-mode", IMMORTAL_MODE_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Enable/disable immortal mode.  In immortal mode the kid can't be harmed.  The default is FALSE.  This can be changed in-game using the I key binding.", 0},

  /* Rendering */
  {NULL, 0, NULL, 0, "Rendering:", 0},
  {"video-mode", VIDEO_MODE_OPTION, "VIDEO-MODE", 0, "Select video mode.  Valid values for VIDEO-MODE are: VGA, EGA, CGA and HGA.  The default is VGA.  This can be changed in-game using the F12 key binding.", 0},
  {"environment-mode", ENVIRONMENT_MODE_OPTION, "ENVIRONMENT-MODE", 0, "Select environment mode.  Valid values for ENVIRONMENT-MODE are: ORIGINAL, DUNGEON and PALACE.  The ORIGINAL value gives level modules autonomy in this choice for each particular level. This is the default.  This can be changed in-game using the F11 key binding.", 0},
  {"guard-mode", GUARD_MODE_OPTION, "GUARD-MODE", 0, "Select guard mode.  Valid values for GUARD-MODE are: ORIGINAL, GUARD, FAT, VIZIER, SKELETON and SHADOW.  The ORIGINAL value gives level modules autonomy in this choice for each particular guard.  This is the default.  This can be changed in-game using the SHIFT+F11 key binding.  This option has no effect on replays.", 0},
{"hue-mode", HUE_MODE_OPTION, "HUE-MODE", 0, "Select hue mode.  Valid values for HUE-MODE are: ORIGINAL, NONE, GREEN, GRAY, YELLOW and BLUE.  The ORIGINAL value gives level modules autonomy in this choice for each particular level.  This is the default.  For the classic behavior of the first version of the original game use NONE.  This can be changed in-game using the ALT+F11 key binding.", 0},
  {"display-flip-mode", DISPLAY_FLIP_MODE_OPTION, "DISPLAY-FLIP-MODE", 0, "Select display flip mode.  Valid values for DISPLAY-FLIP-MODE are: NONE, VERTICAL, HORIZONTAL and VERTICAL-HORIZONTAL.  The default is NONE.  This can be changed in-game using the SHIFT+I key binding.", 0},
  {"mirror-mode", MIRROR_MODE_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Enable/disable mirror mode.  In mirror mode the screen and the keyboard are flipped horizontally.  This is equivalent of specifying both the options --display-flip-mode=HORIZONTAL and --gamepad-flip-mode=HORIZONTAL.  The default is FALSE.  This can be changed in-game using the SHIFT+I and SHIFT+K key bindings for the display and keyboard, respectively.  See also the '--mirror-level' option.", 0},
  {"blind-mode", BLIND_MODE_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Enable/disable blind mode.  In blind mode background and non-animated sprites are not drawn. The default is FALSE.  This can be changed in-game using the SHIFT+B key binding.", 0},
  {"multi-room", MULTI_ROOM_OPTION, "WxH", 0, "Set multi-room width and height to W and H, respectively.  The default is 2x2.  The values W and H are strictly positive integers and must be separated by an 'x'.  This can be changed in-game using the [ (decrement width and height), ] (increment width and height), CTRL+[ (decrement width), CTRL+] (increment width), ALT+[ (decrement height) and ALT+] (increment heigth) key bindings.", 0},
  {"multi-room-fit-mode", MULTI_ROOM_FIT_MODE_OPTION, "MULTI-ROOM-FIT-MODE", 0, "Select multi-room fit mode.  Valid values for MULTI-ROOM-FIT-MODE are: NONE, STRETCH and RATIO.  The default is NONE.  This can be changed in-game using the M key binding.", 0},
  {"rendering", RENDERING_OPTION, "RENDERING-MODE", 0, "Select rendering mode.  Valid values for RENDERING-MODE are: BOTH, VIDEO, AUDIO and NONE.  The default is BOTH.  Notice that video rendering makes replays slower, thus consider using NONE for batch processing of replay chains.", 0},

  /* Gamepad */
  {NULL, 0, NULL, 0, "Gamepad:", 0},
  {"gamepad-mode", GAMEPAD_MODE_OPTION, "GAMEPAD-MODE", 0, "Select gamepad mode.  Valid values for GAMEPAD-MODE are: KEYBOARD and JOYSTICK.  The default is JOYSTICK.  If a joystick is not available, it falls back to KEYBOARD.  This can be changed in-game using the CTRL+K and CTRL+J key bindings.", 0},
  {"gamepad-flip-mode", GAMEPAD_FLIP_MODE_OPTION, "GAMEPAD-FLIP-MODE", 0, "Select gamepad flip mode.  Valid values for GAMEPAD-FLIP-MODE are: NONE, VERTICAL, HORIZONTAL and VERTICAL-HORIZONTAL.  The default is NONE.  This can be changed in-game using the SHIFT+K key binding.", 0},
  {"joystick-axis-threshold", JOYSTICK_AXIS_THRESHOLD_OPTION, "FUNC,VALUE", 0, "Set joystick threshold to VALUE for the axis mapped to FUNC.  Valid values for FUNC are H and V.   VALUE is a floating point ranging from 0.0 to 1.0.  The default VALUE for H is 0.1 and for Y is 0.8.", 0},
  {"joystick-button-threshold", JOYSTICK_BUTTON_THRESHOLD_OPTION, "FUNC,VALUE", 0, "Set joystick threshold to VALUE for the button mapped to FUNC.  Valid values for FUNC are: UP, RIGHT, DOWN, LEFT, ENTER, SHIFT, CTRL, ALT.  VALUE is an integer ranging from 0 to 32767.  The default VALUE for any function is 100.", 0},
  {"joystick-axis", JOYSTICK_AXIS_OPTION, "FUNC,STICK,AXIS", 0, "Map function FUNC to joystick axis STICK,AXIS.  Valid values for FUNC are: H and V.  STICK,AXIS is a valid stick and axis pair.  The default STICK,AXIS for H is 0,0 and for V is 0,1.", 0},
  {"joystick-button", JOYSTICK_BUTTON_OPTION, "FUNC,BUTTON", 0, "Map function FUNC to joystick button BUTTON.  Valid values for FUNC are: UP, RIGHT, DOWN, LEFT, ENTER, SHIFT, CTRL, ALT, TIME, PAUSE.  BUTTON is a valid joystick button number.  The default BUTTON values are 0, 1, 2, 3, 4, 5, 8 and 9, respectively.", 0},
  {"joystick-info", JOYSTICK_INFO_OPTION, NULL, OPTION_NO_USAGE, "Print information about the primary joystick and exit.  Keep sticks/buttons pressed while invoking it to find out their respective assigned numbers.", 0},
  {"gamepad-rumble-gain", GAMEPAD_RUMBLE_GAIN_OPTION, "F", 0, "Set gamepad haptic rumble gain to F.  This number is multiplied by the intensity of rumble effects in order to scale they down proportionally in case the default is too intense for the available gamepad.  It's implemented in software and thus the GAIN gamepad feature is not required.  The default is 1.0.  Valid floating values range from 0.0 (disables rumble) to 1.0 (default intensity).  This option has no effect on replays."
#if HAPTIC_FEATURE
   "\nWARNING: THIS BUILD DOES NOT SUPPORT HAPTICS."
#endif
   , 0},

  {NULL, 0, NULL, OPTION_DOC, "The primary joystick's axis and button numbers are listed by the option '--joystick-info'.  You can find out the number of a particular axis or button by pressing it before invoking MININIM with that option.  If a stick, axis or button given to an option doesn't exist in the primary joystick, it's silently ignored.  The joystick can be activated and auto-calibrated in-game by the CTRL+J key binding.  Use this when hot-plugging a joystick or in case the joystick starts to behave oddly.  If your joystick is peculiar enough, proving the auto-calibration mechanism insufficient, the '--joystick-axis-threshold' and '--joystick-button-threshold' options may help.", 0},

  /* Display */
  {NULL, 0, NULL, 0, "Display:", 0},
  {"display-mode", DISPLAY_MODE_OPTION, "M", 0, "Use display mode number M.  The default is -1 (desktop).  The valid integers list can be obtained using the option '--print-display-modes'. This can be changed in-game using the D key binding, in case a non-desktop display mode has been selected.", 0},
  {"print-display-modes", PRINT_DISPLAY_MODES_OPTION, NULL, OPTION_NO_USAGE, "Print display modes and exit.", 0},
  {"fullscreen", FULLSCREEN_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Enable/disable fullscreen mode for desktop display mode, but ignored for other display modes.  In fullscreen mode the window spans the entire screen.  The default is FALSE.  This can be changed in-game using the F key binding.", 0},
  {"window-position", WINDOW_POSITION_OPTION, "X,Y", 0, "Place the window at screen coordinates X,Y.  The default is to let this choice to the window manager.  The values X and Y are integers and must be separated by a comma.", 0},
  {"window-dimensions", WINDOW_DIMENSIONS_OPTION, "WxH", 0, "Set window width and height to W and H, respectively.  The default is 640x400.  The values W and H are strictly positive integers and must be separated by an 'x'.", 0},

  {NULL, 0, NULL, OPTION_DOC, "The desktop display mode (-1) uses the native desktop resolution and allows for windowed and fullscreen displays.  This is the default and most convenient setting in case the computer is fast enough.  Non-desktop display modes (>= 0) are all fullscreen and change the actual video resolution.  This may be useful for older computers in which the desktop display mode is prohibitively slow.  After the game has started it's not possible to alternate between desktop and non-desktop modes.  No window-related option takes effect for non-desktop display modes.  Changing non-desktop display modes in-game using the D key binding might cause video driver instability.", 0},

  /* replays */
  {NULL, 0, NULL, 0, "Replays", 0},
  {"record-replay", RECORD_REPLAY_OPTION, NULL, 0, "Starts recording replay countdown at game beginning.  Use this in conjunction with '--start-level' to start recording a given level.  This can be done in-game using the ALT+F7 key binding.", 0},
  {"replay-info", REPLAY_INFO_OPTION, NULL, OPTION_NO_USAGE, "Print information about all REPLAY files in replay chain and exit.  The 'initial' field of each replay summary lists arguments intended to be used for recording other replay files with same initial conditions.", 0},
  {"validate-replay-chain", VALIDATE_REPLAY_CHAIN_OPTION, "MODE", 0, "Validate replay chain.  Valid values for MODE are: NONE, READ and WRITE.  The default is NONE.  If MODE is READ, instead of reporting invalid sequent replay pairs, modify replay parameters just enough to validate pairs.  Notice that this requires consecutive replay levels to succeed.  WRITE does the same, additionally updating replay files in case the resulting chain is complete and valid.", 0},
  {"print-replay-favorites", PRINT_REPLAY_FAVORITES_OPTION, NULL, OPTION_NO_USAGE, "Print replay favorites list.  Exit with zero status in case the list is non-empty (non-zero otherwise).", 0},
  {"replay-favorite", REPLAY_FAVORITE_OPTION, "N", OPTION_NO_USAGE, "Go to replay favorite N at start.  See option '--print-replay-favorites' for the list of available replay favorites.", 0},

  {NULL, 0, NULL, OPTION_DOC, "Unless '--replay-info' is specified, REPLAY files given on command line are added to the replay chain in order to play and check for completion and sequence validity.  The replay chain is sorted by increasing level order before processing.  For each replay in the chain a replay summary is printed.  Unless '--validate-replay-chain' is specified, in case there is any invalid sequent pairs in the chain, their incompatible options are printed between their replay summaries.  For any complete replay summary, its 'final' field lists arguments intended to be used for continuing the game from where its respective replay ends.  If the replay chain is complete and valid, MININIM automatically exits with zero status (non-zero otherwise).  Replay chains can be played in-game using the F7 key binding.  One can use '--time-frequency' and its related key bindings to control the playback speed, in particular use '--time-frequency=0' and '--rendering=NONE' for the fastest batch processing of replays.", 0},

  {NULL, 0, NULL, OPTION_DOC, "Replay favorites allow the user to conveniently reach previously marked points in replays.  They can be easily managed and used from the Play>Favorites menu and their records are kept in the main configuration file.", 0},

  /* Compatibility */
  {NULL, 0, NULL, 0, "Compatibility", 0},
  {"semantics", SEMANTICS_OPTION, "SEMANTICS", 0, "Select semantics.  SEMANTICS determines the meaning and behavior of game elements.  Currently it's used to make legacy level sets which depend on the original semantics finishable.  Valid values for SEMANTICS are: NATIVE and LEGACY.  The default is NATIVE.  ", 0},
  {"movements", MOVEMENTS_OPTION, "MOVEMENTS", 0, "Select movements.  MOVEMENTS determines the set of movements the kid can perform.  Valid values for MOVEMENTS are: NATIVE and LEGACY.  The default is NATIVE.", 0},

  /* Paths */
  {NULL, 0, NULL, 0, "Paths:", 0},
  {"data-path", DATA_PATH_OPTION, "PATH", 0, "Set data path to PATH.  Normally, the data files are looked for in the user data directory, then in the current working directory, then in the resources directory, and finally in the system data directory.  If this option is given, after looking in the user data directory the data files are looked for in PATH.", 0},
  {"print-paths", PRINT_PATHS_OPTION, NULL, OPTION_NO_USAGE, "Print paths and exit.", 0},

  /* Others */
  {NULL, 0, NULL, 0, "Others", 0},
  {"sound-gain", SOUND_GAIN_OPTION, "F", 0, "Set sound volume gain to F.  This number is multiplied by the volume of sound effects in order to scale they down proportionally in case the default is too loud for the user.  The default is 1.0.  Valid floating values range from 0.0 (disables sound) to 1.0 (default volume).  This can be changed in-game using the CTRL+S key binding.", 0},
  {"skip-title", SKIP_TITLE_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Skip title screen.  The default is FALSE.", 0},
  {"inhibit-screensaver", INHIBIT_SCREENSAVER_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Prevent the system screensaver from starting up.  The default is TRUE.", 0},
  {"random-seed", RANDOM_SEED_OPTION, "N", 0, "Set initial random seed to N.  If N is zero, the initial random seed is derived from current time.  This is the default.  Valid integers range from 0 to INT_MAX.  This option is potentially useful for debugging purposes.", 0},

  /* Easter eggs */
  {"scream", SCREAM_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL | OPTION_HIDDEN, "In MININIM, everybody screams!", 0},

  /* Help */
  {NULL, 0, NULL, 0, "Help:", -1},
  {0},
};

static const char doc[] = "\nMININIM is the Advanced Prince of Persia Engine --- a childhood dream, the free software implementation of Jordan Mechner's masterpiece game, developed from scratch by Bruno Félix Rezende Ribeiro (oitofelix). MININIM is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3, or (at your option) any later version.\n\n"
  "In addition to being already a complete replacement for the original game with several improvements, MININIM aims to have detailed documentation on all matters concerning the original Prince of Persia world, ranging from its design concepts to the engine's practical use and development. MININIM documentation is free; you can redistribute it and/or modify it under the terms of the GNU FDL (Free Documentation Licence) as published by the FSF --- with no Invariant Sections; either version 1.3, or (at your option) any later version."
  "\vLong option names are case sensitive.  Option values are case insensitive.   Both can be partially specified as long as they are kept unambiguous.  BOOLEAN is an integer equating to 0, or any sub-string (including the null string) of 'FALSE', 'OFF' or 'NO' to disable the respective feature, and any other value (even no string at all) to enable it.  For any non-specified option the documented default applies.  Numbers may be specified in any of the formats defined by the C language.  Key bindings references are based on the default mapping.\n\n\
The legacy command line interface present in versions 1.0, 1.3 and 1.4 of the original game is supported for the sake of compatibility with software that use it.  Legacy level and video non-option arguments are honored, while all others are currently ignored silently.  The legacy arguments can't be used by other configuration method besides the command line.";

static char args_doc[] = "\n"
  "[REPLAY...]\n"
  "--replay-info [REPLAY...]\n"
  "--print-replay-favorites\n"
  "--replay-favorite=N\n"
  "--joystick-info\n"
  "--print-display-modes\n"
  "--print-paths\n"
  "--level-module=LEVEL-MODULE --mirror-level=BOOLEAN --convert-levels";

struct argp_child argp_child = { NULL };

static struct argp pre_argp = {options, pre_parser, args_doc, doc,
                               &argp_child, NULL, NULL};
static struct argp argp = {options, parser, args_doc, doc,
                           &argp_child, NULL, NULL};

char *
key_to_option_name (int key, struct argp_state *state)
{
  char *option_name;
  struct config_info *config_info = (struct config_info *) state->input;

  size_t i;
  for (i = 0; options[i].name != NULL
         || options[i].key != 0
         || options[i].arg != NULL
         || options[i].flags != 0
         || options[i].doc != NULL
         || options[i].group != 0; i++)
    if (options[i].key == key) {
      switch (config_info->type) {
      case CI_CONFIGURATION_FILE:
        return command_line2config_option_name (options[i].name);
      case CI_ENVIRONMENT_VARIABLES:
        return command_line2env_option_name (options[i].name);
      case CI_COMMAND_LINE:
        option_name = xasprintf ("%s", options[i].name);
        return option_name;
      }
    }

  return NULL;
}

void
option_enum_value_error (int key, char *arg, struct argp_state *state,
                         char **enum_vals, bool invalid, int number)
{
  char *msg = NULL;
  char *option_name = key_to_option_name (key, state);
  struct config_info *config_info = (struct config_info *) state->input;

  switch (config_info->type) {
  case CI_CONFIGURATION_FILE:
    msg = xasprintf
      ("%s", invalid
       ? "is invalid for the configuration file option"
       : "is ambiguous for the configuration file option");
    break;
  case CI_ENVIRONMENT_VARIABLES:
    msg = xasprintf
      ("%s", invalid
       ? "is invalid for the environment variable option"
       : "is ambiguous for the environment variable option");
    break;
  case CI_COMMAND_LINE:
    msg = xasprintf
      ("%s", invalid
       ? "is invalid for the command line option"
       : "is ambiguous for the command line option");
    break;
  }

  char *prefix = invalid ? "" : arg;

  char *valid_values = NULL;
  size_t i;
  for (i = 0; enum_vals[i] != NULL; i++)
    if (strcasestr (enum_vals[i], prefix) == enum_vals[i]) {
      char *tmpstr = valid_values;
      if (! valid_values)
        valid_values = xasprintf ("'%s'", enum_vals[i]);
      else
        valid_values = xasprintf ("%s, '%s'", valid_values, enum_vals[i]);
      al_free (tmpstr);
    }

  char *msg2;
  if (invalid) msg2 = xasprintf ("%s", "Valid values are:");
  else msg2 = xasprintf ("Valid values starting with '%s' are:", arg);

  char *config_file_prefix;
  if (config_info->type == CI_CONFIGURATION_FILE)
    config_file_prefix = xasprintf ("%s: ", config_info->filename);
  else config_file_prefix = xasprintf ("%s", "");

  char *error_template = "%s'%s' %s '%s' argument %i.\n%s %s.";
  if (config_info->type == CI_CONFIGURATION_FILE
      && state->flags & ARGP_SILENT) {
    print_text_log (error_template, config_file_prefix, arg, msg, option_name,
                    number, msg2, valid_values);
    print_text_log ("\n");
  } else argp_error (state, error_template,
                   config_file_prefix, arg, msg, option_name,
                   number, msg2, valid_values);

  al_free (option_name);
  al_free (msg);
  al_free (msg2);
  al_free (valid_values);
  al_free (config_file_prefix);
}

void
option_arg_error (int key, char *arg, struct argp_state *state, int number, char *estr)
{
  struct config_info *config_info = (struct config_info *) state->input;
  char *option_name = key_to_option_name (key, state);
  char *config_file_prefix;
  char *msg;
  char *argument_msg;

  if (config_info->type == CI_CONFIGURATION_FILE)
    config_file_prefix = xasprintf ("%s: ", config_info->filename);
  else config_file_prefix = xasprintf ("%s", "");

  switch (config_info->type) {
  case CI_CONFIGURATION_FILE:
    msg = "is invalid for the configuration file option";
    break;
  case CI_ENVIRONMENT_VARIABLES:
    msg = "is invalid for the environment variable option";
    break;
  case CI_COMMAND_LINE:
    msg = "is invalid for the command line option";
    break;
  }

  if (number != -1) argument_msg = xasprintf (" argument %i", number);
  else argument_msg = xasprintf ("%s", "");

  char *error_template = "%s'%s' %s '%s'%s.\n%s";

  if (config_info->type == CI_CONFIGURATION_FILE
      && state->flags & ARGP_SILENT) {
    print_text_log (error_template, config_file_prefix, arg, msg, option_name,
                    argument_msg, estr);
    print_text_log ("\n");
  } else argp_error (state, error_template,
                   config_file_prefix, arg, msg, option_name,
                   argument_msg, estr);

  al_free (option_name);
  al_free (config_file_prefix);
  al_free (argument_msg);
}

bool
optval_to_bool (char *arg)
{
  int i;
  char *FALSE_VAL = "FALSE";
  char *OFF_VAL = "OFF";
  char *NO_VAL = "NO";

  if (! arg) return true;

  if (sscanf (arg, "%i", &i) == 1 && i == 0) return false;
  if (strcasestr (FALSE_VAL, arg) == FALSE_VAL) return false;
  if (strcasestr (OFF_VAL, arg) == OFF_VAL) return false;
  if (strcasestr (NO_VAL, arg) == NO_VAL) return false;

  return true;
}

error_t
optval_to_int (int *retval, int key, char *arg, struct argp_state *state,
               struct int_range *r, int number)
{
  int i;
  char *estr;

  if (sscanf (arg, "%i", &i) != 1) {
    option_arg_error (key, arg, state, number, "Reason: argument is not an integer.");
    return EINVAL;
  }

  if (i < r->a || i > r->b) {
    estr = xasprintf ("Reason: argument is not in the range [%i,%i].",
                      r->a, r->b);
    option_arg_error (key, arg, state, number, estr);
    al_free (estr);
    return EINVAL;
  }

  *retval = i;
  return 0;
}

error_t
optval_to_float (float *retval, int key, char *arg, struct argp_state *state,
                 struct float_range *r, int number)
{
  float f;
  char *estr;

  if (sscanf (arg, "%f", &f) != 1) {
    option_arg_error (key, arg, state, number, "Reason: argument is not a float.");
    return EINVAL;
  }

  if (f < r->a || f > r->b) {
    estr = xasprintf ("Reason: argument is not in the range [%f,%f].",
                      r->a, r->b);
    option_arg_error (key, arg, state, number, estr);
    al_free (estr);
    return EINVAL;
  }

  *retval = f;
  return 0;
}

error_t
optval_to_enum (int *retval, int key, char *arg, struct argp_state *state,
                char **enum_vals, int number)
{
  size_t i;
  int optval = -1;
  bool ambiguous = false;

  for (i = 0; enum_vals[i] != NULL; i++) {
    if (strcasestr (enum_vals[i], arg) == enum_vals[i]) {
      if (! strcasecmp (enum_vals[i], arg)) {
        *retval = i;
        return 0;
      }
      if (optval != -1) ambiguous = true;
      optval = i;
    }
  }

  if (optval == -1) {
    option_enum_value_error (key, arg, state, enum_vals, true, number);
    return EINVAL;
  }
  else if (ambiguous) {
    option_enum_value_error (key, arg, state, enum_vals, false, number);
    return EINVAL;
  }

  *retval = optval;

  return 0;
}

error_t
option_get_args (int key, char *arg, struct argp_state *state, char s, ...)
{
  va_list ap, at, av, ar, atype, aval, arange;
  int i;
  error_t retval;
  char *arg2, *s2;
  arg2 = xasprintf ("%s", arg);
  s2 = xasprintf ("%c", s);
  char *estr;

  /* count number of arguments */
  int num_args = 0;
  va_start (ap, s);
  while (va_arg (ap, enum opt_arg_type) != ARG_TYPE_NULL) num_args++;

  /* Nothing to do, if it expects 0 arguments */
  if (num_args == 0) {
    retval = 0;
    goto end;
  }

  /* set argument lists */
  va_start (atype, s);
  va_end (ap);
  va_copy (ap, atype);
  for (i = 0; i <= num_args; i++) va_arg (ap, enum opt_arg_type);
  va_copy (aval, ap);
  for (i = 0; i < num_args; i++) va_arg (ap, void *);
  va_copy (arange, ap);

  /* get values and check ranges */
  va_copy (at, atype);
  va_copy (av, aval);
  va_copy (ar, arange);
  for (i = 0; i < num_args; i++) {
    enum opt_arg_type type = va_arg (at, enum opt_arg_type);

    char *str = strtok (i ? NULL : arg2, s2);
    if (! str) {
      estr = xasprintf ("Reason: less than %i arguments provided.", num_args);
      option_arg_error (key, arg, state, -1, estr);
      al_free (estr);
      retval = EINVAL;
      goto end;
    }

    void *val = va_arg (av, void *);
    void *range = va_arg (ar, void *);

    assert (type == ARG_TYPE_INT || type == ARG_TYPE_FLOAT
            || type == ARG_TYPE_ENUM);
    switch (type) {
    case ARG_TYPE_BOOL:
      retval = 0;
      *(bool *) val = optval_to_bool (str);
      break;
    case ARG_TYPE_INT:
      retval = optval_to_int ((int *) val, key, str, state, range, i);
      break;
    case ARG_TYPE_FLOAT:
      retval = optval_to_float ((float *) val, key, str, state, range, i);
      break;
    case ARG_TYPE_ENUM:
      retval = optval_to_enum ((int *) val, key, str, state, range, i);
    default: break;
    }

    if (retval) goto end;
  }

  retval = 0;

 end: /* cleanup */
  va_end (ap);
  va_end (at);
  va_end (av);
  va_end (ar);
  va_end (atype);
  va_end (aval);
  va_end (arange);
  al_free (arg2);
  al_free (s2);
  return retval;
}

error_t
pre_parser (int key, char *arg, struct argp_state *state)
{
  switch (key) {
  case IGNORE_MAIN_CONFIG_OPTION:
    ignore_main_config = true;
    break;
  case IGNORE_ENVIRONMENT_OPTION:
    ignore_environment = true;
    break;
  }
  return 0;
}

error_t
parser (int key, char *arg, struct argp_state *state)
{
  char **cargv = NULL;
  size_t cargc = 0;
  int x, y, i, e;
  struct config_info config_info;
  float float_val;
  int int_val0, int_val1, int_val2;
  enum file_type file_type;

  char *level_module_enum[] = {"NATIVE", "LEGACY", "PLV", "DAT", "CONSISTENCY", NULL};

  char *environment_mode_enum[] = {"ORIGINAL", "DUNGEON", "PALACE", NULL};

  char *hue_mode_enum[] = {"ORIGINAL", "NONE", "GREEN",
                           "GRAY", "YELLOW", "BLUE", NULL};

  char *guard_mode_enum[] = {"ORIGINAL", "GUARD", "FAT",
                             "VIZIER", "SKELETON", "SHADOW", NULL};

  char *display_flip_mode_enum[] = {"NONE", "VERTICAL", "HORIZONTAL",
                                    "VERTICAL-HORIZONTAL", NULL};

  char *gamepad_mode_enum[] = {"KEYBOARD", "JOYSTICK", NULL};

  char *gamepad_flip_mode_enum[] = {"NONE", "VERTICAL", "HORIZONTAL",
                                    "VERTICAL-HORIZONTAL", NULL};

  char *joystick_axis_threshold_enum[] = {"H", "V", NULL};

  char *joystick_button_threshold_enum[] = {"UP", "RIGHT", "DOWN", "LEFT",
                                            "ENTER", "SHIFT", "CTRL", "ALT",
                                            NULL};

  char *joystick_axis_enum[] = {"H", "V", NULL};

  char *joystick_button_enum[] = {"UP", "RIGHT", "DOWN", "LEFT",
                                  "ENTER", "SHIFT", "CTRL", "ALT",
                                  "TIME", "PAUSE", NULL};

  char *multi_room_fit_mode_enum[] = {"NONE", "STRETCH", "RATIO", NULL};

  char *semantics_enum[] = {"NATIVE", "LEGACY", NULL};

  char *movements_enum[] = {"NATIVE", "LEGACY", NULL};

  char *rendering_enum[] = {"BOTH", "VIDEO", "AUDIO", "NONE", NULL};

  char *validate_replay_chain_enum[] = {"NONE", "READ", "WRITE", NULL};

  struct int_range total_hp_range = {1, INT_MAX};
  struct int_range start_level_range = {0, INT_MAX};
  struct int_range start_pos_room_range = {1, INT_MAX};
  struct int_range start_pos_floor_range = {0, 2};
  struct int_range start_pos_place_range = {0, 9};
  struct int_range time_limit_range = {1, INT_MAX};
  struct int_range start_time_range = {0, INT_MAX};
  struct int_range time_frequency_range = {0, UNLIMITED_HZ};
  struct int_range kca_range = {0, 100};
  struct int_range kcd_range = {0, 100};
  struct int_range window_position_range = {INT_MIN, INT_MAX};
  struct int_range window_dimensions_range = {1, INT_MAX};
  struct int_range multi_room_range = {1, INT_MAX};
  struct float_range joystick_axis_threshold_range = {0.0,1.0};
  struct int_range joystick_button_threshold_range = {0, 32767};
  struct int_range joystick_axis_range = {0, INT_MAX};
  struct int_range joystick_button_range = {0, INT_MAX};
  struct int_range display_mode_range = {-1, al_get_num_display_modes () - 1};
  struct float_range gamepad_rumble_gain_range = {0.0,1.0};
  struct int_range random_seed_range = {0, INT_MAX};
  struct float_range sound_gain_range = {0.0,1.0};
  struct int_range replay_favorites_range = {0, replay_favorite_nmemb - 1};

  switch (key) {
  case IGNORE_MAIN_CONFIG_OPTION:
    break;
  case IGNORE_ENVIRONMENT_OPTION:
    break;
  case LOAD_CONFIG_OPTION:
    file_type = UNKNOWN_FILE_TYPE;
    e = get_config_args (&cargc, &cargv, options, arg, &file_type);
    if (e) {
      argp_failure (state, -1, e, "can't load %s '%s'",
                    file_type2str (file_type), arg);
      return e;
    }
    config_info.type = CI_CONFIGURATION_FILE;
    config_info.filename = arg;
    argp_parse (&argp, cargc, cargv, 0, NULL, &config_info);
    free_argv (&cargc, &cargv);
    break;
  case LEVEL_MODULE_OPTION:
    e = optval_to_enum (&i, key, arg, state, level_module_enum, 0);
    if (e) return e;
    level_module_given = true;
    switch (i) {
    case 0: level_module = NATIVE_LEVEL_MODULE; break;
    case 1: level_module = LEGACY_LEVEL_MODULE; break;
    case 2: level_module = PLV_LEVEL_MODULE; break;
    case 3: level_module = DAT_LEVEL_MODULE; break;
    case 4: level_module = CONSISTENCY_LEVEL_MODULE; break;
    }
    break;
  case CONVERT_LEVELS_OPTION:
    min_legacy_level = 0;
    max_legacy_level = 15;
    give_dat_compat_preference ();
    for (i = min_legacy_level; i <= max_legacy_level; i++) {
      level_module_next_level (&vanilla_level, i);
      if (mirror_level)
        mirror_level_h (&vanilla_level);
      if (! save_level (&vanilla_level)) exit (-1);
    }
    fprintf (stderr, "\
Levels have been converted using module %s into native format at\n\
%s\n", level_module_str (level_module), user_data_dir);
    exit (0);
    break;
  case MIRROR_LEVEL_OPTION:
    mirror_level = optval_to_bool (arg);
    break;
  case VIDEO_MODE_OPTION:
    set_string_var (&video_mode, arg);
    break;
  case ENVIRONMENT_MODE_OPTION:
    e = optval_to_enum (&i, key, arg, state, environment_mode_enum, 0);
    if (e) return e;
    switch (i) {
    case 0: force_em = false;
      set_string_var (&env_mode, "ORIGINAL"); break;
    case 1: force_em = true, em = DUNGEON;
      set_string_var (&env_mode, "DUNGEON"); break;
    case 2: force_em = true, em = PALACE;
      set_string_var (&env_mode, "PALACE"); break;
    }
    break;
  case HUE_MODE_OPTION:
    e = optval_to_enum (&i, key, arg, state, hue_mode_enum, 0);
    if (e) return e;
    switch (i) {
    case 0: force_hue = false;
      set_string_var (&hue_mode, "ORIGINAL");
      break;
    case 1: force_hue = true, hue = HUE_NONE;
      set_string_var (&hue_mode, "NONE");
      break;
    case 2: force_hue = true, hue = HUE_GREEN;
      set_string_var (&hue_mode, "GREEN");
      break;
    case 3: force_hue = true, hue = HUE_GRAY;
      set_string_var (&hue_mode, "GRAY");
      break;
    case 4: force_hue = true, hue = HUE_YELLOW;
      set_string_var (&hue_mode, "YELLOW");
      break;
    case 5: force_hue = true, hue = HUE_BLUE;
      set_string_var (&hue_mode, "BLUE");
      break;
    }
    break;
  case GUARD_MODE_OPTION:
    e = optval_to_enum (&i, key, arg, state, guard_mode_enum, 0);
    if (e) return e;
    switch (i) {
    case 0: gm = ORIGINAL_GM; break;
    case 1: gm = GUARD_GM; break;
    case 2: gm = FAT_GM; break;
    case 3: gm = VIZIER_GM; break;
    case 4: gm = SKELETON_GM; break;
    case 5: gm = SHADOW_GM; break;
    }
    break;
  case SOUND_GAIN_OPTION:
    e = optval_to_float (&float_val, key, arg, state,
                         &sound_gain_range, 0);
    if (e) return e;
    audio_volume = float_val;
    break;
  case DISPLAY_FLIP_MODE_OPTION:
    e = optval_to_enum (&i, key, arg, state, display_flip_mode_enum, 0);
    if (e) return e;
    switch (i) {
    case 0: screen_flags = 0; break;
    case 1: screen_flags = ALLEGRO_FLIP_VERTICAL; break;
    case 2: screen_flags = ALLEGRO_FLIP_HORIZONTAL; break;
    case 3: screen_flags = ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL;
      break;
    }
    break;
  case MULTI_ROOM_FIT_MODE_OPTION:
    e = optval_to_enum (&i, key, arg, state, multi_room_fit_mode_enum, 0);
    if (e) return e;
    switch (i) {
    case 0: mr_fit_mode = MR_FIT_NONE; break;
    case 1: mr_fit_mode = MR_FIT_STRETCH; break;
    case 2: mr_fit_mode = MR_FIT_RATIO; break;
    }
    break;
  case GAMEPAD_MODE_OPTION:
    e = optval_to_enum (&i, key, arg, state, gamepad_mode_enum, 0);
    if (e) return e;
    switch (i) {
    case 0:
      gpm = KEYBOARD;
      break;
    case 1:
      gpm = JOYSTICK;
      break;
    }
    break;
  case GAMEPAD_FLIP_MODE_OPTION:
    e = optval_to_enum (&i, key, arg, state, gamepad_flip_mode_enum, 0);
    if (e) return e;
    switch (i) {
    case 0:
      flip_gamepad_vertical = false;
      flip_gamepad_horizontal = false;
      break;
    case 1:
      flip_gamepad_vertical = true;
      flip_gamepad_horizontal = false;
      break;
    case 2:
      flip_gamepad_vertical = false;
      flip_gamepad_horizontal = true;
      break;
    case 3:
      flip_gamepad_vertical = true;
      flip_gamepad_horizontal = true;
      break;
    }
    break;
  case GAMEPAD_RUMBLE_GAIN_OPTION:
    e = optval_to_float (&float_val, key, arg, state,
                         &gamepad_rumble_gain_range, 0);
    if (e) return e;
    gamepad_rumble_gain = float_val;
    break;
  case MIRROR_MODE_OPTION:
    mirror_mode (optval_to_bool (arg));
    break;
  case BLIND_MODE_OPTION:
    no_room_drawing = optval_to_bool (arg);
    break;
  case IMMORTAL_MODE_OPTION:
    immortal_mode = optval_to_bool (arg);
    break;
  case TOTAL_HP_OPTION:
    e = optval_to_int (&i, key, arg, state, &total_hp_range, 0);
    if (e) return e;
    initial_total_hp = i;
    break;
  case START_LEVEL_OPTION:
    e = optval_to_int (&i, key, arg, state, &start_level_range, 0);
    if (e) return e;
    start_level = i;
    break;
  case START_POS_OPTION:
    e = option_get_args (key, arg, state, ',', ARG_TYPE_INT, ARG_TYPE_INT, ARG_TYPE_INT, 0,
                         &int_val0, &int_val1, &int_val2,
                         &start_pos_room_range, &start_pos_floor_range, &start_pos_place_range);
    if (e) return e;
    new_pos (&start_pos, &global_level, int_val0, int_val1, int_val2);
    break;
  case TIME_LIMIT_OPTION:
    e = optval_to_int (&i, key, arg, state, &time_limit_range, 0);
    if (e) return e;
    time_limit = i;
    break;
  case START_TIME_OPTION:
    e = optval_to_int (&i, key, arg, state, &start_time_range, 0);
    if (e) return e;
    start_time = i;
    break;
  case TIME_FREQUENCY_OPTION:
    e = optval_to_int (&i, key, arg, state, &time_frequency_range, 0);
    if (e) return e;
    anim_freq = i;
    break;
  case KCA_OPTION:
    e = optval_to_int (&i, key, arg, state, &kca_range, 0);
    if (e) return e;
    skill.counter_attack_prob = i - 1;
    break;
  case KCD_OPTION:
    e = optval_to_int (&i, key, arg, state, &kcd_range, 0);
    if (e) return e;
    skill.counter_defense_prob = i - 1;
    break;
  case DISPLAY_MODE_OPTION:
    e = optval_to_int (&i, key, arg, state, &display_mode_range, 0);
    if (e) return e;
    display_mode = i;
    break;
  case DATA_PATH_OPTION:
    data_dir = xasprintf ("%s", arg);
    break;
  case FULLSCREEN_OPTION:
    if (optval_to_bool (arg))
      al_set_new_display_flags (al_get_new_display_flags ()
                                | ALLEGRO_FULLSCREEN_WINDOW);
    else al_set_new_display_flags (al_get_new_display_flags ()
                                   & ~ALLEGRO_FULLSCREEN_WINDOW);
    break;
  case WINDOW_POSITION_OPTION:
    e = option_get_args (key, arg, state, ',', ARG_TYPE_INT, ARG_TYPE_INT, 0,
                         &x, &y, &window_position_range, &window_position_range);
    if (e) return e;
    al_set_new_window_position (x, y);
    break;
  case WINDOW_DIMENSIONS_OPTION:
    e = option_get_args (key, arg, state, 'x', ARG_TYPE_INT, ARG_TYPE_INT, 0,
                         &display_width, &display_height,
                         &window_dimensions_range, &window_dimensions_range);
    if (e) return e;
    break;
  case MULTI_ROOM_OPTION:
    e = option_get_args (key, arg, state, 'x', ARG_TYPE_INT, ARG_TYPE_INT, 0,
                         &global_mr.fit_w, &global_mr.fit_h,
                         &multi_room_range, &multi_room_range);
    if (e) return e;
    break;
  case INHIBIT_SCREENSAVER_OPTION:
    inhibit_screensaver = optval_to_bool (arg);
    break;
  case PRINT_PATHS_OPTION:
    print_paths ();
    exit (0);
  case PRINT_DISPLAY_MODES_OPTION:
    print_display_modes ();
    exit (0);
  case SKIP_TITLE_OPTION:
    skip_title = optval_to_bool (arg);
    break;
  case JOYSTICK_AXIS_THRESHOLD_OPTION:
    e = option_get_args (key, arg, state, ',', ARG_TYPE_ENUM, ARG_TYPE_FLOAT, 0,
                         &i, &float_val, joystick_axis_threshold_enum,
                         &joystick_axis_threshold_range);
    if (e) return e;
    switch (i) {
    case 0: joystick_h_threshold = float_val; break;
    case 1: joystick_v_threshold = float_val; break;
    }
    break;
  case JOYSTICK_BUTTON_THRESHOLD_OPTION:
    e = option_get_args (key, arg, state, ',', ARG_TYPE_ENUM, ARG_TYPE_INT, 0,
                         &i, &int_val0, joystick_button_threshold_enum,
                         &joystick_button_threshold_range);
    if (e) return e;
    switch (i) {
    case 0: joystick_up_threshold = int_val0; break;
    case 1: joystick_right_threshold = int_val0; break;
    case 2: joystick_down_threshold = int_val0; break;
    case 3: joystick_left_threshold = int_val0; break;
    case 4: joystick_enter_threshold = int_val0; break;
    case 5: joystick_shift_threshold = int_val0; break;
    case 6: joystick_ctrl_threshold = int_val0; break;
    case 7: joystick_alt_threshold = int_val0; break;
    }
    break;
  case JOYSTICK_AXIS_OPTION:
    e = option_get_args (key, arg, state, ',',
                         ARG_TYPE_ENUM, ARG_TYPE_INT, ARG_TYPE_INT, 0,
                         &i, &int_val0, &int_val1, joystick_axis_enum,
                         &joystick_axis_range, &joystick_axis_range);
    if (e) return e;
    switch (i) {
    case 0:
      joystick_h_stick = int_val0;
      joystick_h_axis = int_val1;
      break;
    case 1:
      joystick_v_stick = int_val0;
      joystick_v_axis = int_val1;
      break;
    }
    break;
  case JOYSTICK_BUTTON_OPTION:
    e = option_get_args (key, arg, state, ',', ARG_TYPE_ENUM, ARG_TYPE_INT, 0,
                         &i, &int_val0, joystick_button_enum, &joystick_button_range);
    if (e) return e;
    switch (i) {
    case 0: joystick_up_button = int_val0; break;
    case 1: joystick_right_button = int_val0; break;
    case 2: joystick_down_button = int_val0; break;
    case 3: joystick_left_button = int_val0; break;
    case 4: joystick_enter_button = int_val0; break;
    case 5: joystick_shift_button = int_val0; break;
    case 6: joystick_ctrl_button = int_val0; break;
    case 7: joystick_alt_button = int_val0; break;
    case 8: joystick_time_button = int_val0; break;
    case 9: joystick_pause_button = int_val0; break;
    }
    break;
  case JOYSTICK_INFO_OPTION:
    if (joystick_info ()) error (-1, 0, "Joystick not found");
    exit (0);
    break;
  case SEMANTICS_OPTION:
    e = optval_to_enum (&i, key, arg, state, semantics_enum, 0);
    if (e) return e;
    switch (i) {
    case 0: semantics = NATIVE_SEMANTICS; break;
    case 1:
      semantics = LEGACY_SEMANTICS;
      global_mr.fit_w = global_mr.fit_h = 1;
      break;
    }
    break;
  case MOVEMENTS_OPTION:
    e = optval_to_enum (&i, key, arg, state, movements_enum, 0);
    if (e) return e;
    switch (i) {
    case 0: movements = NATIVE_MOVEMENTS; break;
    case 1: movements = LEGACY_MOVEMENTS; break;
    }
    break;
  case RECORD_REPLAY_OPTION:
    level_start_replay_mode = NO_REPLAY;
    next_level_number = -1;
    skip_title = false;
    prepare_for_recording_replay ();
    break;
  case REPLAY_INFO_OPTION:
    replay_info = true;
    break;
  case PRINT_REPLAY_FAVORITES_OPTION:
    if (print_replay_favorites ()) exit (0);
    else exit (-1);
    break;
  case REPLAY_FAVORITE_OPTION:
    if (! replay_favorite_nmemb)
      error (-1, 0, "no replay favorites available");
    e = optval_to_int (&i, key, arg, state, &replay_favorites_range, 0);
    if (e) return e;
    start_replay_favorite = i;
    skip_title = true;
    break;
  case RENDERING_OPTION:
    e = optval_to_enum (&i, key, arg, state, rendering_enum, 0);
    if (e) return e;
    switch (i) {
    case 0: rendering = BOTH_RENDERING; break;
    case 1: rendering = VIDEO_RENDERING; break;
    case 2: rendering = AUDIO_RENDERING; break;
    case 3: rendering = NONE_RENDERING; break;
    }
    break;
  case VALIDATE_REPLAY_CHAIN_OPTION:
    e = optval_to_enum (&i, key, arg, state, validate_replay_chain_enum, 0);
    if (e) return e;
    switch (i) {
    case 0: validate_replay_chain = NONE_VALIDATE_REPLAY_CHAIN; break;
    case 1: validate_replay_chain = READ_VALIDATE_REPLAY_CHAIN; break;
    case 2: validate_replay_chain = WRITE_VALIDATE_REPLAY_CHAIN; break;
    }
    break;
  case SCREAM_OPTION:
    scream = optval_to_bool (arg);
    break;
  case RANDOM_SEED_OPTION:
    e = optval_to_int (&i, key, arg, state, &random_seed_range, 0);
    if (e) return e;
    random_seed = i;
    break;
  case ARGP_KEY_ARG:
    if (add_replay_file_to_replay_chain (arg)) {
      command_line_replay = true;
      skip_title = true;
      prepare_for_playing_replay (0);
      break;
    }

    /* cheat */
    else if (! strcasecmp ("MEGAHIT", arg)) break;
    else if (! strcasecmp ("IMPROVED", arg)) break;
    else if (sscanf (arg, "%i", &i) == 1
             && i >= 1 && i <= INT_MAX) start_level = i;

    /* audio */
    else if (! strcasecmp ("ADLIB", arg)) break;
    else if (! strcasecmp ("COMPUADD", arg)) break;
    else if (! strcasecmp ("COVOX", arg)) break;
    else if (! strcasecmp ("DIGI", arg)) break;
    else if (! strcasecmp ("DISNEY", arg)) break;
    else if (! strcasecmp ("GBLAST", arg)) break;
    else if (! strcasecmp ("IBMG", arg)) break;
    else if (! strcasecmp ("MIDI", arg)) break;
    else if (! strcasecmp ("MVPAS", arg)) break;
    else if (! strcasecmp ("SBLAST", arg)) break;
    else if (! strcasecmp ("STDSND", arg)) break;
    else if (! strcasecmp ("TANDY", arg)) break;

    /* video */
    else if (! strcasecmp ("CGA", arg))
      set_string_var (&video_mode, "DOS CGA");
    else if (! strcasecmp ("DRAW", arg)) break;
    else if (! strcasecmp ("EGA", arg))
      set_string_var (&video_mode, "DOS EGA");
    else if (! strcasecmp ("HERC", arg))
      set_string_var (&video_mode, "DOS HGA");
    else if (! strcasecmp ("HGA", arg))
      set_string_var (&video_mode, "DOS HGA");
    else if (! strcasecmp ("MCGA", arg))
      set_string_var (&video_mode, "DOS VGA");
    else if (! strcasecmp ("TGA", arg))
      set_string_var (&video_mode, "DOS EGA");
    else if (! strcasecmp ("VGA", arg))
      set_string_var (&video_mode, "DOS VGA");

    /* memory */
    else if (! strcasecmp ("BYPASS", arg)) break;
    else if (! strcasecmp ("PACK", arg)) break;
    else if (! strcasecmp ("UNPACK", arg)) break;

    /* miscellaneous */
    else if (! strcasecmp ("DEMO", arg)) break;
    else if (! strcasecmp ("J", arg)) break;

    else {
      argp_error (state, "unrecognized non-option argument '%s'.\n"
                  "That's not a valid replay file.\n"
                  "That's not a legacy argument.", arg);
      return ARGP_ERR_UNKNOWN;
    }
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

void
version (FILE *stream, struct argp_state *state)
{
  uint32_t allegro_version = al_get_allegro_version ();
  int allegro_major = allegro_version >> 24;
  int allegro_minor = (allegro_version >> 16) & 255;
  int allegro_revision = (allegro_version >> 8) & 255;
  int allegro_release = allegro_version & 255;

  fprintf (stream,
           "%s (%s) %s\n\n"	/* mininim (MININIM) a.b */

           "Copyright (C) %s " PACKAGE_COPYRIGHT_HOLDER " <%s>\n\n"

           "%s\n\n" /* License GPLv3+... */
           "%s\n\n" /* Written by... */
           "Using Allegro %i.%i.%i[%i].\n", /* Using Allegro... */
           PACKAGE, PACKAGE_NAME, VERSION,
           "2015, 2016, 2017", "oitofelix@gnu.org",
           "MININIM is free software under GPLv3+.\n"
           "You are free to change and redistribute it.\n"
           "There is NO WARRANTY, to the extent permitted by law.",

           "Written by Bruno Félix Rezende Ribeiro.",
           allegro_major, allegro_minor, allegro_revision, allegro_release);
}

char *
command_line2env_option_name (const char *option_name)
{
  char *option;
  option = xasprintf (ENV_OPTION_PREFIX "%s", option_name);
  toupper_str (option);
  repl_str_char (option, '-', '_');
  return option;
}

char *
env2command_line_option_name (const char *option_name)
{
  if (memcmp (option_name, ENV_OPTION_PREFIX, sizeof (ENV_OPTION_PREFIX) - 1))
    return NULL;
  char *option;
  option = xasprintf ("%s", option_name + sizeof (ENV_OPTION_PREFIX) - 1);
  tolower_str (option);
  repl_str_char (option, '_', '-');
  return option;
}

void
config_str2key_value (const char *str, char **key, char **value)
{
  *key = xasprintf ("%s", str);
  repl_str_char (*key, '=', '\0');
  *value = *key + strlen (*key) + 1;
}

void
get_env_args (size_t *eargc, char ***eargv, struct argp_option *options)
{
  char *argv0; argv0 = xasprintf ("%s", argv[0]);
  *eargv = add_to_array (&argv0, 1, *eargv, eargc, *eargc, sizeof (argv0));

  size_t i = 0;
  while (environ[i]) {
    char *entry, *key, *value;
    config_str2key_value (environ[i], &entry, &value);
    key = env2command_line_option_name (entry);
    if (! key || ! is_valid_option (key)) goto next;

    char *option;
    option = xasprintf ("--%s=%s", key, value);
    *eargv = add_to_array (&option, 1, *eargv, eargc, *eargc, sizeof (option));

  next:
    al_free (entry);
    al_free (key);
    i++;
  }
}

char *
command_line2config_option_name (const char *option_name)
{
  char *option;
  option = xasprintf ("%s", option_name);
  toupper_str (option);
  repl_str_char (option, '-', ' ');
  return option;
}

char *
config2command_line_option_name (const char *option_name)
{
  char *option;
  option = xasprintf ("%s", option_name);
  tolower_str (option);
  repl_str_char (option, ' ', '-');
  return option;
}

bool
is_valid_option (char *option)
{
  size_t i;

  for (i = 0; options[i].name != NULL
         || options[i].key != 0
         || options[i].arg != NULL
         || options[i].flags != 0
         || options[i].doc != NULL
         || options[i].group != 0; i++) {
    if (! options[i].name) continue;
    if (! strcmp (options[i].name, option)) return true;
  }

  return false;
}

error_t
get_config_args (size_t *cargc, char ***cargv, struct argp_option *options,
                 char *filename, enum file_type *ret_file_type)
{
  enum file_type file_type = UNKNOWN_FILE_TYPE;

  ALLEGRO_CONFIG *config = al_load_config_file (filename);
  if (! config) {
    if (ret_file_type) *ret_file_type = file_type;
    free_argv (cargc, cargv);
    return al_get_errno ();
  }

  char *argv0; argv0 = xasprintf ("%s", argv[0]);
  *cargv = add_to_array (&argv0, 1, *cargv, cargc, *cargc, sizeof (argv0));

  ALLEGRO_CONFIG_ENTRY *iterator;
  char const *entry = al_get_first_config_entry (config, NULL, &iterator);

  while (entry) {
    char *key = config2command_line_option_name (entry);
    char const *value = al_get_config_value (config, NULL, entry);
    if (! is_valid_option (key)) {
      config_entry_file_type (entry, value, &file_type);
      goto next;
    }

    char *option;
    option = xasprintf ("--%s=%s", key, value);
    *cargv = add_to_array (&option, 1, *cargv, cargc, *cargc, sizeof (option));

  next:
    al_free (key);
    entry = al_get_next_config_entry (&iterator);
  }

  al_destroy_config (config);

  if (ret_file_type) *ret_file_type = file_type;

  if (file_type != CONFIG_FILE_TYPE && file_type != GAME_SAVE_FILE_TYPE) {
    free_argv (cargc, cargv);
    return EINVAL;
  }

  return 0;
}

void
free_argv (size_t *cargc, char ***cargv)
{
  assert (cargc);
  assert (cargv);

  size_t i;
  for (i = 0; i < *cargc; i++) al_free ((*cargv)[i]);
  destroy_array ((void *) cargv, cargc);
}

int
main (int _argc, char **_argv)
{
  /* CRITICAL SECTION START: TERMINATION SIGNALS BLOCKED */
  sigset_t intmask;
  sigemptyset (&intmask);
  sigaddset (&intmask, SIGINT);
#ifdef SIGHUP
  sigaddset (&intmask, SIGHUP);
#endif
  sigaddset (&intmask, SIGTERM);
  sigprocmask (SIG_BLOCK, &intmask, NULL);

  /* Init global level structure */
  new_level (&global_level, 1, 1, 1);

  set_string_var (&audio_mode, "SBLAST");
  set_string_var (&env_mode, "DUNGEON");
  set_string_var (&hue_mode, "NONE");

  struct config_info config_info;

  /* glob command line for MinGW */
#if WINDOWS_PORT
  glob_t gl; memset (&gl, 0, sizeof (gl));
  size_t i;
  for (i = 0; i < _argc; i++) {
    repl_str_char (_argv[i], ALLEGRO_NATIVE_PATH_SEP, '/');
    glob (_argv[i], GLOB_APPEND | GLOB_NOCHECK | GLOB_NOESCAPE
          | GLOB_PERIOD | GLOB_BRACE | GLOB_NOMAGIC | GLOB_TILDE,
          NULL, &gl);
  }
  argc = gl.gl_pathc;
  argv = gl.gl_pathv;
#else
  /* make command-line arguments available globally */
  argc = _argc;
  argv = _argv;
#endif

  set_program_name (argv[0]);

  /* initialize Allegro */
  al_init ();

  /* create primary event queue */
  event_queue = al_create_event_queue ();

  /* get global paths */
  get_paths ();

  /* ensure basic path structure */
  al_make_directory (user_data_dir);
  al_make_directory (user_settings_dir);

  /* load replay favorites */
  ui_load_replay_favorites ();

  /* get configuration file arguments */
  enum file_type file_type = UNKNOWN_FILE_TYPE;
  int e = get_config_args (&cargc, &cargv, options, config_filename,
                           &file_type);
  if (e && e != ENOENT)
    error (0, e, "can't load %s '%s'", file_type2str (file_type),
           config_filename);

  /* get environment variable arguments */
  get_env_args (&eargc, &eargv, options);

  /* size_t i; */
  /* for (i = 0; i < cargc; i++) printf ("%s\n", cargv[i]); */
  /* exit (0); */

  argp_program_version_hook = version;

  /* pre parser */
  config_info.type = CI_COMMAND_LINE;
  argp_parse (&pre_argp, argc, argv, 0, NULL, &config_info);

  /* parser */
  if (! ignore_main_config && cargc && cargv) {
    config_info.type = CI_CONFIGURATION_FILE;
    config_info.filename = config_filename;
    argp_parse (&argp, cargc, cargv, 0, NULL, &config_info);
    free_argv (&cargc, &cargv);
  }
  if (! ignore_environment && eargc && eargv) {
    config_info.type = CI_ENVIRONMENT_VARIABLES;
    argp_parse (&argp, eargc, eargv, 0, NULL, &config_info);
    free_argv (&eargc, &eargv);
  }
  config_info.type = CI_COMMAND_LINE;
  argp_parse (&argp, argc, argv, 0, NULL, &config_info);

  if (replay_info) {
    if (replay_chain_nmemb == 0)
      error (-1, 0, "empty replay chain");
    print_replay_chain_info ();
    exit (0);
  }

  init_dialog ();
  init_video ();
  init_audio ();
  init_gamepad ();
  init_mouse ();

  al_inhibit_screensaver (inhibit_screensaver);

  if (is_fullscreen ()) al_hide_mouse_cursor (display);
  else al_show_mouse_cursor (display);

  al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_BUSY);

  load_callback = process_display_events;
  show ();

  /* initialize scripting environment */
  init_script ();

  /* load assets */
  run_lua_hook (main_L, "load_hook");

  /* setup video mode */
  setup_video_mode (video_mode);

  init_gui_menu ();

  load_oitofelix_face ();
  load_audio_data ();
  load_level ();
  load_cutscenes ();

  load_callback = NULL;

  show ();

  al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);

  /* ----------------- */
  /* save_guard_bitmaps (VGA); */
  /* exit (0); */
  /* --------------- */

  give_dat_compat_preference ();

  main_menu_enabled = true;
  main_menu ();

  /* Initialize GUI */
  init_gui (_argc, _argv);

  /* register exit cleanup function */
  atexit (quit_game);
  signal (SIGINT, quit_game_sighandler);
#ifdef SIGHUP
  signal (SIGHUP, quit_game_sighandler);
#endif
  signal (SIGTERM, quit_game_sighandler);

  /* CRITICAL SECTION END: TERMINATION SIGNALS UNBLOCKED */
  sigprocmask (SIG_UNBLOCK, &intmask, NULL);

  if (skip_title) goto play_game;

 restart_game:
  play_time = 0;
  cutscene_mode (true);
  al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
  clear_bitmap (cutscene_screen, BLACK);
  clear_bitmap (uscreen, TRANSPARENT_COLOR);
  cutscene_started = false;
  stop_audio_instances ();
  stop_video_effect ();

  /* /\* begin test *\/ */
  /* cutscene_mode (true); */
  /* play_anim (cutscene_out_of_time_anim, NULL); */
  /* exit (0); */
  /* /\* end test *\/ */

  play_title ();

  stop_audio_instances ();
  stop_video_effect ();
  if (quit_anim == QUIT_GAME) exit (0);
  else if (quit_anim == RESTART_GAME) goto restart_game;
  else if (quit_anim != CUTSCENE_END) goto play_game;

  int min_legacy_level_bkp = min_legacy_level;
  int max_legacy_level_bkp = max_legacy_level;
  free_replay_chain ();
  struct replay *replay_ptr = (struct replay *)
    load_resource ("data/replays/title.mrp", (load_resource_f) xload_replay,
                   true);
  if (replay_ptr) {
    level_start_replay_mode = PLAY_REPLAY;
    struct replay *replay = &replay_ptr[0];
    min_legacy_level = min_int (min_legacy_level, replay->start_level);
    max_legacy_level = max_int (max_legacy_level, replay->start_level);
    if (! next_legacy_level (&vanilla_level, replay->start_level))
      exit (-1);
    title_demo = true;
    play_level (&vanilla_level);
    title_demo = false;
  }
  min_legacy_level = min_legacy_level_bkp;
  max_legacy_level = max_legacy_level_bkp;

  stop_audio_instances ();
  stop_video_effect ();
  if (quit_anim == QUIT_GAME) exit (0);
  else if (quit_anim == RESTART_GAME) goto restart_game;
  else if (quit_anim != CUTSCENE_END) goto play_game;

  goto restart_game;

 play_game:
  min_legacy_level = min_int (min_legacy_level, start_level);
  max_legacy_level = max_int (max_legacy_level, start_level);

  if (play_game_counter > 0) {
    start_time = START_TIME;
    time_limit = TIME_LIMIT;
    start_level = 1;
    initial_total_hp = KID_INITIAL_TOTAL_HP;
    initial_current_hp = KID_INITIAL_CURRENT_HP;
    skill.counter_attack_prob = INITIAL_KCA;
    skill.counter_defense_prob = INITIAL_KCD;
  }

  total_hp = initial_total_hp;
  current_hp = initial_current_hp;
  start_level_time = start_time;
  cutscene_mode (false);
  game_paused = false;

  if (start_replay_favorite >= 0)
    ui_go_to_replay_favorite (start_replay_favorite);

  int level = next_level_number >= 0 ? next_level_number : start_level;
  if (! level_module_next_level (&vanilla_level, level))
    exit (-1);
  play_level (&vanilla_level);

  if (quit_anim == LOAD_GAME) goto play_game;
  else if (quit_anim == RESTART_GAME) {
    play_game_counter++;
    goto restart_game;
  }

  return EXIT_SUCCESS;
}

void
quit_game (void)
{
  finalize_script ();
  finalize_gui ();
  finalize_gui_menu ();

  unload_level ();
  unload_cutscenes ();
  unload_audio_data ();
  unload_oitofelix_face ();

  finalize_mouse ();
  finalize_gamepad ();
  finalize_audio ();
  finalize_dialog ();
  finalize_video ();

  destroy_level (&global_level);

  if (scream && kid_scream && guard_scream
      && fat_scream && shadow_scream && skeleton_scream
      && vizier_scream  && princess_scream && mouse_scream)
    fprintf (stderr, "In MININIM, everybody screams! ;)\n");
  else if (exit_code != EXIT_SUCCESS)
    error (0, 0, "MININIM: exiting with error! :(");
  else fprintf (stderr, "MININIM: Hope you enjoyed it! :)\n");
}

void
quit_game_sighandler (int signum)
{
  if (quitting_in_progress) raise (signum);
  quitting_in_progress = 1;
  quit_game ();
  signal (signum, SIG_DFL);
  raise (signum);
}

void
give_dat_compat_preference (void)
{
  if (al_filename_exists (levels_dat_compat_filename)
      && ! level_module_given) {
    level_module = DAT_LEVEL_MODULE;
    levels_dat_filename = levels_dat_compat_filename;
  }
}

void
get_paths (void)
{
  /* get resources path string */
  ALLEGRO_PATH *resources_path = al_get_standard_path (ALLEGRO_RESOURCES_PATH);
  resources_dir = (char *) al_path_cstr (resources_path, ALLEGRO_NATIVE_PATH_SEP);
  resources_dir = xasprintf ("%s", resources_dir);
  al_destroy_path (resources_path);

  /* get temp path string */
  ALLEGRO_PATH *temp_path = al_get_standard_path (ALLEGRO_TEMP_PATH);
  temp_dir = (char *) al_path_cstr (temp_path, ALLEGRO_NATIVE_PATH_SEP);
  temp_dir = xasprintf ("%s", temp_dir);
  al_destroy_path (temp_path);

  /* get user home path string */
  ALLEGRO_PATH *user_home_path = al_get_standard_path (ALLEGRO_USER_HOME_PATH);
  user_home_dir = (char *) al_path_cstr (user_home_path, ALLEGRO_NATIVE_PATH_SEP);
  user_home_dir = xasprintf ("%s", user_home_dir);
  al_destroy_path (user_home_path);

  /* get user documents path string */
  ALLEGRO_PATH *user_documents_path = al_get_standard_path (ALLEGRO_USER_DOCUMENTS_PATH);
  user_documents_dir = (char *) al_path_cstr (user_documents_path, ALLEGRO_NATIVE_PATH_SEP);
  user_documents_dir = xasprintf ("%s", user_documents_dir);
  al_destroy_path (user_documents_path);

  /* get user data path string */
  ALLEGRO_PATH *user_data_path = al_get_standard_path (ALLEGRO_USER_DATA_PATH);
  user_data_dir = (char *) al_path_cstr (user_data_path, ALLEGRO_NATIVE_PATH_SEP);
  user_data_dir = xasprintf ("%s", user_data_dir);
  al_destroy_path (user_data_path);

  /* get user settings path string */
  ALLEGRO_PATH *user_settings_path = al_get_standard_path (ALLEGRO_USER_SETTINGS_PATH);
  user_settings_dir = (char *) al_path_cstr (user_settings_path, ALLEGRO_NATIVE_PATH_SEP);
  user_settings_dir = xasprintf ("%s", user_settings_dir);
  al_destroy_path (user_settings_path);

  /* system settings path string */
  system_data_dir = xasprintf ("%s%c", PKGDATADIR, ALLEGRO_NATIVE_PATH_SEP);

  /* get executable file name */
  ALLEGRO_PATH *exename_path = al_get_standard_path (ALLEGRO_EXENAME_PATH);
  exe_filename = (char *) al_path_cstr (exename_path, ALLEGRO_NATIVE_PATH_SEP);
  exe_filename = xasprintf ("%s", exe_filename);
  al_destroy_path (exename_path);

  /* get config file name */
  config_filename = xasprintf ("%smininim.mcf", user_settings_dir);

  /* get history file name */
  history_filename = xasprintf ("%shistory", user_settings_dir);

  /* get legacy LEVELS.DAT compatibility path */
  levels_dat_compat_filename = xasprintf ("LEVELS.DAT");

  /* get dialogs initial paths */
  load_config_dialog.initial_path = xasprintf ("%s", user_settings_dir);
  save_game_dialog.initial_path = xasprintf ("%s.msv", user_settings_dir);
  save_picture_dialog.initial_path = xasprintf ("%s.png", user_documents_dir);
  save_replay_dialog.initial_path = xasprintf ("%s.mrp", user_data_dir);
  load_replay_dialog.initial_path = xasprintf ("%s", user_data_dir);
}

void
print_paths (void)
{
  printf ("Main configuration file: %s\n", config_filename);
  printf ("REPL history file: %s\n", history_filename);
  printf ("Executable file: %s\n", exe_filename);
  printf ("Resources: %s\n", resources_dir);
  printf ("System data: %s\n", system_data_dir);
  printf ("Temporary: %s\n", temp_dir);
  printf ("User home: %s\n", user_home_dir);
  printf ("User documents: %s\n", user_documents_dir);
  printf ("User data: %s\n", user_data_dir);
  printf ("User settings: %s\n", user_settings_dir);
}

void
print_display_modes (void)
{
  printf ("Display Modes:\n");
  printf ("\t-1: desktop settings\n");
  int i, n = al_get_num_display_modes ();
  for (i = 0; i < n ; i++) {
    ALLEGRO_DISPLAY_MODE d; al_get_display_mode (i, &d);
    printf ("\t% 2i: %ix%i %iHz\n", i, d.width, d.height, d.refresh_rate);
  }
}

void *
dialog_thread (ALLEGRO_THREAD *thread, void *arg)
{
  struct dialog *d = arg;
  main_menu_enabled = false;
  al_show_mouse_cursor (display);

  ALLEGRO_FILECHOOSER *dialog = NULL;

  do {
    if (dialog) al_destroy_native_file_dialog (dialog);

    dialog = al_create_native_file_dialog (d->initial_path, d->title,
                                           d->patterns, d->mode);

    al_show_native_file_dialog (display, dialog);
  } while (d->mode & ALLEGRO_FILECHOOSER_SAVE
           && al_get_native_file_dialog_count (dialog)
           && ! file_overwrite_dialog
           ((char *) al_get_native_file_dialog_path (dialog, 0)));

  al_set_thread_should_stop (thread);

  if (is_fullscreen () && ! is_showing_menu ())
    al_hide_mouse_cursor (display);
  else al_show_mouse_cursor (display);

  main_menu_enabled = true;

  return dialog;
}

void *
message_box_thread (ALLEGRO_THREAD *thread, void *arg)
{
  struct message_box *d = arg;
  al_show_mouse_cursor (display);

  al_show_native_message_box
    (display, d->title, d->heading, d->text, d->buttons, d->flags);

  al_set_thread_should_stop (thread);

  if (is_fullscreen () && ! is_showing_menu ())
    al_hide_mouse_cursor (display);
  else al_show_mouse_cursor (display);

  return NULL;
}

bool
load_config (char *filename, enum file_type *ret_file_type, int priority)
{
  char **cargv = NULL;
  size_t cargc = 0;
  struct config_info config_info;

  enum file_type file_type = UNKNOWN_FILE_TYPE;
  if (get_config_args (&cargc, &cargv, options, filename, &file_type)) {
    print_text_log ("cannot load %s '%s'\n", file_type2str (file_type),
                    filename);
    if (ret_file_type) *ret_file_type = file_type;
    return false;
  }

  if (ret_file_type) *ret_file_type = file_type;

  config_info.type = CI_CONFIGURATION_FILE;
  config_info.filename = filename;

  bool parse_error = false;
  int last_arg_index, arg_index = -1;
  error_t e;

  do {
    last_arg_index = ++arg_index;
    e = argp_parse (&argp, cargc - arg_index, &cargv[arg_index],
                    ARGP_SILENT, &arg_index, &config_info);
    if (! parse_error) parse_error = e;
  } while (e && arg_index > last_arg_index);

  free_argv (&cargc, &cargv);

  return ! parse_error;
}

bool
save_game (char *filename, int priority)
{
  ALLEGRO_CONFIG *config = al_create_config ();
  char *start_level_str, *start_time_str, *time_limit_str,
    *total_hp_str, *kca_str, *kcd_str;

  start_level_str = xasprintf ("%i", global_level.n);
  start_time_str = xasprintf ("%" PRIu64, start_level_time);
  time_limit_str = xasprintf ("%" PRIu64, time_limit);
  total_hp_str = xasprintf ("%i", total_hp);
  kca_str = xasprintf ("%i", skill.counter_attack_prob + 1);
  kcd_str = xasprintf ("%i", skill.counter_defense_prob + 1);

  al_set_config_value (config, NULL, "FILE TYPE", "MININIM GAME SAVE");
  al_set_config_value (config, NULL, "START LEVEL", start_level_str);
  al_set_config_value (config, NULL, "START TIME", start_time_str);
  al_set_config_value (config, NULL, "TIME LIMIT", time_limit_str);
  al_set_config_value (config, NULL, "TOTAL HP", total_hp_str);
  al_set_config_value (config, NULL, "KCA", kca_str);
  al_set_config_value (config, NULL, "KCD", kcd_str);

  bool success = al_save_config_file (filename, config);

  char *error_str = success
    ? "GAME SAVED"
    : "GAME SAVING FAILED";

  ui_msg (priority, "%s", error_str);

  al_destroy_config (config);
  al_free (start_level_str);
  al_free (start_time_str);
  al_free (time_limit_str);
  al_free (total_hp_str);
  al_free (kca_str);
  al_free (kcd_str);

  return success;
}

void
handle_load_config_thread (int priority)
{
  if (! load_config_dialog_thread
      || ! al_get_thread_should_stop (load_config_dialog_thread))
    return;

  ALLEGRO_FILECHOOSER *dialog;
  al_join_thread (load_config_dialog_thread, (void *) &dialog);
  al_destroy_thread (load_config_dialog_thread);
  load_config_dialog_thread = NULL;
  size_t i;
  size_t n = al_get_native_file_dialog_count (dialog);

  if (n > 0) {
    bool success = true;
    enum file_type file_type = UNKNOWN_FILE_TYPE;
    for (i = 0; i < n; i++) {
      char *filename = (char *) al_get_native_file_dialog_path (dialog, i);

      if (! load_config (filename, &file_type, priority))
        success = false;

      if (file_type == GAME_SAVE_FILE_TYPE) break;
    }

    char *error_str;
    if (n == 1
        && file_type != CONFIG_FILE_TYPE
        && file_type != GAME_SAVE_FILE_TYPE)
      error_str = "UNKNOWN FILE TYPE";
    else {
      if (success)
        error_str = file_type == GAME_SAVE_FILE_TYPE
          ? "GAME LOADED"
          : "CONFIGURATION LOADED";
      else
        error_str = file_type == GAME_SAVE_FILE_TYPE
          ? "GAME LOADED WITH ERRORS"
          : "CONFIGURATION LOADED WITH ERRORS";
    }

    al_free (load_config_dialog.initial_path);
    load_config_dialog.initial_path = xasprintf ("%s",
               al_get_native_file_dialog_path (dialog, n - 1));

    ui_msg (priority, "%s", error_str);

    if (file_type == GAME_SAVE_FILE_TYPE) {
      play_game_counter = 0;
      next_level_number = -1;
      quit_anim = LOAD_GAME;
    }
  }

  al_destroy_native_file_dialog (dialog);
}

void
handle_save_game_thread (int priority)
{
  if (! save_game_dialog_thread
      || ! al_get_thread_should_stop (save_game_dialog_thread))
    return;

  ALLEGRO_FILECHOOSER *dialog;
  al_join_thread (save_game_dialog_thread, (void *) &dialog);
  al_destroy_thread (save_game_dialog_thread);
  save_game_dialog_thread = NULL;
  char *filename = al_get_native_file_dialog_count (dialog) > 0
    ? (char *) al_get_native_file_dialog_path (dialog, 0)
    : NULL;
  if (filename) {
    save_game (filename, priority);
    al_free (save_game_dialog.initial_path);
    save_game_dialog.initial_path = xasprintf ("%s", filename);
  }
  al_destroy_native_file_dialog (dialog);
  pause_animation (false);
}

void
handle_save_picture_thread (int priority)
{
  if (! save_picture_dialog_thread
      || ! al_get_thread_should_stop (save_picture_dialog_thread))
    return;

  ALLEGRO_FILECHOOSER *dialog;
  al_join_thread (save_picture_dialog_thread, (void *) &dialog);
  al_destroy_thread (save_picture_dialog_thread);
  save_picture_dialog_thread = NULL;
  char *filename = al_get_native_file_dialog_count (dialog) > 0
    ? (char *) al_get_native_file_dialog_path (dialog, 0)
    : NULL;
  if (filename) {
    char *error_str = al_save_bitmap
      (filename, al_get_backbuffer (display))
      ? "PICTURE SAVED"
      : "PICTURE SAVING FAILED";
    ui_msg (priority, "%s", error_str);
    al_free (save_picture_dialog.initial_path);
    save_picture_dialog.initial_path = xasprintf ("%s", filename);
  }
  al_destroy_native_file_dialog (dialog);
  pause_animation (false);
}

void
handle_message_box_thread (void)
{
  if (! message_box_thread_id
      || ! al_get_thread_should_stop (message_box_thread_id))
    return;

  al_join_thread (message_box_thread_id, NULL);
  al_destroy_thread (message_box_thread_id);
  message_box_thread_id = NULL;
}

struct level *
level_module_next_level (struct level *l, int n)
{
  switch (level_module) {
  case NATIVE_LEVEL_MODULE: default:
    return next_native_level (l, n);
    break;
  case LEGACY_LEVEL_MODULE:
    return next_legacy_level (l, n);
    break;
  case PLV_LEVEL_MODULE:
    return next_plv_level (l, n);
    break;
  case DAT_LEVEL_MODULE:
    return next_dat_level (l, n);
    break;
  case CONSISTENCY_LEVEL_MODULE:
    return next_consistency_level (l, n);
    break;
  }
}

char *
level_module_str (enum level_module m)
{
  switch (m) {
  case NATIVE_LEVEL_MODULE: default:
    return "NATIVE";
  case LEGACY_LEVEL_MODULE:
    return "LEGACY";
  case PLV_LEVEL_MODULE:
    return "PLV";
  case DAT_LEVEL_MODULE:
    return "DAT";
  case CONSISTENCY_LEVEL_MODULE:
    return "CONSISTENCY";
  }
}

char *
movements_str (enum movements m)
{
  switch (m) {
  case LEGACY_MOVEMENTS: return "LEGACY";
  case NATIVE_MOVEMENTS: return "NATIVE";
  default: assert (false);
  }
  return NULL;
}

char *
semantics_str (enum semantics m)
{
  switch (m) {
  case LEGACY_SEMANTICS: return "LEGACY";
  case NATIVE_SEMANTICS: return "NATIVE";
  default: assert (false);
  }
  return NULL;
}

bool
save_level (struct level *l)
{
  char *f, *d;
  d = xasprintf ("%sdata/levels/", user_data_dir);
  if (! al_make_directory (d)) {
    error (0, al_get_errno (),
           "%s (%s): failed to create native level directory",
           __func__, d);
    al_free (d);
    return false;
  }
  f = xasprintf ("%s%02d.mim", d, l->n);
  if (! save_native_level (l, f)) {
    error (0, al_get_errno (),
           "%s (%s): failed to save native level file",
           __func__, f);
    al_free (f);
    al_free (d);
    return false;
  }
  return true;
}
