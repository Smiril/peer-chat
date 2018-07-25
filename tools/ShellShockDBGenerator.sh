#!/bin/bash
#!/dev/tty1
# crontab like: 25 12 * * * sh /home/ShellShockReportGenerator.sh user password db hostname port
# !!! have https://svn.nmap.org/nmap/scripts/http-shellshock.nse for nmap scanning in /home/http-shellshock.nse !!!
NOW=$(date +"%y-%m-%d");
ARGV=("$@")
ARGC=("$#")

if [ "$#" -ne 7 ]; then
    echo "USAGE: ${0} dbuser dbpassword db hostname port 2018-07-25 localuser"
    exit 0
fi

# Assign user and password
username1="${1}"
password1="${2}"
database1="${3}"
host_fx="${4}"
mysqlport1="${5}"

# Assign local user for recv www presentation scan results
localuser="${7}"
report=`cat /home/${localuser}/public_html/report_${6}_XXXXXX`
 
# List the parameter values passed.
echo "Hostname:  " ${host_fx}
echo "    Port:  " ${mysqlport1}
echo "Username:  " ${username1}
echo "Password:  *********" 
echo "Database:  " ${database1}
echo ""

# Connect and pipe the query result minus errors and warnings to the while loop.
IP21=$(cat $report | awk `/([0-9]{1,3}\.){3}[0-9]{1,3}/ {print $6}` | awk `BEGIN {FS="("}; {print $2}` | awk `BEGIN {FS=")"}; {print $1}` | grep -C 10 -v `|_http-shellshock:` | grep -C 20 `21/tcp open`)
# Read through the piped result until it's empty but format the title.
items=$(echo ${IP21} | tr " " "\n")

for item in $items
do
    mysql -u${username1} -p${password1} -D${database1} -h${host_xf} --port=${mysqlport1} -se "INSERT INTO tested_stage_2 (IP, PAGE, PORT, MESSAGE, SEEN) values ('$item', '/cgi-bin/bin', '21', 'vulnerable', now());"
done

# Connect and pipe the query result minus errors and warnings to the while loop.
IP80=$(cat $report | awk `/([0-9]{1,3}\.){3}[0-9]{1,3}/ {print $6}` | awk `BEGIN {FS="("}; {print $2}` | awk `BEGIN {FS=")"}; {print $1}` | grep -C 10 -v `|_http-shellshock:` | grep -C 20 `80/tcp open`)
# Read through the piped result until it's empty but format the title.
items=$(echo ${IP80} | tr " " "\n")

for item in $items
do
    mysql -u${username1} -p${password1} -D${database1} -h${host_xf} --port=${mysqlport1} -se "INSERT INTO tested_stage_2 (IP, PAGE, PORT, MESSAGE, SEEN) values ('$item', '/cgi-bin/bin', '80','vulnerable', now());"
done

# Connect and pipe the query result minus errors and warnings to the while loop.
IP443=$(cat $report | awk `/([0-9]{1,3}\.){3}[0-9]{1,3}/ {print $6}` | awk `BEGIN {FS="("}; {print $2}` | awk `BEGIN {FS=")"}; {print $1}` | grep -C 10 -v `|_http-shellshock:` | grep -C 20 `443/tcp open`)
# Read through the piped result until it's empty but format the title.
items=$(echo ${IP443} | tr " " "\n")

for item in $items
do
    mysql -u${username1} -p${password1} -D${database1} -h${host_xf} --port=${mysqlport1} -se "INSERT INTO tested_stage_2 (IP, PAGE, PORT, MESSAGE, SEEN) values ('$item', '/cgi-bin/bin', '443','vulnerable', now());"
done

# Connect and pipe the query result minus errors and warnings to the while loop.
IP3128=$(cat $report | awk `/([0-9]{1,3}\.){3}[0-9]{1,3}/ {print $6}` | awk `BEGIN {FS="("}; {print $2}` | awk `BEGIN {FS=")"}; {print $1}` | grep -C 10 -v `|_http-shellshock:` | grep -C 20 `3128/tcp open`)
# Read through the piped result until it's empty but format the title.
items=$(echo ${IP3128} | tr " " "\n")

for item in $items
do
    mysql -u${username1} -p${password1} -D${database1} -h${host_xf} --port=${mysqlport1} -se "INSERT INTO tested_stage_2 (IP, PAGE, PORT, MESSAGE, SEEN) values ('$item', '/cgi-bin/bin', '3128','vulnerable', now());"
done

# Connect and pipe the query result minus errors and warnings to the while loop.
IP8080=$(cat $report | awk `/([0-9]{1,3}\.){3}[0-9]{1,3}/ {print $6}` | awk `BEGIN {FS="("}; {print $2}` | awk `BEGIN {FS=")"}; {print $1}` | grep -C 10 -v `|_http-shellshock:` | grep -C 20 `8080/tcp open`)
# Read through the piped result until it's empty but format the title.
items=$(echo ${IP8080} | tr " " "\n")

for item in $items
do
    mysql -u${username1} -p${password1} -D${database1} -h${host_xf} --port=${mysqlport1} -se "INSERT INTO tested_stage_2 (IP, PAGE, PORT, MESSAGE, SEEN) values ('$item', '/cgi-bin/bin', '8080','vulnerable', now());"
done

# Connect and pipe the query result minus errors and warnings to the while loop.
IP8081=$(cat $report | awk `/([0-9]{1,3}\.){3}[0-9]{1,3}/ {print $6}` | awk `BEGIN {FS="("}; {print $2}` | awk `BEGIN {FS=")"}; {print $1}` | grep -C 10 -v `|_http-shellshock:` | grep -C 20 `8081/tcp open`)
# Read through the piped result until it's empty but format the title.
items=$(echo ${IP8081} | tr " " "\n")

for item in $items
do
    mysql -u${username1} -p${password1} -D${database1} -h${host_xf} --port=${mysqlport1} -se "INSERT INTO tested_stage_2 (IP, PAGE, PORT, MESSAGE, SEEN) values ('$item', '/cgi-bin/bin', '8081','vulnerable', now());"
done

echo ""
echo "Done feeding MYSQL Database \"${database1}\"!"
echo ""
echo "Report: $report"
echo ""

exit 0
