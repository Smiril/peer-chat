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

int action;
bool Done = false;

char SendBuffer[128];
int BytesReceived;
char Buffer[128];
 
void *connection_handler1(void *socket_desc);
void *connection_handler2(void *socket_desc);
int create_tcp_socket();
char *get_ip(char *host);

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
char *host;
char *port;
pthread_t sniffer_thread;

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
 
	printf("Do you want to be:\n1.) Server\n2.) Client\n3.) Exit\n");
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
      return 0;
        break;
	default:
		goto Start;
		break;
	}



	

	
	return 0;
}

void *connection_handler1(void *socket_desc){  
    int client_sock;
	sock2 = *(int*)new_sock;
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
    sock2 = socket(AF_INET , SOCK_STREAM , 0);
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
           
           printf("connected");
           printf("waiting for message...");
           //SERVER MESSAGING
           while(Done == false)
           {
            BytesReceived = 0;
            while(BytesReceived == 0)
            {
              BytesReceived = SSL_read(ssl,Buffer,strlen(Buffer));  
            }
            printf("%s",Buffer);
            scanf("%c",SendBuffer);
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
    //printf("\nPORT: ");
	//getchar();
	//scanf("%d",&port);
	//printf("\nHOST: ");
	//getchar();
	//scanf("%c",&host);
	
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
        			
           printf("Connected to Server.");
           printf("Enter your message.");
           //CLIENT MESSAGING
           while(Done == false)
           {
             BytesReceived = 0;
             scanf("%c",SendBuffer);
             SSL_write(ssl,SendBuffer,strlen(SendBuffer));
             while(BytesReceived == 0)
             {
               BytesReceived = SSL_read(ssl,Buffer,strlen(Buffer));
             }
             printf("%s",Buffer); 
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
struct sockaddr_in sa;
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
char buf[BUFSIZ+1];
char *host;
char *port;
char *appli;
pthread_t sniffer_thread;

int thread1(void){
		if(pthread_create(&sniffer_thread,NULL,connection_handler1,(void*) *new_sock) < 0){
			perror("could not create thread");
			return 1;
		}
	
		pthread_join(sniffer_thread,NULL);   	
}
int thread2(void){
		if(pthread_create(&sniffer_thread,NULL,connection_handler2,(void*) *new_sock) < 0){
			perror("could not create thread");
			return 1;
		}
	
		pthread_join(sniffer_thread,NULL);	
}

int main(int argc, char **argv)
{
	Start:	
	new_sock = (int*)malloc(1);
	*new_sock = create_tcp_socket();  
	printf("Do you want to be:\n1.) Server\n2.) Client\n3.) Exit\n");
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
	printf("HOST: ");
	getchar();
	host = getline2();    	
	printf("\x1b[0m");
	  thread2();
        break;
    case'3':
      return 0;
        break;
	default:
		goto Start;
		break;
	}



	

	
	return 0;
}

void *connection_handler1(void *socket_desc){  
    int client_sock;
	sock2 = *(int*)new_sock;
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
    sock2 = socket(AF_INET , SOCK_STREAM , 0);
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
           
           printf("connected");
           printf("waiting for message...");
           //SERVER MESSAGING
           while(Done == false)
           {
            BytesReceived = 0;
            while(BytesReceived == 0)
            {
              BytesReceived = SSL_read(ssl,Buffer,strlen(Buffer));  
            }
            printf("%s",Buffer);
            scanf("%c",SendBuffer);
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
    //printf("\nPORT: ");
	//getchar();
	//scanf("%d",&port);
	//printf("\nHOST: ");
	//getchar();
	//scanf("%c",&host);
	
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
        			
           printf("Connected to Server.");
           printf("Enter your message.");
           //CLIENT MESSAGING
           while(Done == false)
           {
             BytesReceived = 0;
             scanf("%c",SendBuffer);
             SSL_write(ssl,SendBuffer,strlen(SendBuffer));
             while(BytesReceived == 0)
             {
               BytesReceived = SSL_read(ssl,Buffer,strlen(Buffer));
             }
             printf("%s",Buffer); 
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

#ifdef __WIN32__
#define HOME "C:\\Users\\"
#else
#define HOME "/home/"
#endif

#define CERTF HOME "valid-root-ca.pem"
#define KEYF HOME "valid-root-cakey.pem"

#define PORT "5880"
#define HOST "cluster5.tuwien.ac.at"

int err;
int sd;
int socket_desc,sock2,*new_sock;
struct sockaddr_in serv_addr;
struct sockaddr_in sa;
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
char buf[BUFSIZ+1];
char *host;
char *port;
char *appli;
pthread_t sniffer_thread;

int thread1(void){
		if(pthread_create(&sniffer_thread,NULL,connection_handler1,(void*) *new_sock) < 0){
			perror("could not create thread");
			return 1;
		}
	
		pthread_join(sniffer_thread,NULL);   	
}
int thread2(void){
		if(pthread_create(&sniffer_thread,NULL,connection_handler2,(void*) *new_sock) < 0){
			perror("could not create thread");
			return 1;
		}
	
		pthread_join(sniffer_thread,NULL);	
}

int main(int argc, char **argv)
{
	Start:	
	new_sock = (int*)malloc(1);
	*new_sock = create_tcp_socket();  
	printf("Do you want to be:\n1.) Server\n2.) Client\n3.) Exit\n");
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
	printf("HOST: ");
	getchar();
	host = getline2();    	
	printf("\x1b[0m");
	  thread2();
        break;
    case'3':
      return 0;
        break;
	default:
		goto Start;
		break;
	}



	

	
	return 0;
}

void *connection_handler1(void *socket_desc){  
    int client_sock;
	sock2 = *(int*)new_sock;
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
    sock2 = socket(AF_INET , SOCK_STREAM , 0);
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
    serv_addr.sin_port = htons(atoi(PORT));
    
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
           
           printf("connected");
           printf("waiting for message...");
           //SERVER MESSAGING
           while(Done == false)
           {
            BytesReceived = 0;
            while(BytesReceived == 0)
            {
              BytesReceived = SSL_read(ssl,Buffer,strlen(Buffer));  
            }
            printf("%s",Buffer);
            scanf("%c",SendBuffer);
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
    //printf("\nPORT: ");
	//getchar();
	//scanf("%d",&port);
	//printf("\nHOST: ");
	//getchar();
	//scanf("%c",&host);
	
  ip = get_ip(HOST);
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
  remote->sin_port = htons(atoi(PORT));
 
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
        			
           printf("Connected to Server.");
           printf("Enter your message.");
           //CLIENT MESSAGING
           while(Done == false)
           {
             BytesReceived = 0;
             scanf("%c",SendBuffer);
             SSL_write(ssl,SendBuffer,strlen(SendBuffer));
             while(BytesReceived == 0)
             {
               BytesReceived = SSL_read(ssl,Buffer,strlen(Buffer));
             }
             printf("%s",Buffer); 
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

#ifdef __WIN32__
#define HOME "C:\\Users\\"
#else
#define HOME "/home/"
#endif

#define CERTF HOME "valid-root-ca.pem"
#define KEYF HOME "valid-root-cakey.pem"

#define PORT "5880"
#define HOST "cluster5.tu.ac.org"

int err;
int sd;
int socket_desc,sock2,*new_sock;
struct sockaddr_in serv_addr;
struct sockaddr_in sa;
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
char buf[BUFSIZ+1];
char *host;
char *port;
char *appli;
pthread_t sniffer_thread;

int thread1(void){
		if(pthread_create(&sniffer_thread,NULL,connection_handler1,(void*) *new_sock) < 0){
			perror("could not create thread");
			return 1;
		}
	
		pthread_join(sniffer_thread,NULL);   	
}
int thread2(void){
		if(pthread_create(&sniffer_thread,NULL,connection_handler2,(void*) *new_sock) < 0){
			perror("could not create thread");
			return 1;
		}
	
		pthread_join(sniffer_thread,NULL);	
}

int main(int argc, char **argv)
{
	size_t  n = 1024;
	 
	Start:	
	new_sock = (int*)malloc(1);
	*new_sock = create_tcp_socket();  
	printf("Do you want to be:\n1.) Server\n2.) Client\n3.) Exit\n");
    scanf("%c",&action);
    
	switch(action){

    case '1':
	  thread1();
	  break;
    case '2':
	  thread2();
        break;
    case'3':
      return 0;
        break;
	default:
		goto Start;
		break;
	}
	
	return 0;
}

void *connection_handler1(void *socket_desc){  
    int client_sock;
	sock2 = *(int*)new_sock;
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
    sock2 = socket(AF_INET , SOCK_STREAM , 0);
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
    serv_addr.sin_port = htons(atoi(PORT));
    
	if (bind(sock2, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
              perror("ERROR on binding");
			  }
    
    puts("bind done");
    
    listen(sock2 , 5);
    client_len = sizeof(serv_addr);
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
           
           printf("connected");
           printf("waiting for message...");
           //SERVER MESSAGING
           while(Done == false)
           {
            BytesReceived = 0;
            while(BytesReceived == 0)
            {
              BytesReceived = SSL_read(ssl,Buffer,strlen(Buffer));  
            }
            printf("%s",Buffer);
            scanf("%c",SendBuffer);
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
    //printf("\nPORT: ");
	//getchar();
	//scanf("%d",&port);
	//printf("\nHOST: ");
	//getchar();
	//scanf("%c",&host);
	
  ip = get_ip(HOST);
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
  remote->sin_port = htons(atoi(PORT));
 
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
        			
           printf("Connected to Server.");
           printf("Enter your message.");
           //CLIENT MESSAGING
           while(Done == false)
           {
             BytesReceived = 0;
             scanf("%c",SendBuffer);
             SSL_write(ssl,SendBuffer,strlen(SendBuffer));
             while(BytesReceived == 0)
             {
               BytesReceived = SSL_read(ssl,Buffer,strlen(Buffer));
             }
             printf("%s",Buffer); 
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
