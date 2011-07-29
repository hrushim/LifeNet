#include <stdio.h>

int main(int argc, char** argv) {

    while(1) {

        sleep(10);
	system("rm /www/distance");
        system("cp /proc/distance /www/distance");

    }
}
