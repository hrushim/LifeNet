###############################
# Initial configuration
# Internet and LAN configuration

WAN_IP=$2
WAN_IF=$1

LAN_IP=$4
LAN_PREFIX=$5
LAN_IP_RANGE=$4$5
LAN_IF=$3

LO_IF="lo"
LO_IP="127.0.0.1"

###############################

###############################
# Module loading

/sbin/depmod -a

/sbin/modprobe ip_tables
/sbin/modprobe ip_conntrack
/sbin/modprobe iptable_filter
/sbin/modprobe iptable_mangle
/sbin/modprobe iptable_nat
/sbin/modprobe ipt_LOG
/sbin/modprobe ipt_limit
/sbin/modprobe ipt_state

# End Module Loading
###############################

###############################
# Proc configuration

echo "1" > /proc/sys/net/ipv4/ip_forward

# End Proc configuration
###############################

###############################
# Policy definitions

iptables -P INPUT DROP
iptables -P OUTPUT DROP
iptables -P FORWARD DROP

###############################

###############################
# ICMP rules

iptables -A INPUT -i $LAN_IF -p ICMP -j ACCEPT
iptables -A FORWARD -i $LAN_IF -o $WAN_IF -p ICMP -j ACCEPT
iptables -A OUTPUT -o $WAN_IF -p ICMP -j ACCEPT

iptables -A INPUT -i $WAN_IF -p ICMP -j ACCEPT
iptables -A FORWARD -i $WAN_IF -o $LAN_IF -p ICMP -j ACCEPT
iptables -A OUTPUT -o $LAN_IF -p ICMP -j ACCEPT

iptables -A INPUT -i $LO_IF -p ICMP -j ACCEPT
iptables -A OUTPUT -o $LO_IF -p ICMP -j ACCEPT

###############################

###############################
# DNS rules

echo "icmp done"
iptables -A INPUT -i $LAN_IF -p UDP --dport 1024:65535 --sport 53 -j ACCEPT
iptables -A FORWARD -i $LAN_IF -o $WAN_IF -p UDP -j ACCEPT
iptables -A OUTPUT -o $WAN_IF -p UDP --sport 1024:65535 --dport 53 -j ACCEPT

iptables -A INPUT -i $WAN_IF -p UDP --sport 53 --dport 1024:65535 -j ACCEPT
iptables -A FORWARD -i $WAN_IF -o $LAN_IF -p UDP -j ACCEPT
iptables -A OUTPUT -o $LAN_IF -p UDP --sport 1024:65535 --dport 53 -j ACCEPT

###############################

###############################
# WWW rules

iptables -A INPUT -i $LAN_IF -p TCP --dport 1024:65535 --sport 80 -j ACCEPT
iptables -A FORWARD -i $LAN_IF -o $WAN_IF -p TCP -j ACCEPT
iptables -A OUTPUT -o $WAN_IF -p TCP --sport 1024:65535 --dport 80 -j ACCEPT

iptables -A INPUT -i $WAN_IF -p TCP --sport 80 --dport 1024:65535 -j ACCEPT
iptables -A FORWARD -i $WAN_IF -o $LAN_IF -p TCP -j ACCEPT
iptables -A OUTPUT -o $LAN_IF -p TCP --sport 1024:65535 --dport 80 -j ACCEPT

###############################

iptables -A INPUT -i $LAN_IF -p TCP --dport 50000:60000 --sport 50000:60000 -j ACCEPT
iptables -A FORWARD -i $LAN_IF -o $WAN_IF -p TCP --sport 50000:60000 --dport 50000:60000 -j ACCEPT
iptables -A FORWARD -o $LAN_IF -i $WAN_IF -p TCP --sport 50000:60000 --dport 50000:60000 -j ACCEPT
iptables -A INPUT -o $LAN_IF -p TCP --dport 50000:60000 --sport 50000:60000 -j ACCEPT


###############################
# Source NAT

iptables -t nat -A POSTROUTING -o $WAN_IF -j SNAT --to-source $WAN_IP

###############################


