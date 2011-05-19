/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Project: MyMANET - A Platform to Build Customized MANET's
Developers: Hrushikesh Mehendale, Ashwin Paranjpe, Santosh Vempala
Georgia Institute of Technology, Atlanta, USA
 */

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include"sniffer.h"

#define RANGE 100
#define TESTING 0

extern uint8_t g_distance_table[MAX_NUM_OF_HOSTS];
extern int g_tolerance;

int manifold_routing(int receiver, unsigned char recvmac[6], int s, int r) {
    uint8_t distance = 255;
    char command[50] = "";

    int update_kernel_distance_table = 0;

    if (r >= s) {
        if (r == 0) {
            distance = 255;
        } else {
            distance = (uint8_t) ((s * RANGE) / r);
        }
    } else {
        if (s != 0) {
            if ((((255 - RANGE) * r) / s) >= 255)
                distance = 1;
            else
                distance = (uint8_t) (255 - (((255 - RANGE) * r) / s));
        } else {
            distance = 255;
        }

    }

    if (((distance - g_distance_table[receiver]) >= g_tolerance) || (((g_distance_table[receiver] - distance) >= g_tolerance)) || g_distance_table[receiver] == 0) {
        update_kernel_distance_table = 1;
        g_distance_table[receiver] = distance;
    }

    if (update_kernel_distance_table) {
        sprintf(command, "echo distance %x:%x:%x:%x:%x:%x %d > /proc/wdl", recvmac[0], recvmac[1],
                recvmac[2], recvmac[3], recvmac[4], recvmac[5], g_distance_table[receiver]);
        system(command);
    }

    if (distance < bcast_fwd_threshold) {
        bcast_fwd_deno++;
    }

    return 1;
}
