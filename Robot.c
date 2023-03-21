// C program to implement one side of FIFO
// This side writes first, then s
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <time.h>
#include <stdint.h>

#define P4_COLONNES (6)
#define P4_LIGNES (4)

struct carte{
  int *valeurN;
  int *nbreTete;
};
static struct mainJoueur{
  char nomJoueur[30];
  struct carte *c1 ;
  int *score ;
}cartesJoueur;
int fd1_R;

// FIFO file path
char * myfifo_Robot = "/tmp/myfifoR";

// Creating the named file(FIFO)
// mkfifo(<pathname>, <permission>)
static int indiceCarte =10;
struct carte** tableau_carte_jouees_R ;
static unsigned int indice = 0;



static void initialise_cartesR();
static void affichePlateauR();

void Tour_a_Jouer_R();//recupère la carte à jouer par le joueur et envoie au Maitre du jeu
void affiche_cartes_joueur_R();//permet d'afficher les cartes que dispose en main un joeur
void receivoir_les_cartes_deja_jouees_R();
void remplir_tableau_carte_jouees_R();//permet de remplir le tableau d'affichage
void liberer_tableau_carte_jouees_R();//libère la memoire
void nouvelle_manche_R();//permet de dbuter une nouvelle manche en recuperant les dix nouvelles cartes
void reception_cartes_R();
void joue_R();

//initialisation de tous les cartes du joueur
static void initialise_cartesR()
{

  //ouverture en écriture et on écrit le nom du joueur
    fd1_R = open(myfifo_Robot,O_WRONLY);
    if (fd1_R<0) {
      perror("Erreur d'ouverture open");
      exit(0);
    }
    strcpy(cartesJoueur.nomJoueur ,"Robot");
    write(fd1_R,cartesJoueur.nomJoueur,sizeof(cartesJoueur.nomJoueur));
    close(fd1_R);
    reception_cartes_R();

}
void reception_cartes_R(/* arguments */) {
  fd1_R = open (myfifo_Robot, O_RDONLY);
  cartesJoueur.c1 = ( struct carte*) malloc( 10 * sizeof(struct carte) );
  for (int i =0;i<10; i++) {
    //allocation d'un espace memoire pour eviter "segmentation fault"
      cartesJoueur.c1[i].valeurN = malloc(sizeof(int));
      read(fd1_R,cartesJoueur.c1[i].valeurN,sizeof(cartesJoueur.c1[i].valeurN));
      cartesJoueur.c1[i].nbreTete = malloc(sizeof(int));
      //lecture du descripteur ,le nombre de tete de boeuf d'une carte i
      read(fd1_R,cartesJoueur.c1[i].nbreTete,sizeof(cartesJoueur.c1[i].nbreTete));
      //printf("%d %d\n",*cartesJoueur.c1[i].valeurN,*cartesJoueur.c1[i].nbreTete );
  }
  close(fd1_R);

}
//affichage du plateau du jeu côté joueur et Maitre du jeu
static void affichePlateauR()
{
  //receivoir_les_cartes_deja_jouees_R();
  int col;
  int lgn;

  putchar('\n');

  for (col = 1; col <= P4_COLONNES; ++col)
      printf("  %d ", col);

  putchar('\n');
  putchar('+');

  for (col = 1; col <= P4_COLONNES; ++col)
      printf("-------+");

  putchar('\n');

  for (lgn = 0; lgn < P4_LIGNES; ++lgn)
  {

      for (col = 0; col < P4_COLONNES; ++col)
          if (*tableau_carte_jouees_R[lgn][col].valeurN !=0) {

              printf(" %d  %d |",*(tableau_carte_jouees_R[lgn][col].valeurN),*(tableau_carte_jouees_R[lgn][col].nbreTete));
            }


      putchar('\n');
      putchar('+');

      for (col = 1; col <= P4_COLONNES; ++col)
          printf("-------+");

      putchar('\n');
  }
}
void affiche_cartes_joueur_R() {
  printf("Valeur de carte    nombre de tête de boeufs\n");
  for (int i = 0 ; i <indiceCarte; i++) {//on drevrait recupérer la taille du tableau
    if (*cartesJoueur.c1[i].valeurN !=0) {
      printf(" %d    \t\t\t   %d\n\n",*(cartesJoueur.c1[i].valeurN),*(cartesJoueur.c1[i].nbreTete));
    }

  }
}

void Tour_a_Jouer_R() {



  int *Valcarte_a_jouer= malloc(sizeof(int));
  for (int  i = 0; i < 10; i++) {
    if (*cartesJoueur.c1[i].valeurN !=0) {
      *Valcarte_a_jouer = *cartesJoueur.c1[i].valeurN;
      break;
      printf(" Carte jouée :%d\n",*Valcarte_a_jouer );
    }
  }

  int trouve = 1;int i=0;
  while (trouve==1 ||i<10 ) {//à changer avec la vraie taille du tableau des cartes
          if(*Valcarte_a_jouer==*cartesJoueur.c1[i].valeurN)
          {
                fd1_R = open(myfifo_Robot,O_WRONLY);//ouverture du fichier en ecriture pour envoyer la carte choisit par le joueur
                if (fd1_R<0) {
                    perror("Erreur d'ouverture open");
                    exit(0);
                }
              //  write(fd1_R,cartesJoueur.nomJoueur,sizeof(cartesJoueur.nomJoueur));//on envoie le nom pour la distinction
                write(fd1_R,cartesJoueur.c1[i].valeurN,sizeof(cartesJoueur.c1[i].valeurN));//envoie de la carte au Maitre
                write(fd1_R,cartesJoueur.c1[i].nbreTete,sizeof(cartesJoueur.c1[i].nbreTete));
                close(fd1_R);
                *cartesJoueur.c1[i].valeurN =0;
                *cartesJoueur.c1[i].nbreTete=0;


                trouve=0;
          }
          i++;
        }
        printf(" Carte jouée :%d\n",*Valcarte_a_jouer );
        sleep(10);
        affiche_cartes_joueur_R();
        indice++;
}

void remplir_tableau_carte_jouees_R()
{
  tableau_carte_jouees_R = (struct carte**) malloc( 4* sizeof(struct carte*));
  for (int h = 0; h < 6; h++) {
    tableau_carte_jouees_R[h] = (struct carte*) malloc( 6* sizeof(struct carte));
  }

    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 6; j++) {

      tableau_carte_jouees_R[i][j].valeurN = (int*) calloc(1,sizeof(int));
      tableau_carte_jouees_R[i][j].nbreTete = (int*) calloc(1, sizeof(int));
      }
    }


}
void liberer_tableau_carte_jouees_R(/* arguments */) {
  for (int h = 0; h < 6; h++) {
    for (int i = 0; i < 6; i++) {
      free(tableau_carte_jouees_R[h]->valeurN );
      free(tableau_carte_jouees_R[h]->nbreTete );
    }
    free(tableau_carte_jouees_R[h] );
  }
  free(tableau_carte_jouees_R);
}

void receivoir_les_cartes_deja_jouees_R(/* arguments */) {
  fd1_R = open (myfifo_Robot, O_RDONLY);
  for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 6; j++) {
          read(fd1_R,tableau_carte_jouees_R[i][j].valeurN ,sizeof(tableau_carte_jouees_R[i][j].valeurN));
          //lecture du descripteur ,le nombre de tete de boeuf d'une carte i
          read(fd1_R,tableau_carte_jouees_R[i][j].nbreTete,sizeof(tableau_carte_jouees_R[i][j].nbreTete));
          printf(" %d  %d |",*(tableau_carte_jouees_R[i][j].valeurN),*(tableau_carte_jouees_R[i][j].nbreTete));
        }
  }
  close(fd1_R);
}
void nouvelle_manche_R(/* arguments */) {
  fd1_R = open (myfifo_Robot, O_RDONLY);
  for (int i =0;i<10; i++) {

      read(fd1_R,cartesJoueur.c1[i].valeurN,sizeof(cartesJoueur.c1[i].valeurN));
      //lecture du descripteur ,le nombre de tete de boeuf d'une carte i
      read(fd1_R,cartesJoueur.c1[i].nbreTete,sizeof(cartesJoueur.c1[i].nbreTete));
      printf("%d %d\n",*cartesJoueur.c1[i].valeurN,*cartesJoueur.c1[i].nbreTete );
  }
  close(fd1_R);
}

//fonction mère
void joue_R()
{
    int flags=0;
    initialise_cartesR();
    //receivoir_les_cartes_deja_jouees_R();
    //affichePlateauR();
    affiche_cartes_joueur_R();

    while (1) {
      Tour_a_Jouer_R();
      if (indice%10==0) {

        nouvelle_manche_R();
      }
      if (flags) {
        liberer_tableau_carte_jouees_R();
        break;
      }
    }
    /*if (receive == 0 || strcmp(buff_out, "exit") == 0
			leave_flag = 1;
		}*/
}

int main()
{
mkfifo(myfifo_Robot, 0666);

printf("%s",cartesJoueur.nomJoueur );

  joue_R();

    return 0;
}
