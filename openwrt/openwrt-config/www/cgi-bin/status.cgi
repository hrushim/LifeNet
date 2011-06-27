#!/bin/sh
echo Content-type: text/html
echo ""


m_start=`echo "$QUERY_STRING" | sed -n 's/^.*m_start=\([^&]*\).*$/\1/p' | sed "s/%20/ /g" 2>/home/scripts/err_log`
s_start=`echo "$QUERY_STRING" | sed -n 's/^.*s_start=\([^&]*\).*$/\1/p' | sed "s/%20/ /g" 2>/home/scripts/err_log`
i_start=`echo "$QUERY_STRING" | sed -n 's/^.*i_start=\([^&]*\).*$/\1/p' | sed "s/%20/ /g" 2>/home/scripts/err_log`
p_start=`echo "$QUERY_STRING" | sed -n 's/^.*p_start=\([^&]*\).*$/\1/p' | sed "s/%20/ /g" 2>/home/scripts/err_log`
#c_start=`echo -e "$QUERY_STRING" | awk ' BEGIN { FS="="; } { print $2; } ' `
query_operand=`echo -e "$QUERY_STRING" | awk ' BEGIN { FS="_"; } { print $2; } ' 2>/home/scripts/err_log `
p_start=`echo -e "$query_operand" | awk ' BEGIN { FS="="; } { print $1; } ' 2>/home/scripts/err_log`
#echo -e "query=>$QUERY_STRING, operand=>$query_operand"
ip_ping=`echo -e "$query_operand" | awk ' BEGIN { FS="="; } { print $2; } ' 2>/home/scripts/err_log`

#echo -e "ip_add=$ip_ping p_start=$p_start"

#temp1=`echo -e "$c_start" | awk ' BEGIN { FS="%3A"; } { print $1; } ' `
#temp2=`echo -e "$c_start" | awk ' BEGIN { FS="%3A"; } { print $2; } ' `
#temp3=`echo -e "$c_start" | awk ' BEGIN { FS="%3A"; } { print $3; } ' `
#temp4=`echo -e "$c_start" | awk ' BEGIN { FS="%3A"; } { print $4; } ' `
#temp5=`echo -e "$c_start" | awk ' BEGIN { FS="%3A"; } { print $5; } ' `
#temp6=`echo -e "$c_start" | awk ' BEGIN { FS="%3A"; } { print $6; } ' `


#echo "m_start: $m_start"
#echo "s_start: $s_start"
#echo -e "query = $QUERY_STRING"
#echo "c_start: $c_start"

#if [ $tag == "cellid" ]; then

#	iwconfig ath0 ap `echo "$temp1:$temp2:$temp3:$temp4:$temp5:$temp6"`

#fi

if [ $p_start == 1 ]; then
	ip_addr=`echo -e 192.168.0.$ip_ping`
	echo $ip_addr > /home/scripts/ping_config
else
	if [ $p_start == 0 ]; then
#		echo "killing ping"
		rm /home/scripts/ping_config
		touch /home/scripts/ping_config
	fi
fi

if [ $m_start == 1 ]; then
	/home/scripts/mymanet start
else 

	if [ $m_start == 0 ]; then
		/home/scripts/mymanet stop
		/home/scripts/sniffer stop
		/home/scripts/injector stop
	fi
fi

if [ $s_start == 1 ]; then
	/home/scripts/sniffer start
	/home/scripts/injector start
else 

	if [ $s_start == 0 ]; then
		/home/scripts/injector stop
		/home/scripts/sniffer stop
	fi
fi

echo "<h2>LifeNet router configuration</h2>"

echo "<h3>Router Status</h3>"

m_running=`lsmod | grep manifold | wc -l`
#echo "m_running: $m_running"
if [ "$m_running" == "1" ]; then
	echo "<h4>........ Kernel module ON</h4>"
else
	echo "<h4>........ Kernel module OFF</h4>"
fi


#echo `ps`
s_running=`pidof sniff`
if [ "$s_running" != "" ]; then
	echo "<h4>........ Sniffer ON</h4>"
else
	echo "<h4>........ Sniffer OFF</h4>"
fi

i_running=`pidof inject`
if [ "$s_running" != "" ]; then
	echo "<h4>........ Injector ON</h4>"
else
	echo "<h4>........ Injector OFF</h4>"
fi


p_running=`cat /home/scripts/ping_config | wc -l`
#echo "p_running=>$p_running"
if [ "$p_running" != "0" ]; then
	echo "<h4>........ Ping ON</h4>"
else
	echo "<h4>........ Ping OFF</h4>"
fi

echo "<h3><br><br>Configuration</h3>"

if [ "$m_running" == "1" ]; then

	echo "<form action="status.cgi?m_start=0" method="post">"
	echo "<h4>-> WDL <input type="submit" value=" STOP ALL " /></h4>"
	echo "</form>"


else

	echo "<form action="status.cgi?m_start=1" method="post">"
	echo "<h4>-> WDL <input type="submit" value=" START " /></h4>"
	echo "</form>"
fi

if [ "$p_running" == "0"  ]; then

	echo "<form action="status.cgi?p_start=1" method="get">"
	echo "<h4>-> Ping <input type="text" name="ping_1" value="10" size="3" maxlength="3" />  <input type="submit" value=" START " /></h4>"
	echo "</form>"

else

	echo "<form action="status.cgi?p_start=0" method="get">"
	echo "<h4>-> Ping <input type="text" name="ping_0" value="10" size="3" maxlength="3" />  <input type="submit" value=" STOP " /></h4>"
	echo "</form>"

fi

if [ "$s_running" == "" ]; then

	echo "<form action="status.cgi?s_start=1" method="post">"
	echo "<h4>-> GNSM <input type="submit" value=" START " /></h4>"
	echo "</form>"


else

	echo "<form action="status.cgi?s_start=0" method="post">"
	echo "<h4>-> GNSM <input type="submit" value=" STOP " /></h4>"
	echo "</form>"
fi

