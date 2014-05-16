/***************************************************************************/
/* 					El Antri Abdellah 				   */
/*           			Sous licence GNU					   */
/***************************************************************************/

/* Pour la plate formae unix/linux    		    */
/* Compilation: cc fudpc.c -o client  		    */
/* Execution: ./client adresse_serveur num_protocol */


#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <dirent.h>            
#include <sys/stat.h>           
#include <string.h>             
#include <unistd.h>           
#include <stdio.h>              

#define vrai 1
#define faux 0

// Numero du port du serveur
#define SERV_PORT 1200

// taille maximal du buffer
#define MAX_BUF 2048

// Descripteur du fichier cree pour la reception
FILE *fd;

// Le nom sous lequel le fichier transferer sera garder
char nom_fich_recu[MAX_BUF];

// Pour compter les bloques recues
int num_bloc = 0;

// Fonctions utilise par le client
int cree_socket(int);


////////////////////////////////////////////////////////////////
//                 Procedure principale                       //
///////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
// Buffer pour la lecture et l'ecriture dans le socket
char *buf;
int long_buf;

//Descripteur du client
int client;

// Utilise par sendto pour garder la valeur de retour
int envoye;

// Guarde la valeur de retour de la fonction recvfrom
int recue;

// Pour la construction de l'adresse du serveur
struct sockaddr_in serveur;
int long_serveur;

// Utilise pour la resolution des non des domaine (DNS)
// Conversion entre nom_machine->adresse_ip
struct hostent *h;

// Variable indiquant qu'on a atteint la fin de fichoer
int fin = faux;

struct in_addr *addr_ip_serv;

printf("\n Verfication des arguments de la ligne de commande ... ");
if (argc != 3)
   {
   printf(" [FAILED]");
   printf("\n Usage:  client nom_serveur num_proto\n");
   exit(1);
   }
printf(" [OK]");   

// creation du socket
printf(" \n Creation du socket ... ");
client = cree_socket(atoi(argv[2]));
if(client < 0)
 {
  perror("Cause");
  exit(1);
 }

// Allocation du buffer
printf(" \n Allocation du buffer ... ");
buf = (char *)malloc(MAX_BUF);
if(buf == NULL)
  {
  printf(" [FAILED]\n");  
  perror("Cause");
  exit(1);
  }	
printf(" [OK]\n");

// Le client envoi une demande de connexion 
long_buf = strlen("connect"); 

strcpy(buf,"connect");

// On construit l'adresse du serveur
printf("\n Construction de l'adresse du serveur ... ");

serveur.sin_family = AF_INET;
serveur.sin_port = htons(SERV_PORT);
printf(" [OK]");
printf("\n Resolution du nom de serveur %s ... ", argv[1]);
h = gethostbyname(argv[1]);
if (h == NULL)
    {
    printf(" [FAILED]\n");
    perror("Cause");
    exit(1);
    }
printf(" [OK]\n");

addr_ip_serv = (struct in_addr *)h->h_addr_list[0];

printf("\n Adresse ip du serveur est: %s", 
	inet_ntoa(*addr_ip_serv));

memcpy((char*)(&serveur.sin_addr.s_addr), h->h_addr_list[0], h->h_length);
long_serveur = sizeof(serveur);

// On va envoye une demande de connexion au serveur
// notre protocole consiste a envoye un message contenant
// la chaine de caractere "connect"
// ceci n'est pas necessaire en mode connecte
// car c'est la fonction connect qui assure cette tache
 
printf("\n Demande de connexion au serveur ... ");
envoye =
       	sendto(client, buf, long_buf , 0, 
		(struct sockaddr *)&serveur,
		long_serveur);
if (envoye < 0)
   {
   printf(" [FAILED]");
   perror("Cause");
   // S'il y a une erreur de reception en arrete tout
   exit(1);
   }
printf(" [OK]");   
printf("\n Attente de validation  ou refus de connexion ... ");

memset(buf,0,strlen(buf));
recue = 
	recvfrom(client, buf, long_buf, 0, 
		(struct sockaddr *)&serveur,
		&long_serveur);
if (recue < 0)
   {
   printf(" [FAILED]");
   perror("Cause:");
   exit(1); 
   }   
if(strcmp(buf,"OK") != 0 && strcmp(buf,"NO") != 0)
  {
  printf(" [OK] \n");
  printf("\n Attendu OK ou NO recue %s.\n", buf);
  exit(1);
  }

if(strcmp(buf,"NO") == 0)
  {
  printf(" [FAILED]");
  printf("\n\t\t !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!   ");   
  printf("\n\t\t !!! Le serveur est charge  !!!   ");   
  printf("\n\t\t !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");   
  close(client);
  exit(1);
  }
if(strcmp(buf,"OK") == 0)
  {
  printf(" [OK]");
  printf("\n\t\t ***************************************");   
  printf("\n\t\t *** Le serveur a accepte la demande ***");   
  printf("\n\t\t ***************************************");   
  }

// Changement du port du serveur
// pour devenir celui du thread associe 

serveur.sin_port = htons(atoi(argv[2])+1);

// On envoi le nom de fichier a cherche 
printf("\n Entrer le nom de fichier a cherche: ");
scanf("%s",buf);
long_buf = strlen(buf) + 1;

sprintf(nom_fich_recu, "copie_de_%s",buf);

printf("\n Envoi du nom de fichier ... "); 
envoye =
       	sendto(client, buf, long_buf , 0, 
		(struct sockaddr *)&serveur,
		long_serveur);
 
if(envoye < 0)
   {
   printf(" [FAILED]");
   perror("Cause");
   close(client);
   exit(1);
   }	
printf(" [OK]"); 


// On envoi la chaine a recherche  dans le fichier  

printf("\n Entrer le motif a cherche: ");
memset(buf,0,strlen(buf));
scanf("%s",buf);
long_buf = strlen(buf);
printf("\n Envoi du motif ... "); 
envoye =
       	sendto(client, buf, long_buf , 0, 
		(struct sockaddr *)&serveur,
		long_serveur);
 
if(envoye < 0)
   {
   printf(" [FAILED]\n");
   perror("Cause");
   close(client);
   exit(1);
   }	
printf(" [OK]"); 

// Reception de la reponse pour l'existence du fichier
printf("\n Attente de la reponse d'existance du fichier ...");
memset(buf,0,strlen(buf));
long_buf = MAX_BUF;
recue = 
	recvfrom(client, buf, long_buf, 0, 
		(struct sockaddr *)&serveur,
		&long_serveur);
if (recue < 0)
   {
   printf(" [FAILED]");
   perror("Cause");
   close(client);
   exit(1); 
   }   
printf(" [OK]");
if(!strcmp(buf,"EXIST"))
  {
  printf("\n Le serveur a trouve le fichier cherche. \n");  
  }
  else if(!strcmp(buf,"NO_EXIST"))
     {
     printf("\n Le serveur n'a pas reussi a trouve le fichier.\n");
     close(client);
     exit(1);
     }
    else
       {
       printf(" \n Attendu 'EXIST' ou 'NO_EXIST': Recue %s \n",buf);
       close(client);
       exit(0);
       }


fd = fopen(nom_fich_recu,"w+");
printf("\n ****************************************************************   ");
printf("\n *                     Reception du contenu                     *   ");
printf("\n **************************************************************** \n");
long_buf = MAX_BUF;
if(fd == NULL)
  {
  printf("\n Impossible de cree un fichier pour la reception des donnees recu");
  perror("Cause");
  close(client);
  exit(1);
  }
else 
  do /* tanque non fin de fichier */
    {
    num_bloc ++;
    do /* Tanque un bloque n'a pas ete recue */
     {
     printf("\n - Attente du bloque de donnee N%d... ",num_bloc);
    // Effacer le contenu du buffer
    long_buf = MAX_BUF;
    memset(buf,0,MAX_BUF);
    recue = 
	recvfrom(client, buf, long_buf, 0, 
		(struct sockaddr *)&serveur,
		&long_serveur);
    if(recue < 0)
      {
       printf(" [FAILED]\n");
       printf("\n Erreur lors de la reception des donnees. \n");
       perror("Cause");
       strcpy(buf,"NO");
       long_buf = strlen("NO")+1;
       printf("\n Envoye une demande de retransmission ... ");
       envoye = 
	       sendto(client, buf, long_buf, 0, 
               (struct sockaddr *)&serveur, long_serveur);
        if(envoye < 0)
           {
           printf(" [FAILED]");
           perror("Cause");
	   printf("\n Il y a eu une erreur lors de la reception du bloque ");
	   printf("\n et une erreur lors de la demande de retransmission. ");
	   printf("\n Par consquent le bloque envoye par le serveur est sera perdu. ");
	   break;  // Puisque on a pas pu prevenir le serveur qu'il y eu une erreur
	           // on evite le bloque actuel et en previent le client qu'il y a
		   // une perte de donnees
           }
	
        printf(" [OK]");
        continue;
       } /* if recue < 0 */

    printf(" [OK]");
    break; // Cassage de la boucle de retransmission

    }while(vrai); /* Reception du bloque */
    
    if(!strcmp(buf,"$FIN$")  || !strcmp(buf, "$ERREUR$"))
       fin = vrai;
    
    // Ecrire les donnees recu dans le fichier qui porte comme nom
    // client-num_port
    
    if(fin == faux) // Car $FIN$, $ERREUR$ ne font pas partie du fichier transferer
    if(fprintf(fd,"%s",buf) < 0)
        {
         printf("\n Erreur d'ecriture dans le fichier de reception. \n");
         perror("Cause");
         exit(1);
        }
    
    // On previent le serveur que le bloque qu'il a envoye
    // a ete recue avec succe en lui envoyant un OK
    strcpy(buf,"OK");
    long_buf = strlen("OK")+1;
    printf("\n * Envoye un 'OK' au serveur ... "); 
    envoye = 
	    sendto(client, buf, long_buf,0,
                    (struct sockaddr *)&serveur, long_serveur);
    if(envoye < 0) 
      {
       printf(" [FAILED]");
       perror("Cause");
       continue;
      }
    printf(" [OK]");
  
    }while(fin == faux);  /* !feof(df) */     

// On attend la reponse concernant le motif //
printf("\n Attente de la recpetion concernant le motif ... ");
long_buf = MAX_BUF;
memset(buf,0,MAX_BUF);
recue = 
	recvfrom(client, buf, long_buf, 0, 
	(struct sockaddr *)&serveur,&long_serveur);
if(recue < 0)
  {
  printf(" [FAILED]");	  
  perror("Cause");
  }	  
  else
     {
      printf(" [OK]");
      printf(" %s",buf);
      printf("\n Envoye un OK au serveur ... ");
      strcpy(buf,"OK");
      long_buf = strlen(buf) +1;
      envoye = 
	       sendto(client, buf, long_buf,0,
                      (struct sockaddr *)&serveur, long_serveur);
       if(envoye < 0) 
         { 
         printf(" [FAILED]");
         perror("Cause");
         }
       printf(" [OK]");
       printf(" \n Le transfert est termine avec succe.");
       printf(" \n Ouvrir le fichier %s pour voir le resultat de la recpetion.",
		       nom_fich_recu);
     }


fclose(fd);
close(client);
printf("\n");
return 0;
}

//////////////////////////////////////////////////////////////////
//Procedure aui permet la creation d'une socket                //
//Elle attend le numero de port                               //
//et elle nou renvoi un descripteur                          //
//////////////////////////////////////////////////////////////

int cree_socket(int port)
{

  struct sockaddr_in serveur_addr;
  int serveur;
  
  // Creation du socket
  serveur = socket(AF_INET, SOCK_DGRAM, 0);
  if(serveur < 0)
    {
      perror(" [FAILED] \n");
      return(-1);
    }
  printf(" [OK]\n"); 
  // La famille a laquelle appartient le socket
  serveur_addr.sin_family = AF_INET;
  
  // Adresse specifique qui permet de dir au systeme
  // que je suis en ecoute sur tout les adresse IP entrantes
  serveur_addr.sin_addr.s_addr = htons(INADDR_ANY);
  
  // Le port utilise par le socket
  serveur_addr.sin_port = htons(port);

  // Attachement du socket au systeme
   printf("\n Attachement de la socket cree avec le systeme ... ");
  if(bind(serveur, (struct sockaddr *) &serveur_addr, sizeof(serveur_addr)) < 0)
    {
      printf(" [FAILED]\n");
      perror("Cause");
      return(-1);
    }
  printf(" [OK]"); 
  return serveur;
}
