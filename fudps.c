/***************************************************************************/
/* 					El Antri Abdellah 				   */
/*           			Sous licence GNU					   */
/***************************************************************************/

/**************************************************************************/
/*				Rmarque importante					  */
/* Le serveur créé des fichiers log pour chaque client et un historique   */
/* des connexions directement dans la racine / par consequent il faut     */
/* ilo faut avoir les privileges root                                     */
/**************************************************************************/

/* Marche sous linux/unix */
/* Compilation: cc fudps.c -o serveur -lpthreqd */
/* Execution: ./serveur 1200                    */

#include <time.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <dirent.h>            
#include <sys/stat.h>           
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>             
#include <unistd.h>           
#include <stdio.h>              

#define vrai 1
#define faux 0

// Cette constante permet de specifie la longueur
// maximal d'un chemin specifiant un repertoire
#define MAX_REP_CHEMIN 2048

// Cette constante desgine le nombre de thread autorise a cree
#define MAX_THREAD 4

// Ddefinie la long maximal du buffer
#define MAX_BUF 2048 


// Declaraation des variables globales utilise
int num_libre;

// Tableau d'identificateur
pthread_t th_id[MAX_THREAD];

// Tableau pour indique les identificateur deja pris
int th_occupe [MAX_THREAD];

// Fichier contenant l'historique des connexions et des dmandes de connexion
FILE *hist;

// Declaration des fonctions et procedures 
// utiliser par notre serveur

void section_critique(FILE *);
int cree_socket(int, FILE * );
char * cherche_fichier(char *, FILE *);
void * th_trait(void  *);


// Procedure principale 
int main (int argc , char ** argv)
{
// Descripteur du serveur 
int serveur;

// Adresse du client
// Utilise par recvfrom pour recevoir les
// informations du client qui a envoye une 
// demande de connexion

struct sockaddr_in client;

// Longueur de la structure utilise par client
int long_client;

// Pour le parcour du tabbleau des identificateurs des thread
int i; 

// utilise pour guarder la Valeur de retour de recvfrom
int recue;

// Utiliser pour guarder la valeur de retour de sendto
int envoye;

// Utilise pour la lecture de la demande de connexion
char *buf;
int long_buf;

// Pour lire la date
time_t temp;
// Contient le chemin du fichier historique
char nom_hist[MAX_REP_CHEMIN + 1];


system("clear");
printf("\n Arguments de la ligne de commande ... ");

if (argc != 2)
   {
   printf(" [FAILED]");
   printf("\n Usage: serveur num_proto\n");
   exit(1);
   }
printf(" [OK]");

strcpy(nom_hist, "/historique");
printf("\n Creation du fichier %s ... ",nom_hist);
hist = fopen(nom_hist,"w+");
if(hist == NULL)
  {
  printf(" [FAILED]\n");	  
  perror("Cause");
  exit(1);
  }	  
printf(" [OK]\n");	  

time(&temp);
fprintf(hist," -------------------------------------------------\n");
fprintf(hist,"| Ce fichier contient l'historique des connexions |\n");
fprintf(hist," -------------------------------------------------\n");

fprintf(hist," Date de creation: %s \n ", ctime(&temp) );

// Initialisation du buffer
// On va essayer d'alloue MAX_BUF octet

printf("\n Allocation du buffer ... ");
fprintf(hist, "\n Allocation du buffer ... ");
buf = (char *)malloc(MAX_BUF);
if(buf == NULL)
  {
  printf("\n Impossible d'alloue de l'espace pour le buffer");
  // Sans le buffer on ne peut rien faire
  exit(1);
  }
printf(" [OK]");
fprintf(hist, " [OK]");
       
// Initialisation du tableau des threads
printf("\n Initialisation du tableau d'id de thread ... ");
fprintf(hist, "\n Initialisation du tableau d'id de thread ... ");
for (i = 0; i < 4; i++)
   {
   th_occupe[i] = -1;
   }
printf(" [OK]");
fprintf(hist, " [OK]");

// Initialisation num_libre a 0
num_libre = 0;

// creation du socket
fprintf(hist, "\n Creation de socket sur le port %d... ",atoi(argv[1]));
printf("\n Creation de socket sur le port %d... ",atoi(argv[1]));
serveur = cree_socket(atoi(argv[1]),hist);
if(serveur == -1)
  {
  printf("\n Un probleme a interrompu la creation du socket.\n");
  exit(1);
  }

printf("\n \t  Le serveur va entre dans une boucle infini  \n");
printf("   \t  =========================================   ");

      ////////////////////////////////////////////////////////////
      //         Boucle de gestion des connexions               //
      ///////////////////////////////////////////////////////////

while(vrai)
{
fflush(stdout);//vidage de la sortie standard

printf("\n \t **************************************************");
printf("\n \t ***     Attente des demandes de connexion      ***");
printf("\n \t **************************************************");
printf("\n");
// Le serveur attend une demande de connexion 
fprintf(hist, "\n Attente de demande de connexion ... ");
long_buf = strlen("connect"); 
recue = 
       recvfrom(serveur, buf, long_buf , 0, 
		(struct sockaddr*)&client, &long_client);
    
if (recue < 0)
   {
   fprintf(hist, " [FAILED]\n");
   perror("Cause");
   // S'il y a une erreur de reception en arrete tout
   exit(1);
   }    
fprintf(hist, " [OK]");   

// On doit recupere les informations du client
// qui a demande une connexion
printf("\n %s:%u demande la connexion au serveur ...", 
	inet_ntoa(client.sin_addr),ntohs(client.sin_port));
time(&temp);
fprintf(hist, "\n  Une demande de connexion est recue:\n");
fprintf(hist, "  _==================================_\n");
fprintf(hist, "\t * Date de connexion:  %s", ctime(&temp));
fprintf(hist, "\t * Famille du Socket client: %x\n", ntohs(client.sin_family));
fprintf(hist, "\t * Port client: %u\n", ntohs(client.sin_port));
fprintf(hist, "\t * IP client:  %s\n", inet_ntoa(client.sin_addr));

if(strcmp(buf,"connect") == 0)
   {
   fprintf(hist, "\n Le client a envoye %s.", buf);	   
   fprintf(hist, " ...  Requete valide");
   }
   else
       {
       fprintf(hist, "\n Requete inconnu: Attendu  connect recue %s", buf);
       continue ;
       }

// Validation de la demande par un OK
// Ceci n'est pas necessaire en mode connecte
// car c'est la commande accepte qui realise cette tache
fprintf(hist, 
	    "\n Le serveur envoi une reponse au %s:%u ...",
	     inet_ntoa(client.sin_addr),ntohs(client.sin_port));
long_client = sizeof(client);

if(num_libre > 3)
   {
   strcpy(buf,"NO");
   long_buf = strlen("NO") + 1;
   envoye =
            sendto(serveur,buf,long_buf,0,
	    (struct sockaddr *)&client,
            long_client);
   if(envoye < 0)
     {
      fprintf(hist, " [FAILED]\n");
      perror("Cause");
      continue ;
      }
   printf("\n Connexion du %s:%u refuse: depacement de capacite \n",
                 inet_ntoa(client.sin_addr),ntohs(client.sin_port));
   fprintf(hist, " [OK]\n");
   fprintf(hist,"\n|**************************************************|");
   fprintf(hist,"\n DEPACEMENT DE  CAPACITE  DU SERVEUR       "); 
   fprintf(hist, "\n Connexion du client %s:%u refuse       ", 
          inet_ntoa(client.sin_addr),
	  ntohs(client.sin_port));
   fprintf(hist,"\n|**************************************************|");
   } 
    else
        {
        strcpy(buf,"OK");
        long_buf = strlen("OK") + 1;
        envoye =
                 sendto(serveur,buf,long_buf,0,
	         (struct sockaddr *)&client,
                 long_client);
        if(envoye < 0)
          {
           fprintf(hist," [FAILED]\n");
           perror("Cause");
           continue;
          }
        fprintf(hist, " [OK]\n");
        num_libre ++;

	// On cherche un identificateur libre
        for(i = 0; i < 4; i++)
	    if(th_occupe[i]  == -1)
	       {
	        // La structure special contient le nom de fichier
		// la chaine a recherche
		// et l'adresse du client
		// creation du thread associe au client
		fprintf(hist, "\n Creation de thread ... ");
                if(
		   pthread_create(&th_id[i], 
                		    NULL,(void *) th_trait, 
	                 	     (void *)&client)  != 0
		   )
		   {
		   fprintf(hist, " [FAILED]\n");
		   exit(1);
		   }
	       th_occupe[i] = th_id[i];   
               break;
               }
        } 
 freopen(nom_hist,"a+",hist);	
 }// while(true)

}

// Thread  dedie  pour la recherche de fichier
// Et la recherche des motifs dans un fichier

void * th_trait(void *client_addr)
{
 
// Descripteur de fichier 
// utilise par fopen pour la lecture de fichier
// cherche dans le cas ou il existe

FILE *df;

// Descripteur de fichier log pour chaque thread
FILE *log;
char nom_log[MAX_BUF];

// Descripteur du socket serveur
// car chaque thread va avoir un numero de port
// speciale pour communiquer avec sont client
// ceci n'est pas necessaire dans le mode connecte
// puisque accepet retourne un identificateur nouveau
// pour chaque demande de connexion

int serveur;

// longuer de la structure sockaddr client
int long_client;

// utilise pour recuperer les valeurs de retour
// des fonctions recvfrom et sendto
int envoye, recue;

// Tampon de lecture ecriture
char *buf;

// Pour specifie la longueur du bufer
int long_buf;

// Stocke le nom de fichier envoye par le client
char nom_fichier[MAX_REP_CHEMIN];

// Stocke le chemin d'acce au fichier
// retourne par la fonction recherche_fichier
char chemin[MAX_REP_CHEMIN];

// Stocke la chaine a cherche dans le fichier
char motif[MAX_BUF];
// ndice indiquant si le motif a ete trouve ou pas
int motif_trouve = -1;

// Pour le parcour du tableau de thread
int i;

// Pour compter le nombre des bloque envoye au client;
int num_bloc = 0;

// Utilise pour savoir la longueur lu par la fct read
int long_lu=0;
 
struct sockaddr_in *client = (struct sockaddr_in *)client_addr; 

// Pour obtenir le repertoire courant du travail
char rtc[MAX_REP_CHEMIN];

// Creation de thread reussi


sprintf(nom_log,"/serveur%s:%u.log", 
       inet_ntoa(client->sin_addr),ntohs(client->sin_port));
printf("\n -> Pour les details de transfert voir le fichier: %s", nom_log);

log = fopen(nom_log ,"w+");
if(log == NULL)
   {
   printf("\n Erreur d'ouverture du fichier log.\n");
   perror("Cause");
   pthread_exit(1);
   }

fprintf(log," -----------------------------------------------------\n");
fprintf(log,"| Fichier log generer automatiquement par le serveur |");
fprintf(log,"\n -----------------------------------------------------\n");

fprintf(log, "\n Le thread %d va s'occupe de la demande du client  %s\n",
       pthread_self(),inet_ntoa(client->sin_addr));

// Essayer d'allouer le buffer
buf = (char *)malloc(MAX_BUF); 
if (buf == NULL)
    {
    fprintf(log,"\n Impossible d'alloue le buffer.\n", 
           pthread_self());
    fprintf
	  (log, "\n Arret du trait associe su client %s:%u \n",
          inet_ntoa(client->sin_addr),
	  ntohs(client->sin_port)
	  );
    fclose(log);
    section_critique(log);
    pthread_exit(1);
    }
 
long_buf = MAX_BUF; 
long_client = sizeof(client);

// Creation de socket pour le thread
fprintf(log, " Creation de socket ... ",pthread_self());

// Creation d'un nouveau socket pour chaque thread
// avec un numero de port = num_port_client + 1

serveur = cree_socket(ntohs(client->sin_port)+1, log);
if(serveur < 0)
  {
   fclose(log);
   section_critique(log);
   pthread_exit(1);
  }

// Attente de messages des clients
// On attend un nom de fichier a cherhe 
fprintf(log, "\n Attente du nom de fichier a cherche ... ",
        pthread_self()); 
memset(buf,0,strlen(buf));
recue = 
        recvfrom(
		serveur, buf, long_buf , 0, 
		(struct sockaddr*)client, &long_client
		);
if (recue < 0)
    {
      fprintf(log, " [FAILED]");
      perror("Cause");
      fclose(log);
      close(serveur);
      section_critique(log);
      pthread_exit(1);
    }
  
fprintf(log, " [OK]");
strcpy(nom_fichier,buf);
fprintf(log, "\n Le fichier cherche est: %s", nom_fichier);

// On attend la motif a cherche dans le fichier

 fprintf(log, "\n Attente du motif a cherche ... ",pthread_self());
 memset(buf,0,strlen(buf));
 recue = 
	 recvfrom(
		serveur, buf, long_buf , 0, 
		(struct sockaddr*)client, &long_client
		);
 if (recue < 0)
    {
      fprintf(log," [FAILED]\n");
      perror("Cause");
      fclose(log);
      close(serveur);
      section_critique(log);
      pthread_exit(1);
    }
 strcpy(motif, buf);
 fprintf(log, " [OK]"); 
 fprintf(log, " \n Le motif cherche est: %s", motif); 

// Recherche du fichier et recuperation de sont chemin
// dans le cas ou il existe

 fprintf(log, "\n Recherche du fichier %s ... ",nom_fichier);
 
 memset(chemin,0,strlen(chemin));
 
 strcpy(chemin,cherche_fichier(nom_fichier,log));


 if(!strcmp(chemin,"NO_EXIST"))
   {
   fprintf(log, "\n Le fichier cherche n'existe pas ...  ");
   // On envoi un message pour prevenir le client
   // que le fichier qu'il a demande n'existe pas
   // dans le serveur
   fprintf(log, "\n Envoyer  'NO_EXIST' au client ... ");
   strcpy(buf,"NO_EXIST");
   long_buf = strlen("NO_EXIST") + 1;
   envoye =
           sendto(serveur,buf,long_buf,0,
           (struct sockaddr *)client,
            long_client);
    if(envoye < 0)
       {
       fprintf(log," [FAILED]");
       perror("Cause");
       }
  fprintf(log," [OK]");
  close(serveur);
  fclose(log);
  section_critique(log);
  pthread_exit(1);
  }	


// Si on est arrive ici, ca veut dir que le fichier existe
fprintf(log,"\n Le fichier existe et sont chemin est: %s\n",chemin);
printf("\n Le fichier existe et sont chemin est: %s\n",chemin);

fprintf(log, "\n Envoyer 'EXIST' au client ... ");
strcpy(buf,"EXIST");
long_buf = strlen("EXIST") + 1;

envoye =
         sendto(serveur,buf,long_buf,0,
         (struct sockaddr *)client,
          long_client);

if(envoye < 0)
  {
  perror("Cause");
  close(serveur);
  fclose(log);
  section_critique(log);
  pthread_exit(1);
  }

// On ouvre le fichier pour lecture 
// la fonction fopen nous retourne un descripteur 
// qui va etre utilise par la fonction fscanf;
fprintf(log, "\n Debut de transfert:");
fprintf(log, "\n ___________________");
df = fopen(chemin,"r+");

if(df != NULL)
  {

   // Tanque fin de fichier non atteinte on envoi un bloque de donne de taille
   // strlen(buf) au client qui possede l'adresse client
   while(!feof(df))
      {
      num_bloc ++; 
      if((long_lu = fread(buf,1,MAX_BUF,df)) < 0)
         {
         fprintf(log, "\n Erreur de lecture de fichier.\n");
         perror("Cause");
         break;         
         }
      buf[long_lu] = '\0';
      // La recherche du motif dans le buffer courant
      if(strstr(buf, motif))
	 {
	 motif_trouve = num_bloc;	 
	 }
      fprintf(log,"\n Lecture d'un bloque avec succe.");
      do
       {
        fprintf(log,
              "\n\t Envoi du bloc N %d ",num_bloc);
	long_buf = strlen(buf)+1;
        envoye = 
 	      sendto(
	 	   serveur,buf, long_buf, 0, 
	           (struct sockaddr *)client, long_client
		   );
        if(envoye < 0)
           {
           fprintf(log, "\n Erreur d'envoi de donne. \n");
           perror("Cause");
           continue;
           }
       fprintf(log,"\n\t Les donnees on ete envoyees avec succe.");
       fprintf(log, "\n Attente de validation par le client ... ");
       memset(buf,0,strlen(buf));
       recue = 
            recvfrom(serveur, buf, long_buf,0,
                      (struct sockaddr *)client, &long_client);
       if(recue < 0)
         {
         fprintf(log," [FAILED]");
         continue;
         }
       fprintf(log," [OK]");
       if (!strcmp(buf,"OK"))
          {
          fprintf(log, 
           "\n Le bloque N %d a ete recue par le client avec succe.",num_bloc);
          break;
          }
          else if(!strcmp(buf,"NO"))
              {
              fprintf(log,
                     "\n Le client a demande la retransmission du bloc N %d",
                     num_bloc);
              continue;
              }
        }while (vrai);   /* do  d'envoi de bloc*/

     }                 /* while(!feof(df))*/  
  }                    /*if(df != NULL)*/

   else 
        {
        fprintf(log, 
              "\n Un probleme est survennu lors de l'ouverture du fichier.");
        perror("Cause");
        strcpy(buf,"$ERREUR$");
        long_buf = strlen(buf);
        sendto(serveur, (char *) buf, long_buf, 0,
               (struct sockaddr *)client, long_client);       
        fclose(log);
        close(serveur);
        section_critique(log);
        pthread_exit(1);
        }

strcpy(buf, "$FIN$");
long_buf = strlen(buf) + 1;
fprintf(log,"\n Envoye $FIN$ au client %s:%u ... ", inet_ntoa(client->sin_addr),
		ntohs(client->sin_port));
envoye =
          sendto(serveur, (char *) buf, long_buf, 0,
          (struct sockaddr *)client, long_client);       
if(envoye < 0)
  {
  printf(" [FAILED]");
  fprintf(log, " [FAILED]");
  perror("CAUSE");
  }
  else 
    {
    fprintf(log, " [OK]");
    printf("\n Envoi du fichier a ete termine avec succe");
    fprintf(log, "\n\t\t  ------------------------  ");
    fprintf(log, "\n\t\t |Envoi du fichier termine| ");
    fprintf(log, "\n\t\t  ------------------------  ");

    fprintf(log,"\n Envoyer la reponse indiquant si le motif a ete trouve ...");
    if(motif_trouve >= 0)
     {
     memset(buf,0,MAX_BUF);
     // Le serveur renvoi une reponse indiquant le numero de bloque
     // qui contient la premiere occurence du motif cherche
     sprintf(buf,"\n Le motif cherche existe dans le bloc %d:",motif_trouve);
     long_buf = strlen(buf) + 1;
     }
      else 
          {
          sprintf(buf,"\n Le motif cherche n'existe pas dans le fichier.");
          long_buf = strlen(buf) + 1;
          long_buf = strlen("\n Le motif n'existe pas dans le fichier.") + 1;
          }
     envoye =
	      sendto(serveur, (char *) buf, long_buf, 0,
               (struct sockaddr *)client, long_client);       
     if(envoye < 0)
	{
	fprintf(log, " [FAILED]");
	}
        else
	     {	
              fprintf(log, " [OK]");	      
	      fprintf(log,"%s",buf); // Le motif existe ou n'existe pas
	      fprintf(log, " \n Reponse concernant le motif ... ");
              recue =
                      recvfrom(serveur, (char *) buf, long_buf, 0,
                      (struct sockaddr *)client, &long_client);       
               if(recue < 0)
                 {
	          fprintf(log, " [FAILED]");
                  perror("CAUSE");	
	           }
                  else
	             {
                      fprintf(log, " [OK]");
	              printf(
		      "\n La requete du client %s:%u a ete traite avec succe.",                            inet_ntoa(client->sin_addr),
			    ntohs(client->sin_port));
                      fprintf(log,
   		    "\n La requete du client %s:%u a ete traite avec succe.", 
                              inet_ntoa(client->sin_addr),
			      ntohs(client->sin_port));
	             }
             }
     }
fclose(log);
fclose(df);
close(serveur);
section_critique(log);

return NULL;
}


///////////////////////////////////////////////////////////////////////////
// Cette procedure permet de liberer un identificateur de decrementer   //
// le nombre de clients qui se trouve dans le serveur                   //
/////////////////////////////////////////////////////////////////////////

void section_critique(FILE *log)               
{
int i;
pthread_mutex_t mut;

// Initialisation du verrou
// Cette fonction retourne toujour 0
fprintf(log, "\n Initialisation du verrou ... ");  
pthread_mutex_init(&mut, NULL);
fprintf(log, " [OK]\n");


// Section critique
fprintf(log, "\n Debut de la section critique ... ");  
pthread_mutex_lock(&mut);
// Decrement le nombre de client
num_libre --;
// libere l'identificateur de thread
for(i = 0; i < 4; i ++)
   if(th_occupe[i] == pthread_self())
     th_occupe[i] = -1;
pthread_mutex_unlock(&mut);       
fprintf(log, "\n Fin de la section critique ... ");  
// Fin de la section critique
}

///////////////////////////////////////////////////////////////////////////
// Cette fonction permet de cree un Socket                               //
// Ca valeur de retour est un descripteur dans la table des descripteur  //
// du processus c'est pour cela qu'il faut un bind pour quelle soit     //
// definie aupres du systeme                                            // 
/////////////////////////////////////////////////////////////////////////


int cree_socket(int port, FILE *hist)
{

  struct sockaddr_in serveur_addr;
  int serveur;
  
  // Creation du socket
  serveur = socket(AF_INET, SOCK_DGRAM,0);
  if(serveur < 0)
    {
      printf(" [FAILED]");
      perror("Cause");
      return -1;
    }
  fprintf(hist, " [OK]");
  // La famille a laquelle appartient le socket
  serveur_addr.sin_family = AF_INET;
  
  // Adresse specifique qui permet de dir au systeme
  // que je suis en ecoute sur tout les adresse IP entrantes
  serveur_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  
  // Le port utilise par le socket
  serveur_addr.sin_port = htons(port);
 
 fprintf(hist, "\n Attachement du socket avec une adresse ip/port ... ");
 // Attachement du socket au systeme
  if(bind(serveur, (struct sockaddr *) &serveur_addr, sizeof(serveur_addr)) < 0)
    {
      fprintf(hist, " [FAILED]");
      perror("Cause");
      return -1;
    }
  fprintf(hist, " [OK]\n"); 
  return serveur;
}

///////////////////////////////////////////////////////////////////////////
// Cette fonction permet de cherche un fichier                           //
// dans l'arborescence du systemes et de voir si ce fichier              //
// contient la chaine recherche                                          //
///////////////////////////////////////////////////////////////////////////

char * cherche_fichier(char *nom, FILE * log)
{

    static int trouve = faux;

    // Pointeur sur descripteur de repertoire          
    DIR* rep;                            

    // Pointeur sur une entree repertoire              
    struct dirent* entree; 

    // Repertoire de travail courant                  
    char rtc[MAX_REP_CHEMIN + 1], sauv[MAX_REP_CHEMIN + 1];
    //Chemin complet de notre fichier
    static char chemin[MAX_REP_CHEMIN + 1];

    // Utilise par stat() pour teste le type des entree   
    struct stat rep_stat;                

     char pile[MAX_REP_CHEMIN][MAX_REP_CHEMIN];
     int som = 0;
     

strcpy(pile[som],"/");
som++;
do {
   som --;
   strcpy(sauv,pile[som]);
   if  (chdir(sauv) == -1)
    {
    fprintf(log, "\n %s",sauv);
    fprintf(log," \n Erreur lors du changement de repertoire ...");
    strcpy(chemin,"NO_EXIST");
    return chemin;
    }
      
   fprintf(log, "\n Recherche dans %s: ", sauv);
   
   // ouvrir le repertoir pour lecture 
   rep = opendir(".");
   if (!rep) 
        {
	fprintf(log, "\n Impossible de lire le repertoire %s.\n ", sauv);
	perror("Cause");
	strcpy(chemin,"NO_EXIST");
	return chemin;
        } 

    ///////////////////////////////////////////////////////////////////////
    //  Parcour du repertoire courant ainsi que tout les sous repertoire //
    ///////////////////////////////////////////////////////////////////////    

    do
    {
     entree = readdir(rep);
     if(entree == NULL)
       {
       fprintf(log, "\n Le parcour de %s est termine.\n",sauv);
       break;
       }  

     if(entree != NULL)
        {
        fprintf(log, "\n Entree courante est: %s", entree->d_name);
	
	if ( entree->d_name && !strcmp(entree->d_name,nom)) 
            {
            if (getcwd(rtc, MAX_REP_CHEMIN) == NULL) 
                {
	         perror("Cause");
	         strcpy(chemin,"NO_EXIST");
	         return chemin;
                 }
	    if(strcmp(rtc,"/"))
	        strcat(rtc,"/"); 
	    strcat(rtc,entree->d_name);
	    strcpy(chemin,rtc);
	    // retourne le chemin d'acce complet du fichier 
             fprintf(log, "\n Le chemin du fichier cherche est: %s\n",
                 chemin);
 	     return chemin;
	    }
        
        // on evite les repertoires systemes
        // si vous voulez effectue une recherche dans ses repertoires
        // il faut les supprimes, mais le temps de recherche augmentera
        // considerablement
        if(strcmp(entree->d_name,"Recycled") == 0)
            continue;
        if(strcmp(entree->d_name,"dev") == 0)
            continue;
        if(strcmp(entree->d_name,"proc") == 0)
            continue;
        if(strcmp(entree->d_name,"root") == 0)
            continue;
        if(strcmp(entree->d_name,"mnt") == 0)
            continue;
        if(strcmp(entree->d_name,"lib") == 0)
            continue;
        if(strcmp(entree->d_name,"usr") == 0)
            continue;
        if(strcmp(entree->d_name,"etc") == 0)
            continue;
	if (strcmp(entree->d_name, ".") == 0)
	    continue;
	if (strcmp(entree->d_name, "..") == 0)
	    continue;

        if(entree->d_name != NULL)
          if ( stat(entree->d_name, &rep_stat) == -1)
             {
             continue;
             }
	// On teste si l'entre courante est un repertoir
        if (S_ISDIR(rep_stat.st_mode))
           {
	    if(som < MAX_REP_CHEMIN )
	        {
                if (getcwd(rtc, MAX_REP_CHEMIN) == NULL) 
                   {
	           perror("Cause");
	           strcpy(chemin,"NO_EXIST");
	           return chemin;
                   }
		if(strcmp(rtc,"/"))
		   strcat(rtc,"/"); 
		strcat(rtc,entree->d_name);
		if(strcmp(rtc, sauv) != 0) // Eviter les autoreferences
		  {
	   	  strcpy(pile[som],rtc);
	          som++;
		  }
	        }
	        else
		   {
		   fprintf(log, "\n Pile pleine ... arret de recherche.");
                   strcpy(chemin,"NO_EXIST");
	           return chemin;
		   }
         } // if (S_DIR)
       } // if(entree =! null )
    }while(vrai); 
  closedir(rep);
  }while(som > 0);

strcpy(chemin,"NO_EXIST");
return chemin;
}
