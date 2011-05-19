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

Compile:

	make NEW_KERNEL=[1/0] DEBUG_ON=[1/0]


Clean: 

	make clean


Insert:

	sudo insmod manifold.ko device_name=[INTERFACE NAME]


Remove:

	sudo rmmod manifold
