#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

int main( int argc, char *argv[] ) {
    FILE * f;
    char * line = NULL;
    size_t len = 0;
    ssize_t read1;
    char * ip;
    char * domain;
    char * req;
    int found;
    int flags;
    f= fopen ("database.txt","r");
    if (f == NULL)
        exit(EXIT_FAILURE);


   int sockfd, newsockfd, portno, clilen;
   char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   int  n;

   /* First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);

   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }

   /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = atoi(argv[1]);

   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);

   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }

   /* Now start listening for the clients, here process will
      * go in sleep mode and will wait for the incoming connection
   */

   listen(sockfd,5);
   printf("Server listening on all interfaces on port %d\n", portno);
   clilen = sizeof(cli_addr);

   while(1)
   {

       /* Accept actual connection from the client */
       newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

       if (newsockfd < 0) {
          perror("ERROR on accept");
          exit(1);
       }

       /* If connection is established then start communicating */
       bzero(buffer,256);
       n = recv( newsockfd,buffer,255,0);

       if (n < 0) {
          perror("ERROR reading from socket");
          exit(1);
       }

       found=0;
       rewind(f);
       printf("Request: %s", buffer);
        if(buffer[0]=='1')
        {
            req= strtok (buffer," ");
            req = strtok (NULL, " \n");

            while ((read1 = getline(&line, &len, f)) != -1) {
                  ip = strtok (line," ");
                  domain = strtok (NULL, " \n");

                  if(strcmp(req,domain)==0)
                  {
                        found=1;
                        break;
                  }
            }
            if(found==1)
            {
                char out[256];
                sprintf(out,"3 %s",ip);
                // n = send(newsockfd,"3 ",2,0);
                n = send(newsockfd,out,strlen(out),0);
            }
            else
            {
               n = send(newsockfd,"4 entry not found in the database",35,0);

            }
        }
        else if(buffer[0] == '2'){

             req= strtok (buffer," ");
            req = strtok (NULL, " \n");
            while ((read1 = getline(&line, &len, f)) != -1) {
                  ip = strtok (line," ");
                  domain = strtok (NULL, " \n");
                  if(strcmp(req,ip)==0)
                  {
                        found=1;
                        break;
                  }
            }
            if(found==1)
            {
                char out[256];
                sprintf(out,"3 %s",domain);
                // n = send(newsockfd,"3 ",2,0);
                n = send(newsockfd,out,strlen(out),0);
            }
            else
            {
                 n = send(newsockfd,"4 entry not found in the database",35,0);
            }
        }

        else {
            n = send(newsockfd,"ERROR in request format",23,0);
        }

       /* Write a response to the client */

       if (n < 0) {
          perror("ERROR writing to socket");
          exit(1);
       }
   }
   fclose(f);

   return 0;
}
