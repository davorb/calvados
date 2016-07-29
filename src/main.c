#include <stdlib.h>
#include <stdio.h>

#define LIGHT_SENSOR_PATH "/sys/devices/platform/applesmc.768/light"

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

int main()
{
		printf("current light sensor level: %i\n", sensor_value());
		return 0;
}
