#!/usr/bin/python

# This script should perform the following steps:
##  1. Platform specific checks before starting Lifenet
##  2. Read configuration
##  3. Generate iwconfig/ifconfig commands to configure the interfaces for Lifenet
##  4. Start the GNSM module

# Note: The script assumes that the default installation path is ~/.LifeNetData

import platform
import os
import random
import subprocess

DEBUG = True

def read_from_file(filename):
    f = open(filename, "r")
    if f: return f.readline().strip('\n')
    else: return ""

def run_command(command):
    try:
        print 'Executing command: ' + command
        output = subprocess.check_output(command, shell=True)
    except subprocess.CalledProcessError, e:
        print "ERR: Command " + command + " failed with error code: " + \
            str(e.returncode) + ": " + \
            str(e.output)
    except KeyboardInterrupt, k:
        print "ERR: User killed process executing the command: " + command + "."

print "\nWelcome to Lifenet v1.1\n"
print

# Perform platform specific checks

print "System Information:"
print 'uname:', platform.uname()
print 'machine  :', platform.machine()
print 'processor:', platform.processor()
print 'system   :', platform.system()
if platform.system() != 'Linux':
    print "ERR: Lifenet currently runs only on Linux."
print 'version  :', platform.version()
print 'release  :', platform.release()
release_list = platform.release().split('.')
if int(release_list[0]) == 2:
    if int(release_list[1]) >= 6 and int(release_list[2]) >= 3:
        LINUX_VERSION_2_6_PLUS = 1
        print "Kernel version > 2.6.3 detected."
elif int(release_list[0]) == 3:
        LINUX_VERSION_3_0_PLUS = 1
        print "Kernel version > 3.0 detected."


# Fetch the configuration from configuration files

APP_DATA_PATH = os.environ['HOME'] + '/.LifeNetData'
INSTALL_DIR_PATH = read_from_file(APP_DATA_PATH + '/config/install_path.conf')
ETH_IF = read_from_file(APP_DATA_PATH + '/config/eth_iface.conf')
WIFI_IF = read_from_file(APP_DATA_PATH + '/config/wifi_iface.conf')
NODE_NAME = read_from_file(APP_DATA_PATH + '/config/node_name.conf')


# Notify the user to disable NetworkManager

print "Please disable Wireless from your NetworkManager."
print "This is very important. "
print "NetworkManager interferes with the commandline utilities for altering the network interfaces."
DONT_CARE = raw_input("Press any key to continue...")


# Generate a dynamically asigned IP address for Lifenet

WIFI_IP = "192.168.0." + str(random.randint(1, 250))


# Configure the interfaces for running Lifenet

output = run_command('sudo iwconfig ' + WIFI_IF + ' ap 00:00:00:00:00:00')
output = run_command('sudo ifconfig ' + WIFI_IF + ' down')
output = run_command('sudo iwconfig ' + WIFI_IF + ' mode ad-hoc')
output = run_command('sudo iwconfig ' + WIFI_IF + ' channel 1')
output = run_command('sudo ifconfig ' + WIFI_IF + ' up')
output = run_command('sudo iwconfig ' + WIFI_IF + ' ESSID lifenet')
output = run_command('sudo ifconfig ' + WIFI_IF + ' ' + WIFI_IP + ' up')
output = run_command('rmmod manifold')
output = run_command('/tmp/loop.py &')
output = run_command('sudo insmod ' + APP_DATA_PATH + '/manifold.ko device_name=' + WIFI_IF + ' mymanet_alpha=20 mymanet_beta=-20')


# Start the Global network statistics module (GNSM)

os.system('sudo ' + APP_DATA_PATH + '/inject ' + WIFI_IF + ' ' + WIFI_IP + ' ' + \
            NODE_NAME + ' 5 & > /tmp/injectlog')
os.system('sudo ' + APP_DATA_PATH + '/sniff ' + WIFI_IF + ' 5 0 0 30 & > /tmp/snifflog')


# The script ran successfully. Print a message and exit.

if DEBUG: print '\n--- Debug Information ---\n' + \
            APP_DATA_PATH + ' ' + \
            INSTALL_DIR_PATH + ' ' + \
            ETH_IF + ' ' + WIFI_IF + ' ' + \
            NODE_NAME + ' ' + WIFI_IP + ' ' + \
            '\n--- End ---\n'
print "Lifenet is now running. Relevant files have been installed in: " \
        + os.environ['HOME'] + "."
print
