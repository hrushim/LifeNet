###############################
# Initial configuration
# Internet and LAN configuration

echo "Setting firewall"

#WAN_IP=
WAN_IF=`cat $HOME/.LifeNetData/config/eth_iface.conf`
#echo -e "\n$WAN_IF"
WAN_IP=`ifconfig $WAN_IF | grep "inet addr" | awk ' BEGIN { FS = ":"  } ;  { print $2 } ' | awk ' { print $1 } '`
#echo -e "\n$WAN_IP"
LAN_IF=`cat $HOME/.LifeNetData/config/wifi_iface.conf`
#echo -e "\n$LAN_IF"
LAN_IP=`ifconfig $LAN_IF | grep "inet addr" | awk ' BEGIN { FS = ":"  } ;  { print $2 } ' | awk ' { print $1 } '`
#echo -e "\n$LAN_IP"
bash $HOME/.LifeNetData/firewall/flush_all.sh

echo $WAN_IF $WAN_IP $LAN_IF $LAN_IP

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
# Forwarding rules

iptables -A INPUT -i $LAN_IF -j ACCEPT
iptables -A FORWARD -i $LAN_IF -o $WAN_IF -j ACCEPT
iptables -A OUTPUT -o $WAN_IF -j ACCEPT

#echo -e here

iptables -A INPUT -i $WAN_IF -j ACCEPT
iptables -A FORWARD -i $WAN_IF -o $LAN_IF -j ACCEPT
iptables -A OUTPUT -o $LAN_IF -j ACCEPT
#echo -e here

iptables -A INPUT -i $LO_IF -j ACCEPT
iptables -A OUTPUT -o $LO_IF -j ACCEPT

###############################
# Source NAT

iptables -t nat -A POSTROUTING -o $WAN_IF -j SNAT --to-source $WAN_IP

###############################

