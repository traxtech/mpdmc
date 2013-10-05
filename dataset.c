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
#include "dataset.h"

#include <stdlib.h>
#include <string.h>

void dataset_init(dataset_t *ds) {
    ds->state = DATASET_STATE_STOPPED;
    ds->elapsed_time = 0;
    ds->total_time = 0;
    ds->song_artist = 0;
    ds->song_title = 0;
    ds->song_album = 0;
    ds->volume = 0;
}

void dataset_update(dataset_t *ds, MpdObj *mi, ChangedStatusType what) {
    if(what&MPD_CST_SONGID)
        {
                mpd_Song *song = mpd_playlist_get_current_song(mi);
                if(song)
                {
                        if(ds->song_artist)
                            free(ds->song_artist);
                        ds->song_artist = strdup(song->artist);
                        if(ds->song_title)
                            free(ds->song_title);
                        ds->song_title = strdup(song->title);
                        if(ds->song_album)
                            free(ds->song_album);
                        ds->song_album = strdup(song->album);

                }
        }

        if(what&MPD_CST_STATE)
        {
                switch(mpd_player_get_state(mi))
                {
                        case MPD_PLAYER_PLAY:
                                ds->state = DATASET_STATE_PLAYING;
                                break;
                        case MPD_PLAYER_PAUSE:
                                ds->state = DATASET_STATE_PAUSED;
                                break;
                        case MPD_PLAYER_STOP:
                                ds->state = DATASET_STATE_STOPPED;
                                break;
                        default:
                                break;
                }
        }
        if(what&MPD_CST_VOLUME){
                ds->volume = mpd_status_get_volume(mi);
        }
        if(what&MPD_CST_TOTAL_TIME){
            ds->total_time = mpd_status_get_total_song_time(mi);
        }
        if(what&MPD_CST_ELAPSED_TIME){
            ds->elapsed_time = mpd_status_get_elapsed_song_time(mi);
        }
}
