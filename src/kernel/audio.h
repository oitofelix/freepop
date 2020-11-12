/*
  audio.h -- audio module;

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

#ifndef MININIM_AUDIO_H
#define MININIM_AUDIO_H

/* functions */
void init_audio (void);
void finalize_audio (void);
void set_audio_volume (float volume);
bool audio_source_eq (struct audio_source *as0, struct audio_source *as1);
struct audio_source *load_audio (struct audio_source *as,
                                 enum audio_type, const char *filename);
union audio_instance_data play_audio (struct audio_source *as,
                                      struct pos *p, int anim_id);
int compare_audio_instances (const void *_ai0, const void *_ai1);
struct audio_instance *get_audio_instance (union audio_instance_data data);
struct audio_instance *search_audio_instance (struct audio_source *as,
                                              uint64_t anim_cycle,
                                              struct pos *p, int anim_id);
void play_audio_instances (void);
bool pause_audio_instance (union audio_instance_data data, bool val);
void pause_audio_instances (bool val);
void clear_played_audio_instances (void);
double get_audio_instance_position (union audio_instance_data data);
bool is_audio_instance_playing (union audio_instance_data data);
bool is_audio_instance_paused (union audio_instance_data data);
struct audio_instance *is_audio_source_playing (struct audio_source *as);
void destroy_audio (struct audio_source *as);
void remove_audio_instance (struct audio_instance *ai);
void adjust_audio_instances_volume (void);
float get_adjusted_audio_instance_volume (struct audio_instance *ai);
void stop_audio_instances (void);
bool stop_audio_instance (struct audio_source *as, struct pos *p, int anim_id);
bool is_instance_of_audio_source (union audio_instance_data data,
                                  struct audio_source *as);
bool is_audio_rendering ();

/* variables */
extern float audio_volume;

#endif	/* MININIM_AUDIO_H */
