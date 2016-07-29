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
	set_backlight(1800);
	return 0;
}
