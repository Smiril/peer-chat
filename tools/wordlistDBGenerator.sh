#!/bin/bash
#!/dev/tty1
# wordlistdbgenerator v. 0.1

NOW=$(date +"%y-%m-%d");
ARGV=("$@")
ARGC=("$#")

if [ "$#" -ne 7 ]; then
    echo "USAGE: ${0} dbuser dbpassword db hostname port folder localuser"
    exit 0
fi

# Assign user and password
username1="${1}"
password1="${2}"
database1="${3}"
host="${4}"
mysqlport1="${5}"

# Assign local user for www presentation scan results
localuser="${7}"
report="/home/${localuser}/${6}/*.txt"
 
# List the parameter values passed.
echo "Hostname:  " ${host}
echo "    Port:  " ${mysqlport1}
echo "Username:  " ${username1}
echo "Password:  *********" 
echo "Database:  " ${database1}
echo ""

# Connect and pipe the query result minus errors and warnings to the while loop.
IP21=$(sudo cat $report | awk '/([a-z])/ {print $1}' | awk 'BEGIN {FS="("}; {print $2}' | awk 'BEGIN {FS=")"}; {print $1}' )
# Read through the piped result until it's empty but format the title.
items=$(echo ${IP21} | tr " " "\n")

for item in $items
do
    mysql -u${username1} -p${password1} -D${database1} -h${host} --port=${mysqlport1} -se "CREATE TABLE IF NOT EXISTS enigma${#item} (
    task_id INT AUTO_INCREMENT PRIMARY KEY,
    WORD VARCHAR(255) NOT NULL,
    start_date DATE,
    due_date DATE,
    status TINYINT NOT NULL,
    LEN TINYINT NOT NULL,
    MESSAGE TEXT,
    SEEN TIMESTAMP DEFAULT CURRENT_TIMESTAMP
)  ENGINE=INNODB;"
    mysql -u${username1} -p${password1} -D${database1} -h${host} --port=${mysqlport1} -se "INSERT INTO  enigma${#item} (WORD, MESSAGE,status, LEN, SEEN) values ('$item', 'test', '0', '${#item}' , now());"
done

echo ""
echo "Done feeding MYSQL Database ${database1}"
echo ""
echo "Report: " $report
echo ""

exit 0

