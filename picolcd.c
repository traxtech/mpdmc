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
 *
 * Parts of this source code is from the LCD4Linux project.
 */
#include "picolcd.h"

#include <stdio.h>
#include <string.h>
#include <cairo.h>
#include <usb.h>

#define OUT_REPORT_CMD			0x94
#define OUT_REPORT_DATA			0x95
#define OUT_REPORT_CMD_DATA		0x96

static usb_dev_handle *lcd;
static cairo_surface_t *play_image;
static cairo_surface_t *pause_image;
static cairo_surface_t *stop_image;
static cairo_surface_t *database_image;
static cairo_surface_t *volctrl_image;
static cairo_surface_t *mono_image;
static cairo_surface_t *stereo_image;

static void picolcd_send(unsigned char *data, int size)
{
    int __attribute__ ((unused)) ret;
    ret = usb_interrupt_write(lcd, USB_ENDPOINT_OUT + 1, (char *) data, size, 1000);
    //fprintf(stderr, "%s written %d bytes\n", __FUNCTION__, ret);
}

void picolcd_clear(void)
{
    unsigned char cmd[3] = { 0x93, 0x01, 0x00 };	/* init display */
    unsigned char cmd2[9] = { OUT_REPORT_CMD };	/* init display */
    unsigned char cmd3[64] = { OUT_REPORT_CMD_DATA };	/* clear screen */
    unsigned char cmd4[64] = { OUT_REPORT_CMD_DATA };	/* clear screen */

    int init, index;
    unsigned char cs, line;


    picolcd_send(cmd, 3);

    for (init = 0; init < 4; init++) {
	unsigned char cs = ((init << 2) & 0xFF);

	cmd2[0] = OUT_REPORT_CMD;
	cmd2[1] = cs;
	cmd2[2] = 0x02;
	cmd2[3] = 0x00;
	cmd2[4] = 0x64;
	cmd2[5] = 0x3F;
	cmd2[6] = 0x00;
	cmd2[7] = 0x64;
	cmd2[8] = 0xC0;

	picolcd_send(cmd2, 9);
    }


    for (cs = 0; cs < 4; cs++) {
	unsigned char chipsel = (cs << 2);	//chipselect
	for (line = 0; line < 8; line++) {
	    //ha64_1.setHIDPkt(OUT_REPORT_CMD_DATA, 8+3+32, 8, cs, 0x02, 0x00, 0x00, 0xb8|j, 0x00, 0x00, 0x40);
	    cmd3[0] = OUT_REPORT_CMD_DATA;
	    cmd3[1] = chipsel;
	    cmd3[2] = 0x02;
	    cmd3[3] = 0x00;
	    cmd3[4] = 0x00;
	    cmd3[5] = 0xb8 | line;
	    cmd3[6] = 0x00;
	    cmd3[7] = 0x00;
	    cmd3[8] = 0x40;
	    cmd3[9] = 0x00;
	    cmd3[10] = 0x00;
	    cmd3[11] = 32;

	    unsigned char temp = 0;

	    for (index = 0; index < 32; index++) {
		cmd3[12 + index] = temp;
	    }

	    picolcd_send(cmd3, 64);

	    //ha64_2.setHIDPkt(OUT_REPORT_DATA, 4+32, 4, cs | 0x01, 0x00, 0x00, 32);

	    cmd4[0] = OUT_REPORT_DATA;
	    cmd4[1] = chipsel | 0x01;
	    cmd4[2] = 0x00;
	    cmd4[3] = 0x00;
	    cmd4[4] = 32;

	    for (index = 32; index < 64; index++) {
		temp = 0x00;
		cmd4[5 + (index - 32)] = temp;
	    }
	    picolcd_send(cmd4, 64);
	}
    }
}

static void picolcd_send_framebuffer(unsigned char *framebuffer)
{
    unsigned char cmd3[64] = { OUT_REPORT_CMD_DATA };	/* send command + data */
    unsigned char cmd4[64] = { OUT_REPORT_DATA };	/* send data only */

    int index, bit, x, y;
    unsigned char cs, line;
    unsigned char pixel;

    for (cs = 0; cs < 4; cs++) {
	unsigned char chipsel = (cs << 2);	//chipselect
	for (line = 0; line < 8; line++) {
	    cmd3[0] = OUT_REPORT_CMD_DATA;
	    cmd3[1] = chipsel;
	    cmd3[2] = 0x02;
	    cmd3[3] = 0x00;
	    cmd3[4] = 0x00;
	    cmd3[5] = 0xb8 | line;
	    cmd3[6] = 0x00;
	    cmd3[7] = 0x00;
	    cmd3[8] = 0x40;
	    cmd3[9] = 0x00;
	    cmd3[10] = 0x00;
	    cmd3[11] = 32;

	    cmd4[0] = OUT_REPORT_DATA;
	    cmd4[1] = chipsel | 0x01;
	    cmd4[2] = 0x00;
	    cmd4[3] = 0x00;
	    cmd4[4] = 32;

	    for (index = 0; index < 32; index++) {
		pixel = 0x00;

		for (bit = 0; bit < 8; bit++) {
		    x = cs * 64 + index;
		    y = (line * 8 + bit + 0) % 64;

		    if (framebuffer[y * 256 + x])// ^ inverted)
			pixel |= (1 << bit);
		}
		cmd3[12 + index] = pixel;
	    }

	    for (index = 32; index < 64; index++) {
		pixel = 0x00;

		for (bit = 0; bit < 8; bit++) {
		    x = cs * 64 + index;
		    y = (line * 8 + bit + 0) % 64;
		    if (framebuffer[y * 256 + x])// ^ inverted)
			pixel |= (1 << bit);
		}

		cmd4[5 + (index - 32)] = pixel;
	    }

	    picolcd_send(cmd3, 44);
	    picolcd_send(cmd4, 38);
	}
    }
}

int picolcd_init() {
    struct usb_bus *bus;
    struct usb_device *dev;
    int ret;
    char driver[1024];
    char product[1024];
    char manufacturer[1024];
    char serialnumber[1024];

    play_image = cairo_image_surface_create_from_png("icons/play.png");
    pause_image = cairo_image_surface_create_from_png("icons/pause.png");
    stop_image = cairo_image_surface_create_from_png("icons/stop.png");
    database_image = cairo_image_surface_create_from_png("icons/database.png");
    volctrl_image = cairo_image_surface_create_from_png("icons/volctrl.png");
    mono_image = cairo_image_surface_create_from_png("icons/mono.png");
    stereo_image = cairo_image_surface_create_from_png("icons/stereo.png");

    printf("PicoLCD: Scanning USB devices\n");
    usb_init();
    usb_find_busses();
    usb_find_devices();
    struct usb_bus *busses = usb_get_busses();
    for (bus = busses; bus; bus = bus->next) {
	for (dev = bus->devices; dev; dev = dev->next) {
	    if ((dev->descriptor.idVendor == 0x04d8) && (dev->descriptor.idProduct == 0xc002)) {

		printf("PicoLCD: Found picoLCD on bus %s device %s\n", bus->dirname, dev->filename);

		lcd = usb_open(dev);

		ret = usb_get_driver_np(lcd, 0, driver, sizeof(driver));

		if (ret == 0) {
		    printf("PicoLCD: Interface 0 already claimed by '%s', trying to detach driver\n", driver);
		    if (usb_detach_kernel_driver_np(lcd, 0) < 0) {
			printf("PicoLCD: Failed to detach interface 0\n");
			return 0;
		    }
		}

		usb_set_configuration(lcd, 1);
		usleep(100);

		if (usb_claim_interface(lcd, 0) < 0) {
		    printf("PicoLCD: Failed to claim interface\n");
		    return 0;
		}

		usb_set_altinterface(lcd, 0);

		usb_get_string_simple(lcd, dev->descriptor.iProduct, product, sizeof(product));
		usb_get_string_simple(lcd, dev->descriptor.iManufacturer, manufacturer, sizeof(manufacturer));
		usb_get_string_simple(lcd, dev->descriptor.iSerialNumber, serialnumber, sizeof(serialnumber));

		printf("PicoLCD: Manufacturer='%s' Product='%s' SerialNumber='%s'\n", manufacturer, product, serialnumber);

                picolcd_clear();
		return 1;
	    }
	}
    }
    printf("PicoLCD: Device not found\n");
    return 0;
}

void picolcd_update(dataset_t *ds) {
    char buffer[100];
    cairo_surface_t *surface = cairo_image_surface_create (CAIRO_FORMAT_A8, 256, 64);
    cairo_t *cr = cairo_create (surface);
    cairo_set_antialias (cr, CAIRO_ANTIALIAS_NONE);
    //cairo_set_font_antialias (cr, CAIRO_ANTIALIAS_NONE);
    cairo_font_options_t* font_options =  cairo_font_options_create();
    cairo_font_options_set_antialias(font_options, CAIRO_ANTIALIAS_NONE);
    cairo_set_font_options (cr, font_options);
    //
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_select_font_face (cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size (cr, 16);
    cairo_set_source_rgb (cr, 1, 1, 1);

    if(ds->state == DATASET_STATE_PLAYING) {
        cairo_save(cr);
        cairo_set_source_surface (cr, play_image, 0, 0);
        cairo_paint (cr);
        cairo_restore(cr);
    } else if(ds->state == DATASET_STATE_PAUSED) {
        cairo_save(cr);
        cairo_set_source_surface (cr, pause_image, 0, 0);
        cairo_paint (cr);
        cairo_restore(cr);
    } else if(ds->state == DATASET_STATE_STOPPED) {
        cairo_save(cr);
        cairo_set_source_surface (cr, stop_image, 0, 0);
        cairo_paint (cr);
        cairo_restore(cr);
    }

    if(ds->channels == 1) {
        cairo_save(cr);
        cairo_set_source_surface (cr, mono_image, 208, 0);
        cairo_paint (cr);
        cairo_restore(cr);        
    } else if(ds->channels == 2) {
        cairo_save(cr);
        cairo_set_source_surface (cr, stereo_image, 208, 0);
        cairo_paint (cr);
        cairo_restore(cr);        
    }


    if(ds->db_updating) {
        cairo_save(cr);
        cairo_set_source_surface (cr, database_image, 224, 0);
        cairo_paint (cr);
        cairo_restore(cr);        
    }
    if(ds->volume >= 0) {
        cairo_save(cr);
        cairo_set_source_surface (cr, volctrl_image, 240, 0);
        cairo_paint (cr);
        cairo_restore(cr);        
    }

            cairo_move_to (cr, 20, 16);
            snprintf(buffer, sizeof(buffer), "%02i:%02i / %02i:%02i",
                                ds->elapsed_time/60,
                                ds->elapsed_time%60,
                                ds->total_time / 60,
                                ds->total_time % 60);
            cairo_show_text (cr, buffer);
		
            cairo_select_font_face (cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
            cairo_set_font_size (cr, 16);
            
            cairo_move_to (cr, 2, 30);
            cairo_show_text (cr, ds->song_title);
            cairo_move_to (cr, 2, 46);
            cairo_show_text (cr, ds->song_album);
            cairo_move_to (cr, 2, 62);
            cairo_show_text (cr, ds->song_artist);
            //cairo_surface_write_to_png (surface, "picolcd.png");
            
            cairo_destroy (cr);
            picolcd_send_framebuffer(cairo_image_surface_get_data(surface));
            cairo_surface_destroy (surface);
    //printf("bitrate=%dkbs samplerate=%dbps\n", ds->bitrate, ds->samplerate);
}

void picolcd_close() {
    picolcd_clear();
    usb_release_interface(lcd, 0);
    usb_close(lcd);
}
