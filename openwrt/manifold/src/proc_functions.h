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

#ifndef PROC_FUNCTIONS_H
#define PROC_FUNCTIONS_H

int wdl_read(char* page, char** start, off_t off, int count, int* eof, void *data);
int wdl_write(struct file *file, const char *buffer, unsigned long count, void *data);

int rxstats_read(char* page, char** start, off_t off, int count, int* eof, void *data);
int txstats_read(char* page, char** start, off_t off, int count, int* eof, void *data);

int distance_read(char* page, char** start, off_t off, int count, int* eof, void *data);


#endif 	/*End of proc_functions.h*/
