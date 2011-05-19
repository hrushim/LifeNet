/*
 * File:   main.c
 * Author: hrushi
 *
 * Created on September 15, 2010, 3:38 PM
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>	/*for uint32_t*/
#include<sys/socket.h>
#include<features.h>
#include<linux/if_packet.h>
#include<linux/if_ether.h>
#include<errno.h>
#include<sys/ioctl.h>
#include<net/if.h>
#include<net/ethernet.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#if IS_EMB_DEV
#define GNST_FILE_PATH "/etc/config/mymanet/gnst.txt"
#define NAME_FILE_PATH "/etc/config/mymanet/node_name.conf"
#else
#define GNST_FILE_PATH "/.LifeNetData/config/gnst.txt"
#define NAME_FILE_PATH "/.LifeNetData/config/node_name.conf"
#endif

#define MAX_NUM_OF_HOSTS 255
#define INTERVAL 10

char my_name[30];
char hosts[MAX_NUM_OF_HOSTS][30];
int host_count = 0;

int check_manifold_lkm_status() {

    FILE *fp_lkm = NULL;

    uint8_t ret_value = 255;

    if ((fp_lkm = popen("lsmod | grep manifold | wc -l", "r")) == NULL) {
        printf("\nCould open lsmod.. Terminating...");
        //  exit(1);
    } else {
        char line[10];

        memset(&line, '\0', 10);
        while (fgets(line, 10, fp_lkm)) {
            ret_value = (uint8_t) atoi(line);
        }
    }

    pclose(fp_lkm);
    //    printf("Returning...%d", ret_value);
    fflush(stdout);
    return ret_value;
}

void check_and_add(char hostname[30]) {
    //    printf("\nInside check_and_add");
    fflush(stdout);

    if(hostname != NULL)
    {
	    
	    int present_flag = 0;

	    int i = 0;
	    for (i = 0; i < host_count; i++) {
		if (strncmp(hosts[i], hostname, 20) == 0) {
		    //            printf("\nPresent!! %s", hostname);
		    present_flag = 1;
		}
	    }

	    if (!present_flag) {
		strcpy(hosts[host_count], hostname);
		//        printf("\nAdding at %d, %s", host_count, hostname);
		host_count++;
	    }

	    //    printf("\nOutside check_and_add");
	    fflush(stdout);
    }
}

void display_hosts() {

    int i = 0;
    for (i = 0; i < host_count; i++) {
        printf("\n%d->%s", i, hosts[i]);
        fflush(stdout);
    }
}

void add_or_update_hosts() {

    FILE *fptr = NULL;
    char *ptr_name1, *ptr_name2;
    char ptr_name1_string[30];
    char ptr_name2_string[30];
    char line[200];
    char filename[100];

    memset(&filename, '\0', 100);
    memset(&line, '\0', 200);
    memset(&ptr_name1_string, '\0', 30);
    memset(&ptr_name2_string, '\0', 30);

#if IS_EMB_DEV
    sprintf(filename, "%s", GNST_FILE_PATH);
#else
    sprintf(filename, "%s%s", getenv("HOME"), GNST_FILE_PATH);
#endif

    fptr = fopen(filename, "r");
    if (fptr == NULL) {
        // exit(1);
    } else {
        while ((fgets(line, 200, fptr) != NULL)) {

            //                printf("\nRead line ->%s", line);

            if (strlen(line) > 1) {
                ptr_name1 = strtok(line, " ");
                ptr_name2 = strtok(NULL, " ");
                strcpy(ptr_name1_string, ptr_name1);
                strcpy(ptr_name2_string, ptr_name2);

                //                    printf("\nName1 = %s, Name2 = %s", ptr_name1_string, ptr_name2_string);
                fflush(stdout);

                check_and_add(ptr_name1_string);
                check_and_add(ptr_name2_string);

                memset(&line, '\0', 100);
                memset(&ptr_name1_string, '\0', 10);
                memset(&ptr_name2_string, '\0', 10);
            }
        }
        fclose(fptr);
    }
}

char* get_stats_from_file(char name1[30], char name2[30]) {
    FILE *fptr = NULL;
    char *ptr_name1, *ptr_name2, *ptr_bcast, *ptr_bcast_rx, *ptr_timestamp;
    char ptr_name1_string[30];
    char ptr_name2_string[30];
    char ptr_bcast_string[20];
    char ptr_timestamp_string[20];
    char ptr_bcast_rx_string[20];
    char line[200];
    char filename[100];
    char ret_value[30];

    memset(&filename, '\0', 100);
    memset(&line, '\0', 200);
    memset(&ptr_timestamp_string, '\0', 20);
    memset(&ptr_name1_string, '\0', 30);
    memset(&ptr_name2_string, '\0', 30);
    memset(&ret_value, '\0', 30);

#if IS_EMB_DEV
    sprintf(filename, "%s", GNST_FILE_PATH);
#else
    sprintf(filename, "%s%s", getenv("HOME"), GNST_FILE_PATH);
#endif

    fptr = fopen(filename, "r");
    if (fptr == NULL) {
        //exit(1);
    } else {
        while ((fgets(line, 200, fptr) != NULL)) {

            //                printf("\nRead line ->%s", line);

            if (strlen(line) > 1) {
                ptr_name1 = strtok(line, " ");
                ptr_name2 = strtok(NULL, " ");
                strcpy(ptr_name1_string, ptr_name1);
                strcpy(ptr_name2_string, ptr_name2);

                if (strcmp(ptr_name1_string, name1) == 0 && strcmp(ptr_name2_string, name2) == 0) {

                    int i = 0;

                    for (i = 0; i < 10; i++) {
                        ptr_bcast = strtok(NULL, " ");
                    }
                    ptr_bcast_rx = strtok(NULL, " ");
                    ptr_bcast_rx = strtok(NULL, " ");

                    ptr_timestamp = strtok(NULL, " ");
                    ptr_timestamp = strtok(NULL, " ");

                    strcpy(ptr_bcast_string, ptr_bcast);
                    strcpy(ptr_bcast_rx_string, ptr_bcast_rx);
                    strcpy(ptr_timestamp_string, ptr_timestamp);

                    //            printf("\n%s %s %s %s %s", name1, name2, ptr_bcast_string, ptr_bcast_rx_string, ptr_timestamp_string);

                    __time_t time_from_file_sec = atoi(ptr_timestamp_string);

                    struct timeval curr_time;
                    gettimeofday(&curr_time, NULL);

                    if ((curr_time.tv_sec - time_from_file_sec) > 30) {
                        sprintf(ret_value, "--, --", ptr_bcast_string, ptr_bcast_rx_string);
                    } else
                        sprintf(ret_value, "%s,%s", ptr_bcast_string, ptr_bcast_rx_string);
                }
            }
        }
        fclose(fptr);
    }

    return &ret_value;
}

void display_gnst() {
    int i = 0, j = 0;
    system("clear");
    printf("\n\nMy name: %s\n", my_name);
    fflush(stdout);
    printf("\nNumber of MyMANET nodes detected: %d\n\n", host_count);
    printf("\n===== Global Statistics Table =====\n\n\n");
    printf("%-20s", "SENDER / RECEIVER");


    while (i < host_count) {
        printf("%-20s", hosts[i++]);
    }

    while (j < host_count) {
        printf("\n\n%-20s", hosts[j]);
        i = 0;

        while (i < host_count) {
            if (j != i) {
                char stats_string[20];
                strncpy(stats_string, get_stats_from_file(hosts[j], hosts[i]), 20);
                printf("%-20s", stats_string);

            } else {
                printf("%-20s", "XXXX");
            }
            i++;
        }
        printf("\n");
        j++;
    }

    fflush(stdout);
}

void update_my_name() {
    FILE *fptr = NULL;
    char line[200];
    char filename[100];

    memset(&filename, '\0', 100);
    memset(&line, '\0', 200);

    strcpy(my_name, "My name: Not known yet!");

#if IS_EMB_DEV
    sprintf(filename, "%s", GNST_FILE_PATH);
#else
    sprintf(filename, "%s%s", getenv("HOME"), NAME_FILE_PATH);
#endif

    fptr = fopen(filename, "r");
    if (fptr == NULL) {
        //exit(1);
    } else {
        while ((fgets(line, 200, fptr) != NULL)) {
            strncpy(my_name, line, 30);
        }
        fclose(fptr);
    }
    my_name[strlen(my_name) - 1] = '\0';
}

void display_distances() {

    int i = 0;

    printf("\n\n\n\n========= Distance Table ==========\n");
    printf("\n%-20s", "CURRENT DISTANCES");

    for (i = 0; i < host_count; i++) {
        printf("%-20s", hosts[i]);
    }

    printf("\n%-20s", "FROM THIS HOST");
    fflush(stdout);

    for (i = 0; i < host_count; i++) {
        if (strcmp(hosts[i], my_name) != 0) {

            char stats_string[20];
            strncpy(stats_string, get_stats_from_file(my_name, hosts[i]), 20);

            char *ptr_tx, *ptr_rx;
            char tx_string[20], rx_string[20];

            ptr_tx = strtok(stats_string, ",");
            ptr_rx = strtok(NULL, " \0\n");

            strncpy(tx_string, ptr_tx, 20);
            strncpy(rx_string, ptr_rx, 20);

            uint32_t num_tx = atoi(tx_string);
            uint32_t num_rx = atoi(rx_string);

            uint8_t distance = 255;

            if (num_rx >= num_tx) {
                if (num_rx == 0 && num_tx == 0) {
                    distance = 255;
                } else {
                    distance = (uint8_t) ((num_tx * 100) / num_rx);
                }
            } else {
                if ((((255 - 100) * num_rx) / num_tx) >= 255)
                    distance = 1;
                else
                    distance = (uint8_t) (255 - (((255 - 100) * num_rx) / num_tx));

            }

            printf("%-20d", distance);
        } else {
            printf("%-20d", 0);
        }
    }
    printf("\n\n");
    fflush(stdout);
}

/*
 * 
 */
int main(int argc, char** argv) {

    while (check_manifold_lkm_status() == 1) {
        update_my_name();
        printf("\nAfter update_my_name");
        fflush(stdout);
        add_or_update_hosts();
        printf("\nAfter add_or_update_hosts");
        fflush(stdout);
        display_hosts();
        display_gnst();
        display_distances();
        sleep(INTERVAL);
    }
    return (EXIT_SUCCESS);
}

