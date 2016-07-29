/* calvados -- daemon for automatically setting laptop backlight levels */
/* Copyright (C) 2016 Davor Babić <davor@davor.se> */

/* This program is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include <stdlib.h>
#include <stdio.h>

#define LIGHT_SENSOR_PATH "/sys/devices/platform/applesmc.768/light"
#define BACKLIGHT_PATH "/sys/class/backlight/intel_backlight/brightness"
#define MAX_BRIGHTNESS 1808
#define MIN_BRIGHTNESS 255
#define MAX_LIGHT_SENSOR_LEVEL 255.0f

int desired_backlight_val(int sensor_val)
{
	float sensor_percentage = sensor_val / MAX_LIGHT_SENSOR_LEVEL;
	printf("sensor val: %i, percentage: %f\n", sensor_val,
		   sensor_percentage);
	int val = (int)(MAX_BRIGHTNESS * sensor_percentage);
	if (val < MIN_BRIGHTNESS) {
		val = MIN_BRIGHTNESS;
	}
	return val;
}

int max_brightness()
{
	return MAX_BRIGHTNESS;
}

void sensor_err_msg()
{
	fprintf(stderr, "Failed to read the light sensor value.\n");
	exit(1);
}

int sensor_value()
{
	FILE *fp;
	if ((fp = fopen(LIGHT_SENSOR_PATH, "r")) == NULL) {
		sensor_err_msg();
	}

	char line[16];
	fgets(line, 16, fp);
	if (line == NULL) {
		sensor_err_msg();
	}
	int result = atoi(line+1);
	fclose(fp);
	return result;
}

void set_backlight(int level)
{
	FILE *fp;
	if ((fp = fopen(BACKLIGHT_PATH, "w")) == NULL) {
		fprintf(stderr, "Unable to find backlight ACPI.\n");
		exit(1);
	}

	fprintf(fp, "%i", level);

	fclose(fp);
}

int main()
{
	printf("current light sensor level: %i\n", sensor_value());
	int light_level = sensor_value();
	int new_brightness = desired_backlight_val(light_level);
	printf("setting brightness to: %i\n", new_brightness);
	set_backlight(new_brightness);
	return 0;
}
