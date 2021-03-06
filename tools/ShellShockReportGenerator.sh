#!/bin/bash
#!/dev/tty1
# crontab like: 25 12 * * * sh /home/ShellShockReportGenerator.sh user password db hostname port
# !!! have https://svn.nmap.org/nmap/scripts/http-shellshock.nse for nmap scanning in /home/http-shellshock.nse !!!
NOW=$(date +"%y-%m-%d");
ARGV=("$@")
ARGC=("$#")

if [ "$#" -ne 6 ]; then
    echo "USAGE: ${0} dbuser dbpassword db hostname port localuser"
    exit 0
fi

# Assign user and password
username="${1}"
password="${2}"
database="${3}"
host_fx="${4}"
mysqlport="${5}"

# Assign local user for www presentation scan results
localuser="${6}"
report=`mktemp /home/${localuser}/public_html/report_${NOW}_XXXXXX`

echo "Downloading plugin!"
echo ""
$(command -v wget) https://svn.nmap.org/nmap/scripts/http-shellshock.nse
echo ""
 
# List the parameter values passed.
echo "Hostname:  " ${host_fx}
echo "    Port:  " ${mysqlport}
echo "Username:  " ${username}
echo "Password:  *********" 
echo "Database:  " ${database}
echo ""

$(command -v mkdir)  /home/${localuser}/Downloads/log
echo ""

# Connect and pipe the query result minus errors and warnings to the while loop.
IP21=$(mysql -u${username} -p${password} -D${database} -h${host_fx} --port=${mysqlport} -se "SELECT DISTINCT(IP) IP FROM PORTOPEN WHERE PORT = '21' AND 'SEEN' < date_add(CURDATE(), INTERVAL 0 DAY) ORDER BY IP DESC " )
file1=`mktemp /home/${localuser}/Downloads/log/21.log_XXXXXX`
# Read through the piped result until it's empty but format the title.
items=$(echo ${IP21} | tr " " "\n")

for item in $items
do
    echo "$item" >> $file1
done

# Connect and pipe the query result minus errors and warnings to the while loop.
IP443=$(mysql -u${username} -p${password} -D${database} -h${host_fx} --port=${mysqlport} -se "SELECT DISTINCT(IP) IP FROM PORTOPEN WHERE PORT = '443' AND 'SEEN' < date_add(CURDATE(), INTERVAL 0 DAY) ORDER BY IP DESC " )
file2=`mktemp /home/${localuser}/Downloads/log/443.log_XXXXXX`
# Read through the piped result until it's empty but format the title.
items=$(echo ${IP443} | tr " " "\n")
 
for item in $items
do
    echo "$item" >> $file2
done

# Connect and pipe the query result minus errors and warnings to the while loop.
IP3128=$(mysql -u${username} -p${password} -D${database} -h${host_fx} --port=${mysqlport} -se "SELECT DISTINCT(IP) IP FROM PORTOPEN WHERE PORT = '3128' AND 'SEEN' < date_add(CURDATE(), INTERVAL 0 DAY) ORDER BY IP DESC " )
file3=`mktemp /home/${localuser}/Downloads/log/3128.log_XXXXXX`
# Read through the piped result until it's empty but format the title.
items=$(echo ${IP3128} | tr " " "\n")

for item in $items
do
    echo "$item" >> $file3
done

# Connect and pipe the query result minus errors and warnings to the while loop.
IP8080=$(mysql -u${username} -p${password} -D${database} -h${host_fx} --port=${mysqlport} -se "SELECT DISTINCT(IP) IP FROM PORTOPEN WHERE PORT = '8080' AND 'SEEN' < date_add(CURDATE(), INTERVAL 0 DAY) ORDER BY IP DESC " )
file4=`mktemp /home/${localuser}/Downloads/log/8080.log_XXXXXX`
# Read through the piped result until it's empty but format the title.
items=$(echo ${IP8080} | tr " " "\n")

for item in $items
do
    echo "$item" >> $file4
done

# Connect and pipe the query result minus errors and warnings to the while loop.
IP8081=$(mysql -u${username} -p${password} -D${database} -h${host_fx} --port=${mysqlport} -se "SELECT DISTINCT(IP) IP FROM PORTOPEN WHERE PORT = '8081' AND 'SEEN' < date_add(CURDATE(), INTERVAL 0 DAY) ORDER BY IP DESC " )
file5=`mktemp /home/${localuser}/Downloads/log/8081.log_XXXXXX`
# Read through the piped result until it's empty but format the title.
items=$(echo ${IP8081} | tr " " "\n")

for item in $items
do
    echo "$item" >> $file5
done

# Connect and pipe the query result minus errors and warnings to the while loop.
IP80=$(mysql -u${username} -p${password} -D${database} -h${host_fx} --port=${mysqlport} -se "SELECT DISTINCT(IP) IP FROM PORTOPEN WHERE PORT = '80' AND 'SEEN' < date_add(CURDATE(), INTERVAL 0 DAY) ORDER BY IP DESC " )
file6=`mktemp /home/${localuser}/Downloads/log/80.log_XXXXXX`
# Read through the piped result until it's empty but format the title.
items=$(echo ${IP80} | tr " " "\n")

for item in $items
do
    echo "$item" >> $file6
done
echo "List Finalized!"
echo ""
echo "Gone Scanning ..."
echo ""

for i in `cat /home/${localuser}/Downloads/log/21.log_*`;  do $(command -v nmap) -sV -p 21 -Pn --script http-shellshock --script-args uri=/cgi-bin/bin,cmd=ping $i ;done >> $report ;
for i in `cat /home/${localuser}/Downloads/log/80.log_*`;  do $(command -v nmap) -sV -p 80 -Pn --script http-shellshock --script-args uri=/cgi-bin/bin,cmd=ping $i ;done >> $report ;
for i in `cat /home/${localuser}/Downloads/log/443.log_*`; do $(command -v nmap) -sV -p 443 -Pn --script http-shellshock --script-args uri=/cgi-bin/bin,cmd=ping $i ;done >> $report ;
for i in `cat /home/${localuser}/Downloads/log/3128.log_*`; do $(command -v nmap) -sV -p 3128 -Pn --script http-shellshock --script-args uri=/cgi-bin/bin,cmd=ping $i ;done >> $report ;
for i in `cat /home/${localuser}/Downloads/log/8080.log_*`; do $(command -v nmap) -sV -p 8080 -Pn --script http-shellshock --script-args uri=/cgi-bin/bin,cmd=ping $i ;done >> $report ;
for i in `cat /home/${localuser}/Downloads/log/8081.log_*`; do $(command -v nmap) -sV -p 8081 -Pn --script http-shellshock --script-args uri=/cgi-bin/bin,cmd=ping $i ;done >> $report ;

echo "Done Scanning!"
echo ""
echo "Report: $report"
echo ""

$(command -v rm) -rf /home/${localuser}/Downloads/log/ 
$(command -v rm) -rf ./http-shellshock.nse

exit 0
