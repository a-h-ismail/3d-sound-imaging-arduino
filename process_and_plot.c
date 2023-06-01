#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define POINTS_PER_SWEEP 19
#define STEPPER_TOTAL_STEPS 18
#define TOTAL_DATAPOINTS POINTS_PER_SWEEP *STEPPER_TOTAL_STEPS
#define SENSOR_HEIGHT_CM 5

int main()
{
    FILE *serial_input = NULL, *output = NULL;
    puts("Waiting for input...");
    while (serial_input == NULL)
    {
        // Depends where the USB serial is connected
        serial_input = fopen("/dev/ttyACM0", "r");
        if (serial_input == NULL)
            serial_input = fopen("/dev/ttyACM1", "r");
        if (serial_input == NULL)
            serial_input = fopen("/dev/ttyACM2", "r");
        sleep(1);
    }
    puts("Input stream found, reading from /dev/ttyACM0-2");
    char buffer[256];
    double x, y, z;
    double ro, teta, phi;
    int i, status;

    do
    {
        fscanf(serial_input, "%256s", buffer);
        sleep(1);
    } while (strcmp(buffer, "start") == 0);

    puts("Received start signal, reading and processing data...");
    // Open the output file
    output = fopen("plot_data.txt", "w");

    i = 0;
    while (i < POINTS_PER_SWEEP * STEPPER_TOTAL_STEPS)
    {
        fscanf(serial_input, "%255s", buffer);
        status = sscanf(buffer, "%lf,%lf,%lf", &ro, &phi, &teta);
        if (strcmp("End", buffer) == 0)
            break;
        // Skip empty inputs
        if (status == 3)
        {
            teta *= M_PI / 180;
            phi *= M_PI / 180;
            // Correcting ro
            ro = sqrt(pow(SENSOR_HEIGHT_CM, 2) + pow(ro, 2));

            double teta_c = asin(SENSOR_HEIGHT_CM / ro);

            // Correcting teta
            teta = M_PI / 2 - teta_c - teta;

            x = ro * sin(teta) * cos(phi);
            y = ro * sin(teta) * sin(phi);
            z = ro * cos(teta);
            printf("P%d (%g,%g,%g)\n", i, x, y, z);
            fprintf(output, "%lf %lf %lf\n", x, y, z);
            ++i;
        }
    }
    fclose(output);
    system("echo \"splot '$PWD/plot_data.txt' with points\" | gnuplot --persist");
    return 0;
}