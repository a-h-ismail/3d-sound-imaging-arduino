#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
int main()
{
    FILE *arduino_input = NULL;
    puts("Waiting for input...");
    while (arduino_input == NULL)
    {
        arduino_input = fopen("/home/ahmad/test.txt", "r");
        sleep(1);
    }
    puts("Input stream found, reading from /dev/ttyACM0");
    char buffer[256];
    double x[324], y[324], z[324];
    double ro, teta, phi;
    int i = 0, status;
    while (1)
    {
        bool run;
        do
        {
            fscanf(arduino_input, "%s", buffer);
            sleep(1);
        } while (strcmp(buffer, "start") == 0);
        puts("Received start signal, processing data...");

        while (i < 314)
        {
            fscanf(arduino_input, "%255s", buffer);
            if (strcmp("The End", buffer) == 0)
            {
                puts("Received stop signal, saving datapoints and plotting them.");
                break;
            }

            status = sscanf(buffer, "%lf,%lf,%lf", &ro, &phi, &teta);
            if (status == 3)
            {
                teta *= M_PI / 180;
                phi *= M_PI / 180;
                x[i] = ro * sin(teta) * cos(phi);
                y[i] = ro * sin(teta) * sin(phi);
                z[i] = ro * cos(teta);
                printf("P%d (%g,%g,%g)\n",i,x[i],y[i],z[i]);
                ++i;
            }
        }
        FILE *output = fopen("output.txt", "w");
        for (i = 0; i < 324; ++i)
        {
            fprintf(output, "%lf,%lf,%lf\n", x[i], y[i], z[i]);
        }
        fclose(output);
    }
}