#include <stdio.h>

int main(int argc, char** argv) {

    while(1) {

        sleep(10);
        system("rm /var/www/distance");
        system("cp /proc/distance /var/www/distance");

    }
}
