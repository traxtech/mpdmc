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

#ifdef WITH_PICOLCD
#include "picolcd.h"
#endif // WITH_PICOLCD
#ifdef WITH_MOSQUITTO
#include "mosquitto.h"
#endif // WITH_MOSQUITTO

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

    /* Get MPD configuration */
    char *mpd_host = getenv("MPDMC_MPD_HOST");
    if(mpd_host == NULL)
    {
        mpd_host = "localhost";
    }
    char *mpd_port = getenv("MPDMC_MPD_PORT");
    if(mpd_port == NULL)
    {
        mpd_port = "6600";
    } else if (atoi(mpd_port) <= 0) {
        printf("Invalid MPD port '%s'\n", mpd_port);
        return 1;
    }
    char *mpd_password = getenv("MPDMC_MPD_PASSWORD");
    if(mpd_password == NULL)
    {
        mpd_password = "";
    }
    /* Allocate the dataset */
    dataset = malloc(sizeof(dataset_t));
    if(!dataset) {
        printf("Failed to allocate memory\n");
        return 1;
    }
    dataset_init(dataset);
    /* Initialize the modules */
    #ifdef WITH_PICOLCD
    if(!picolcd_init()) {
        printf("Failed to init the PicoLCD module\n");
        return 1;
    }
    #endif // WITH_PICOLCD
    #ifdef WITH_MOSQUITTO
    if(!mosquitto_init()) {
        printf("Failed to init the Mosquitto module\n");
        return 1;
    }
    #endif // WITH_MOSQUITTO

    /* Connect to MPD */
    printf("Connecting to MPD host='%s' port='%d', password='%s'\n", mpd_host, atoi(mpd_port), mpd_password);
    MpdObj *obj = mpd_new(mpd_host, atoi(mpd_port), mpd_password);
    mpd_signal_connect_error(obj,(ErrorCallback)error_callback, 0);
    mpd_signal_connect_status_changed(obj,(StatusChangedCallback)status_changed_callback, 0);
    /* Proces MPD events */
    if(!mpd_connect(obj))
    {
        printf("Connected to MPD, sending password\n");
        mpd_send_password(obj);
	printf("Password sent, entering main event loop\n");
        while(!usleep(100000))
        {
            mpd_status_update(obj);
            #ifdef WITH_PICOLCD
            picolcd_update(dataset);
            #endif // WITH_PICOLCD
            #ifdef WITH_MOSQUITTO
            mosquitto_update(dataset);
            #endif // WITH_MOSQUITTO
        }
    } else {
        printf("Failed to connect to MPD\n");
    }
    mpd_free(obj);
    #ifdef WITH_PICOLCD
    picolcd_close();
    #endif // WITH_PICOLCD
    #ifdef WITH_MOSQUITTO
    mosquitto_close();
    #endif // WITH_MOSQUITTO
    return 0;
}

