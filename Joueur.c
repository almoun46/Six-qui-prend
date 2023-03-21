
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
int fd1_J;

// FIFO file path
char * myfifo_J = "/tmp/myfifo";

// Creating the named file(FIFO)
// mkfifo(<pathname>, <permission>)
static int indiceCarte =10;
struct carte** tableau_carte_jouees_H ;
static unsigned int indice =0;//permet d'indiquer si une manche à termniner pour attendre les dix autres cartes du Maitre


static void initialise_cartesJ();//envoie le nom du joueur recuperée au clavier et recupére les dix première carte du jeu
static void affichePlateauJ();//affiche les cartes en main du joueur
void nouvelle_manche();//permet de remplir la main du joueur par les nouvelles cartes qui seront transmis par le Maitre
void Tour_a_Jouer();//recupère la carte à jouer par le joueur et envoie au Maitre du jeu
void affiche_cartes_joueur();//permet d'afficher les cartes que dispose en main un joeur
void receivoir_les_cartes_deja_jouees_H();
void remplir_tableau_carte_jouees_H();//permet de remplir le tableau d'affichage
void liberer_tableau_carte_jouees_H();//libère la memoire
void reception_cartes();//permet la lecture des 10 cartes envoyées par le Maitre
void joue();//permet de lancer le jeu

//initialisation de tous les cartes du joueur
static void initialise_carteJ()
{

  //ouverture en écriture et on écrit le nom du joueur
    fd1_J = open(myfifo_J,O_WRONLY);
    if (fd1_J<0) {
      perror("Erreur d'ouverture open");
      exit(0);
    }

    write(fd1_J,cartesJoueur.nomJoueur,sizeof(cartesJoueur.nomJoueur));
    close(fd1_J);
    reception_cartes();

}
void reception_cartes(/* arguments */) {
  fd1_J = open (myfifo_J, O_RDONLY);
  cartesJoueur.c1 = ( struct carte*) malloc( 10 * sizeof(struct carte) );
  for (int i =0;i<10; i++) {
    //allocation d'un espace memoire pour eviter "segmentation fault"
      cartesJoueur.c1[i].valeurN = malloc(sizeof(int));
      read(fd1_J,cartesJoueur.c1[i].valeurN,sizeof(cartesJoueur.c1[i].valeurN));
      cartesJoueur.c1[i].nbreTete = malloc(sizeof(int));
      //lecture du descripteur ,le nombre de tete de boeuf d'une carte i
      read(fd1_J,cartesJoueur.c1[i].nbreTete,sizeof(cartesJoueur.c1[i].nbreTete));
      printf("%d %d\n",*cartesJoueur.c1[i].valeurN,*cartesJoueur.c1[i].nbreTete );
  }
  close(fd1_J);
}
//affichage du plateau du jeu côté joueur et Maitre du jeu
static void affichePlateauJ()
{
  //receivoir_les_cartes_deja_jouees_H();
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
          if (*tableau_carte_jouees_H[lgn][col].valeurN !=0) {

              printf(" %d  %d |",*(tableau_carte_jouees_H[lgn][col].valeurN),*(tableau_carte_jouees_H[lgn][col].nbreTete));
            }


      putchar('\n');
      putchar('+');

      for (col = 1; col <= P4_COLONNES; ++col)
          printf("-------+");

      putchar('\n');
  }
}

void nouvelle_manche(/* arguments */) {
  fd1_J = open (myfifo_J, O_RDONLY);
  for (int i =0;i<10; i++) {
    //allocation d'un espace memoire pour eviter "segmentation fault"

      read(fd1_J,cartesJoueur.c1[i].valeurN,sizeof(cartesJoueur.c1[i].valeurN));

      //lecture du descripteur ,le nombre de tete de boeuf d'une carte i
      read(fd1_J,cartesJoueur.c1[i].nbreTete,sizeof(cartesJoueur.c1[i].nbreTete));
      printf("%d %d\n",*cartesJoueur.c1[i].valeurN,*cartesJoueur.c1[i].nbreTete );
  }
  close(fd1_J);
}
void affiche_cartes_joueur() {
  printf("Valeur de carte    nombre de tête de boeufs\n");
  for (int i = 0 ; i <indiceCarte; i++) {//on drevrait recupérer la taille du tableau
    if (*cartesJoueur.c1[i].valeurN !=0) {
      printf(" %d    \t\t\t   %d\n\n",*(cartesJoueur.c1[i].valeurN),*(cartesJoueur.c1[i].nbreTete));
    }

  }
}

void Tour_a_Jouer() {
  affiche_cartes_joueur();
  printf("Donner le numéro de la carte à jouer parmi vos cartes : \n");
  int *Valcarte_a_jouer= malloc(sizeof(int));
  scanf("%d",Valcarte_a_jouer);
  printf("%d\n",*Valcarte_a_jouer );
  int trouve = 1;int i=0;
  while (trouve==1 ||i<10 ) {//à changer avec la vraie taille du tableau des cartes
          if(*Valcarte_a_jouer==*cartesJoueur.c1[i].valeurN)
          {
                fd1_J = open(myfifo_J,O_WRONLY);//ouverture du fichier en ecriture pour envoyer la carte choisit par le joueur
                if (fd1_J<0) {
                    perror("Erreur d'ouverture open");
                    exit(0);
                }
              //  write(fd1_J,cartesJoueur.nomJoueur,sizeof(cartesJoueur.nomJoueur));//on envoie le nom pour la distinction
                write(fd1_J,cartesJoueur.c1[i].valeurN,sizeof(cartesJoueur.c1[i].valeurN));//envoie de la carte au Maitre
                write(fd1_J,cartesJoueur.c1[i].nbreTete,sizeof(cartesJoueur.c1[i].nbreTete));
                close(fd1_J);
                *cartesJoueur.c1[i].valeurN =0;
                *cartesJoueur.c1[i].nbreTete=0;


                trouve=0;
          }
          i++;
        }

        indice++;
}

void remplir_tableau_carte_jouees_H()
{
  tableau_carte_jouees_H = (struct carte**) malloc( 4* sizeof(struct carte*));
  for (int h = 0; h < 6; h++) {
    tableau_carte_jouees_H[h] = (struct carte*) malloc( 6* sizeof(struct carte));
  }

    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 6; j++) {

      tableau_carte_jouees_H[i][j].valeurN = (int*) calloc(1,sizeof(int));
      tableau_carte_jouees_H[i][j].nbreTete = (int*) calloc(1, sizeof(int));
      }
    }


}
void liberer_tableau_carte_jouees_H(/* arguments */) {
  for (int h = 0; h < 6; h++) {
    for (int i = 0; i < 6; i++) {
      free(tableau_carte_jouees_H[h]->valeurN );
      free(tableau_carte_jouees_H[h]->nbreTete );
    }
    free(tableau_carte_jouees_H[h] );
  }
  free(tableau_carte_jouees_H);
}

void receivoir_les_cartes_deja_jouees_H(/* arguments */) {
  fd1_J = open (myfifo_J, O_RDONLY);
  for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 6; j++) {
          read(fd1_J,tableau_carte_jouees_H[i][j].valeurN ,sizeof(tableau_carte_jouees_H[i][j].valeurN));
          //lecture du descripteur ,le nombre de tete de boeuf d'une carte i
          read(fd1_J,tableau_carte_jouees_H[i][j].nbreTete,sizeof(tableau_carte_jouees_H[i][j].nbreTete));
          printf(" %d  %d |",*(tableau_carte_jouees_H[i][j].valeurN),*(tableau_carte_jouees_H[i][j].nbreTete));
        }
  }
  close(fd1_J);
}

//fonction mère
void joue()
{
    int flags=0;
    initialise_carteJ();
    //receivoir_les_cartes_deja_jouees_H();
    //affichePlateauJ();
    //affiche_cartes_joueur();

    while (1) {
      Tour_a_Jouer();

      if (indice%10==0) {
        //printf("JE suis dedans\n" );
        nouvelle_manche();
      }
      if (flags) {
      liberer_tableau_carte_jouees_H();
        break;
      }
    }
    /*if (receive == 0 || strcmp(buff_out, "exit") == 0
			leave_flag = 1;
		}*/
}

int main()
{
mkfifo(myfifo_J, 0666);
printf("Veuillez entrer votre nom: \n");
scanf("%s",cartesJoueur.nomJoueur);


  joue();

    return 0;
}
