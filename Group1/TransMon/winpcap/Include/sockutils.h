 #ifndef __SOCKUTILS_H__
 #define __SOCKUTILS_H__
 
 
 
 // Definition for Microsoft Visual Studio
 #if _MSC_VER > 10
 #pragma once
 #endif
 
 #ifdef WIN32
 #include <winsock.h>
 //#include <ws2tcpip.h>
 #else
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netdb.h>  /* DNS lookup */
 #include <unistd.h> /* close() */
 #endif
 
 
 
 // Some minor differences between UNIX and Win32
 #ifdef WIN32
 
     #define SOCKET unsigned int 
 #else
 
     #define SOCKET int
 
     #define closesocket(a) close(a) 
 #endif
 
 
 
 
 
 // All this stuff is private
 #ifdef NDEBUG
 #define SOCK_ASSERT(msg, expr) ((void)0)
 #else
     #include <assert.h>
     #if (defined(WIN32) && defined(_MSC_VER))
         #include <crtdbg.h>             // for _CrtDbgReport
         // Use MessageBox(NULL, msg, "warning", MB_OK)' instead of the other calls if you want to debug a Win32 service
         // Remember to activate the 'allow service to interact with desktop' flag of the service
         #define SOCK_ASSERT(msg, expr) { _CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "%s\n", msg); fprintf(stderr, "%s\n", msg); assert(expr); }
    #else
         #define SOCK_ASSERT(msg, expr) { fprintf(stderr, "%s\n", msg); assert(expr); }
     #endif
 #endif
 
 /****************************************************
  *                                                  *
  * Exported functions / definitions                 *
  *                                                  *
  ****************************************************/
 
 #define SOCKBUF_CHECKONLY 1     
 #define SOCKBUF_BUFFERIZE 0     
 #define SOCKOPEN_CLIENT 0       
 #define SOCKOPEN_SERVER 1       
 #define SOCK_ERRBUF_SIZE 2     // end of private documentation
 
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 
 void sock_geterror(const char *caller, char *string, int size);
 int sock_init(char *errbuf);
 void sock_cleanup();
 int sock_validaddr(const char *address, const char *port,
                         struct addrinfo *hints, struct addrinfo **addrinfo, char *errbuf);
 int sock_recv(SOCKET socket, char *buffer, int size, char *errbuf);
 int sock_recv_dgram(SOCKET sock, char *buffer, int size, char *errbuf);
 int sock_open(struct addrinfo *addrinfo, int server, int nconn, char *errbuf);
 int sock_close(SOCKET sock, char *errbuf);
 
 int sock_send(SOCKET socket, const char *buffer, int size, char *errbuf);
 int sock_bufferize(const char *buffer, int size, char *tempbuf, int *offset, int totsize, int checkonly, char *errbuf);
 int sock_discard(SOCKET socket, int size, char *errbuf);
 int sock_check_hostlist(char *hostlist, const char *sep, struct sockaddr_storage *from, char *errbuf);
 int sock_cmpaddr(struct sockaddr_storage *first, struct sockaddr_storage *second);
 
 
 #ifdef __cplusplus
 }
 #endif
 
 
 #endif
 
