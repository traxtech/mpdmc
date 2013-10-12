/*
 * Copyright (C) 2013 Arnaud Rolly <github@gedial.com>
 *
 * This file is part of mpdmc.
 *
 * Mpdmc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mpdmc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Mpdmc.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef DATASET_H
#define DATASET_H

#include <libmpd/libmpd.h>

enum state_t { DATASET_STATE_STOPPED = 0, DATASET_STATE_PLAYING = 1, DATASET_STATE_PAUSED = 2};

enum playback_t {DATASET_PLAYBACK_UNKNOWN = 0, DATASET_PLAYBACK_CONSUME = 1, DATASET_PLAYBACK_SINGLE = 2, DATASET_PLAYBACK_REPEAT = 3, DATASET_PLAYBACK_RANDOM = 2};

typedef struct {
    enum state_t state;
    int elapsed_time;
    int total_time;
    char *song_artist;
    char *song_title;
    char *song_album;
    int volume;
    int db_updating;
    enum playback_t playback;
    int bitrate; // bitrate of the current playing song in kbs. This is a constantly updating value. (for vbr songs)
    int channels; // number of channels in the current playing song. This is usually only 1(mono) or 2(stereo)
    unsigned int samplerate; // samplerate of the current playing song in bps.
} dataset_t;

void dataset_init(dataset_t *ds);

void dataset_update(dataset_t *ds, MpdObj *mi, ChangedStatusType what);

#endif // DATASET_H
