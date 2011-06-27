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
Developers: Ashwin Paranjpe, Santosh Vempala
Georgia Institute of Technology, Atlanta, USA
*/

#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/sched.h>
#include<linux/module.h>
#include<linux/moduleparam.h>

#include<linux/string.h>

int compare_mac_address( uint8_t mac1[6], uint8_t mac2[6])
{
        if((memcmp(mac1, mac2, 6))==0)
                return 1;
        else 
                return 0;
}

/*
 *  * Convert ascii hardware address into byte string.
 *   * */
int hwaddr_aton(char *a, u_char *n)
{
        int i, o[6];
        i = sscanf(a, "%x:%x:%x:%x:%x:%x", &o[0], &o[1], &o[2],
                                           &o[3], &o[4], &o[5]);
        if (i != 6) {
                printk(KERN_ALERT "Invalid hardware address '%s'\n",a);
                return 0;
        }
        for (i=0; i<6; i++)
                n[i] = o[i];
        return 6;
}

