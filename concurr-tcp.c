#include<stdio.h>
#include<sys/types.h>//socket
#include<sys/socket.h>//socket
#include<string.h>//memset
#include<stdlib.h>//sizeof
#include<netinet/in.h>//INADDR_ANY

#define PORT 4950
#define MAXSZ 100

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main()
{
 int listenfd;//to create socket
 int connfd;//to accept connection

 struct sockaddr_in serverAddress;//server receive on this address
 struct sockaddr_in clientAddress;//server sends to client on this address
 struct sockaddr_storage their_addr;
 
 int n;
 char msg[MAXSZ];
 int clientAddressLength;
 int pid;

 char s[INET6_ADDRSTRLEN];
	
 //create socket
 listenfd=socket(AF_INET,SOCK_STREAM,0);
 //initialize the socket addresses
 memset(&serverAddress,0,sizeof(serverAddress));
 serverAddress.sin_family=AF_INET;
 serverAddress.sin_addr.s_addr=htonl(INADDR_ANY);
 serverAddress.sin_port=htons(PORT);

 //bind the socket with the server address and port
 bind(listenfd,(struct sockaddr *)&serverAddress, sizeof(serverAddress));

 //listen for connection from client
 listen(listenfd,5);

 while(1) {
   //parent process waiting to accept a new connection
   printf("\n*****server waiting for new client connection:*****\n");
   clientAddressLength=sizeof(clientAddress);
   connfd=accept(listenfd,(struct sockaddr*)&clientAddress,&clientAddressLength);
   printf("accept = %d \n", connfd );
   //printf("connected to client: %s\n",inet_ntoa(clientAddress.sin_addr));
   
   
   printf("listener: got packet from %s:%d\n",
	  inet_ntop(clientAddress.sin_family,
		    get_in_addr((struct sockaddr *)&clientAddress),
		    s, sizeof s),ntohs(clientAddress.sin_port));
   //child process is created for serving each new clients
   pid=fork();
   if(pid==0)//child process rec and send
     {
       printf("Im the child.\n");
       //rceive from client
       while(1){
	 n=recv(connfd,msg,MAXSZ,0);
	 printf("Child: recv(%d) .\n", n);
	 if(n==0){
	   close(connfd);
	   break;
	 }
	 msg[n]=0;
	 send(connfd,msg,n,0);
	 
	 printf("Receive and set:%s\n",msg);
       }//close interior while
       exit(0);
     }
   else {
     printf("Parent, close connfd().\n");
     close(connfd);//sock is closed BY PARENT
   }
 }//close exterior while
 
 return 0;
}
