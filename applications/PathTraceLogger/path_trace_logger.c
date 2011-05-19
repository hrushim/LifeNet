/* 
 * File:   main.c
 * Author: hrushi
 *
 * Created on September 8, 2010, 11:16 AM
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
#include "path_trace_logger.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int check_manifold_lkm_status() {

    FILE *fp_lkm = NULL;

    uint8_t ret_value = 255;

    if ((fp_lkm = popen("lsmod | grep manifold | wc -l", "r")) == NULL) {
        printf("\nCould open lsmod.. Terminating...");
        exit(1);
    } else {
        char line[10];

        memset(&line, '\0', 10);
        while (fgets(line, 10, fp_lkm)) {
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

    FILE *fp = NULL;
    FILE *fptr_wdl = NULL;
    char filename[100];
    char buffer[MAX_BUF_SIZE];
    int line_cnt = 0;
    char ch = '0';

    int rd_fd = 0;

    while (check_manifold_lkm_status() == 1) {

#if IS_EMB_DEV
        sprintf(filename, "%s", TRACE_FILE_PATH);
#else
        sprintf(filename, "date >> %s%s", getenv("HOME"), TRACE_FILE_PATH);
#endif
        system(filename);

        sleep(SLEEP_INTERVAL);
        line_cnt = 0;

        rd_fd = open("/proc/wdl", O_RDONLY);

#if IS_EMB_DEV
        sprintf(filename, "%s", TRACE_FILE_PATH);
#else
        sprintf(filename, "%s%s", getenv("HOME"), TRACE_FILE_PATH);
#endif
        if ((fp = fopen(filename, "a")) == NULL) {
            printf("\nError: %s file could not be opened.\n", filename);
            return;
        }

        if (rd_fd != 0) {
            memset(buffer, '\0', MAX_BUF_SIZE);
            int num;
            while ((num = read(rd_fd, buffer, MAX_BUF_SIZE)) > 0) {
                fprintf(fp, "%s", buffer);
                fflush(fp);
                memset(buffer, '\0', MAX_BUF_SIZE);
            }
            close(rd_fd);

        }
        fclose(fp);

    }

    return (EXIT_SUCCESS);
}

