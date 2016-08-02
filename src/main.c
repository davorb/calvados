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
#include <syslog.h>
#include <unistd.h>
#include <sys/types.h>

#define LIGHT_SENSOR_PATH "/sys/devices/platform/applesmc.768/light"
#define BACKLIGHT_PATH "/sys/class/backlight/intel_backlight/brightness"
#define MAX_BRIGHTNESS 1808
#define MIN_BRIGHTNESS 155
#define MAX_LIGHT_SENSOR_LEVEL 255.0f
#define SLEEP_TIME 15

int desired_backlight_val(int sensor_val)
{
	float sensor_percentage = (float)sensor_val / MAX_LIGHT_SENSOR_LEVEL * 2.3f;

#ifdef DEBUG
	syslog(LOG_DEBUG,
		   "sensor val: %i, percentage: %f\n",
		   sensor_val,
		   sensor_percentage);
#endif

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
	syslog(LOG_ERR, "Failed to read the light sensor value.\n");
	exit(EXIT_FAILURE);
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
		syslog(LOG_ERR, "Unable to find backlight ACPI.\n");
		exit(EXIT_FAILURE);
	}

	fprintf(fp, "%i", level);

	fclose(fp);
}

void check_and_set_brightness()
{
	int light_level = sensor_value();
	int new_brightness = desired_backlight_val(light_level);
#ifdef DEBUG
	syslog(LOG_DEBUG, "current light sensor level: %i\n", sensor_value());
	syslog(LOG_DEBUG, "setting brightness to: %i\n", new_brightness);
#endif

	set_backlight(new_brightness);
}

void launch_daemon() {
	pid_t pid;
	pid = fork();

	if (pid < 0) {				/* fork error */
		exit(EXIT_FAILURE);
	} else if (pid > 0) {		/* parent exits */
		exit(EXIT_SUCCESS);
	}

	if (setsid() < 0) {			/* child process becomes session leader */
		exit(EXIT_FAILURE);
	}

	openlog(NULL, LOG_CONS, LOG_USER);

	for (;;) {
		check_and_set_brightness();
		sleep(SLEEP_TIME);
	}
	closelog();
}

int main()
{
	launch_daemon();
	return EXIT_SUCCESS;
}
