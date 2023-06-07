#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define POINTS_PER_SWEEP 19
#define STEPPER_TOTAL_STEPS 18
#define TOTAL_DATAPOINTS (POINTS_PER_SWEEP * STEPPER_TOTAL_STEPS)
#define SENSOR_HEIGHT_CM 5

int main()
{
    FILE *serial_input = NULL, *output = NULL, *locate_stream = NULL;
    char tmp[256];
    double x, y, z;
    double ro, teta, phi;
    int i, status;

    puts("Waiting for input...");
    while (serial_input == NULL)
    {
        // Detect the USB serial connection by scanning /dev
        locate_stream = popen("echo /dev/`ls /dev | grep ttyACM`", "r");
        fscanf(locate_stream, "%255s", tmp);

        // If the output is not /dev/
        if (strlen(tmp) > 5)
            serial_input = fopen(tmp, "r");
        
        pclose(locate_stream);
        sleep(1);
    }
    printf("Input stream found, reading from %s\n", tmp);

    do
    {
        fscanf(serial_input, "%256s", tmp);
        sleep(1);
    } while (strcmp(tmp, "start") == 0);

    puts("Received start signal, reading and processing data...");
    // Open the output file
    output = fopen("plot_data.txt", "w");

    i = 0;
    while (i < TOTAL_DATAPOINTS)
    {
        fscanf(serial_input, "%255s", tmp);
        status = sscanf(tmp, "%lf,%lf,%lf", &ro, &phi, &teta);
        if (strcmp("End", tmp) == 0)
            break;
        // Skip empty inputs
        if (status == 3)
        {
            teta *= M_PI / 180;
            phi += 45;
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
        // Prevent thrashing the computer performance if the serial line is not sending data, wait 0.1s between reads
        else
            usleep(100000);
    }
    fclose(output);
    // Call the plotter
    system("echo \"splot '$PWD/plot_data.txt' with points\" | gnuplot --persist");
    return 0;
}