#include <stdio.h>
#include <stdint.h>

uint8_t check_manifold_lkm_status() {

    FILE *fp_lkm = NULL;

    uint8_t ret_value = 255;

    if ((fp_lkm = popen("lsmod | grep manifold | wc -l", "r")) == NULL) {
        printf("\nCould open lsmod.. Terminating...");
        exit(1);
    } else {
        char line[10];

        memset(&line, '\0', 10);
        while(fgets(line, 10, fp_lkm)) {
            ret_value = (uint8_t)atoi(line);
        }
    }

    pclose(fp_lkm);
    return ret_value;
}

int main(int argc, char** argv) {

    while(check_manifold_lkm_status()==1) {

        sleep(10);
        system("rm /var/www/distance");
        system("cp /proc/distance /var/www/distance");

    }
}
