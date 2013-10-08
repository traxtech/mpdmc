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

typedef struct {
    enum state_t state;
    int elapsed_time;
    int total_time;
    char *song_artist;
    char *song_title;
    char *song_album;
    int volume;
    int db_updating;
	unsigned int samplerate;
	int bitdepth;
	int randomplayback;
	//int consumeplayback;
	int repeatplayback;
	int bitrate;
} dataset_t;

void dataset_init(dataset_t *ds);

void dataset_update(dataset_t *ds, MpdObj *mi, ChangedStatusType what);

#endif // DATASET_H
