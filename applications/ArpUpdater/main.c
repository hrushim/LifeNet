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
#define HOSTS_FILE_PATH "/etc/config/mymanet/hosts"
#else
#define HOSTS_FILE_PATH "/.LifeNetData/config/hosts"
#endif

#define INTERVAL 10

int check_manifold_lkm_status() {

    FILE *fp_lkm = NULL;

    uint8_t ret_value = 255;

    if ((fp_lkm = popen("lsmod | grep manifold | wc -l", "r")) == NULL) {
        printf("\nCould open lsmod.. Terminating...");
        exit(1);
    } else {
        char line[10];

        memset(&line, '\0', 10);
        while (fgets(line, 10, fp_lkm) != NULL) {
            ret_value = (uint8_t) atoi(line);
        }
    }

    pclose(fp_lkm);
    return ret_value;
}

/*
 * 
 */
int main(int argc, char** argv) {

    FILE *fptr = NULL;
    char *ptr_ip, *ptr_mac;
    char command_str[100], line[100], my_mac[50], str_mac[50], str_ip[50], filename[100];

    while (check_manifold_lkm_status() == 1) {

        memset(&command_str, '\0', 100);
        memset(&filename, '\0', 100);
        memset(&line, '\0', 100);
        memset(&str_mac, '\0', 50);
        memset(&str_ip, '\0', 50);


#if IS_EMB_DEV
        sprintf(filename, "%s", HOSTS_FILE_PATH);
#else
        sprintf(filename, "%s%s", getenv("HOME"), HOSTS_FILE_PATH);
#endif

        fptr = fopen(filename, "r");
        if (fptr == NULL) {
            //            exit(1);
        } else {
            ptr_ip = strtok(line, " ");

            while ((fgets(line, 80, fptr) != NULL)) {

                char *ptr_ip1, *ptr_ip2, *ptr_ip4, *ptr_ip3;
                char temp1[5], temp2[5], temp3[5], temp4[5];

                ptr_ip = strtok(line, " ");
                ptr_ip = strtok(NULL, " ");
                strcpy(str_ip, ptr_ip);

                ptr_mac = strtok(NULL, " ");
                strcpy(str_mac, ptr_mac);

                ptr_mac = strtok(NULL, " \n");
                strcpy(my_mac, ptr_mac);

                memset(temp1, '\0', 5);
                memset(temp3, '\0', 5);
                memset(temp2, '\0', 5);
                memset(temp4, '\0', 5);

                ptr_ip1 = strtok(str_ip, ".");
                strcpy(temp1, ptr_ip1);

                ptr_ip2 = strtok(NULL, ".");
                strcpy(temp2, ptr_ip2);

                ptr_ip3 = strtok(NULL, ".");
                strcpy(temp3, ptr_ip3);

                ptr_ip4 = strtok(NULL, ".");
                strcpy(temp4, ptr_ip4);
                if (strcmp(my_mac, str_mac) != 0) {

                    if (strcmp(temp1, "192") == 0) {
                        memset(&command_str, '\0', 100);
                        sprintf(command_str, "\nsudo arp -s %s.%s.%s.%s %s", ptr_ip1, ptr_ip2, ptr_ip3, ptr_ip4, str_mac);
                    } else {
                        memset(&command_str, '\0', 100);
                        sprintf(command_str, "\nsudo arp -s %s.%s.%s.%s %s", ptr_ip4, ptr_ip3, ptr_ip2, ptr_ip1, str_mac);
                    }
                    fflush(stdout);

                    system(command_str);
                }

            }
            fclose(fptr);

            sleep(INTERVAL);
        }
    }


    return (EXIT_SUCCESS);
}

