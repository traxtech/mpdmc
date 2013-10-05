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
#include <libmpd/libmpd.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "dataset.h"

static dataset_t *dataset;

/*
 * Callback for MPD errors.
 */
void error_callback(MpdObj *mi,int errorid, char *msg, void *userdata)
{
        fprintf(stderr, "MPD error %i:'%s'\n", errorid, msg);
}

void status_changed_callback(MpdObj *mi, ChangedStatusType what)
{
    dataset_update(dataset, mi, what);
}

/*
 * Callback for MPD status change
 */


int main(int argc, char **argv)
{
    dataset = malloc(sizeof(dataset_t));
    if(!dataset) {
        fprintf(stderr, "Failed to allocate memory\n");
        return 1;
    }
    dataset_init(dataset);

    if(!picolcd_init()) {
        fprintf(stderr, "Failed to init PicoLCD\n");
        return 2;
    }

    /* Connect to mpd */
    MpdObj *obj = mpd_new("localhost", 6600, ""); // get from mpd.conf passed as argument or shell variables ?
    mpd_signal_connect_error(obj,(ErrorCallback)error_callback, 0);
    mpd_signal_connect_status_changed(obj,(StatusChangedCallback)status_changed_callback, 0);
    

    if(!mpd_connect(obj))
    {
        mpd_send_password(obj);

        while(!usleep(100000))
        {
            mpd_status_update(obj);
            picolcd_update(dataset);
        }
        
    }
    mpd_free(obj);
    picolcd_close();
    return 0;
}

