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
#include <math.h>

#define P4_COLONNES (6)
#define P4_LIGNES (4)



int valide( int indiceActuel, int nombre);
static void remplissagePateau();
static void initialise_plateau();//affiche le plateau du jeu
static void envoieCarte(char* fichier,char nom[30]);//permet d'envoyer les dix cartes aux joueurs
void remplir_tableau_carte_jouees();//permet de remplir le tableau d'affichage
void liberer_tableau_carte_jouees();//libère la memoire
void recevoir_une_carte(char* fichier);//recupère la carte jouée
void compare_cartes(char nomJ[30]);//compare la carte que le joueur a envoyé et la place à la bonne position
int* derniere_carte_de_la_ligne();//permet de retourner un tableau des indices des dernière carte d'une ligne
void envoie_les_cartes_deja_jouees();//permet d'envoyer le plateau  à jour
void fin_dune_manche();//annonce la fin d'une manche et envoie 1O cartes à chacun des joueurs
int mettre_zero(int lgn,int col);//permet de mettre zero pour chaque case d'une ligne et retourne la somme des valeurs de têtes de boeufs
int souhait_de_ligne(int lgn,int col,char f[30]);//permetde demander au joueur quelle ligne il souhaite recuperée et renvoie la somme des têtes
void commencer_le_jeu() ;//per de lancer le jeu au complet
struct carte
{
  int* valeurN;
  int* nbreTete;
};
//les cartes complet du jeu
static struct plateau
{
  struct carte tableau [104];
}p;
int fd;//le descripteur
// FIFO file path
char * myfifo = "/tmp/myfifo";//fichier de communication avec le joueur humain
char * myfifo_R = "/tmp/myfifoR";//fichier de communication avec le joueur Robot
static int tete =4;
char bufferjoueur1[30];
char nomJoueur_h[30];//nom du joueur humain
char nomJoueur_R[30];//nom du joueur Robot
int scoreJoueur_h;//score joueur humain
int scoreJoueur_R;//score joueur Robot
static int flags =0;//met fin au jeu dès que le score de un des joueurs atteint 66
struct carte** tableau_carte_jouees ;
struct carte carte_recuperee;//permet
static int  manche=0;//permet de compter le nombre de manche (tour)
int indice=0;//permet de savoir si les joueurs ont déjà joués leur dix cartes pour commencer une nouvelle manche



int valide( int indiceActuel, int nombre)
{
    int i = 0;


    /* Maintenant, on verifie que le nombre n'a pas deja ete entre */
    for(i = 0; i < indiceActuel; i++)
        if(*p.tableau[i].valeurN == nombre)
            return 0;
    /* Tout va bien on retourne 1 */
    return 1;
}
static void remplissagePateau()
{int x, ok;
  srand((unsigned int)time(NULL));
  for (int i = 0; i < 104; i++) {
      p.tableau[i].valeurN =malloc(sizeof(int));
      p.tableau[i].nbreTete =malloc(sizeof(int));
      *p.tableau[i].nbreTete = ((int) rand()%7 +1);

        ok=1;
        do
        {

            x=((int) rand()%104 +1);
            /*for (int j = 0; j < i; j++) {
              if (*p.tableau[i].valeurN==x) {
                ok=0;
              }
            }*/
        }while(!valide(i,x));
        *p.tableau[i].valeurN=x;
        //printf("%d\n",*p.tableau[i].valeurN );
    }
  /*  for (int i = 0; i < 104; i++) {
      // les tetes qui on le chiffre 5 à la fin
      if( (*p.tableau[i].valeurN==5 || *p.tableau[i].valeurN%10==5) && *p.tableau[i].valeurN != 55)
      {
        *p.tableau[i].nbreTete = 2;
      }
       //Les tetes qui on le chiffre 0 à la fin
      if( (*p.tableau[i].valeurN % 10) == 0  )
      {
        *p.tableau[i].nbreTete = 3;
      }

     // Les tetes qui on le chiffre 55 à la fin
      if(*p.tableau[i].valeurN == 55){
        *p.tableau[i].nbreTete = 7;
      }
    // Les tetes qui on des doubles de nombre 11, 22,33,44, et...
       if(*p.tableau[i].valeurN != 55)
       {
         int lastdigit = *p.tableau[i].valeurN%10;
         while(*p.tableau[i].valeurN >= 10)
            {

            int  firsdigit =(int) floor(*p.tableau[i].valeurN/11);
              if(lastdigit == firsdigit)
               {
                *p.tableau[i].nbreTete = 5;
                }
            }
        }else{
          *p.tableau[i].nbreTete = 1;
        }
    }*/
    remplir_tableau_carte_jouees();
    carte_recuperee.valeurN =  malloc(sizeof(int));
    carte_recuperee.nbreTete =  malloc( sizeof(int));
}
void remplir_tableau_carte_jouees()
{
  tableau_carte_jouees = (struct carte**) malloc( 4* sizeof(struct carte*));
  for (int h = 0; h < 6; h++) {
    tableau_carte_jouees[h] = (struct carte*) malloc( 6* sizeof(struct carte));

  }
  for (int i = 0; i < 4; i++) {
      for (int k = 0; k < 6; k++) {
        tableau_carte_jouees[i][k].valeurN = (int*) calloc(1,sizeof(int));
        tableau_carte_jouees[i][k].nbreTete = (int*) calloc(1, sizeof(int));
      }
  }

 for (int j = 0; j < 4; j++) {

      tableau_carte_jouees[j][0].valeurN = p.tableau[j].valeurN;
      tableau_carte_jouees[j][0].nbreTete = p.tableau[j].nbreTete;
  }

}
void liberer_tableau_carte_jouees(/* arguments */) {
  for (int h = 0; h < 6; h++) {
    for (int i = 0; i < 6; i++) {
      free(tableau_carte_jouees[h]->valeurN );
      free(tableau_carte_jouees[h]->nbreTete );
    }
    free(tableau_carte_jouees[h] );
  }
  free(tableau_carte_jouees);
}
void recevoir_une_carte(char * fichier) {
  fd = open(fichier,O_RDONLY);
  if (fd<0) {
    perror("Erreur d'ouverture open");
    exit(0);
  }

  int res=read(fd,carte_recuperee.valeurN,sizeof(carte_recuperee.valeurN));
  if (res <0) {
    perror("Erreur:recption read");
    exit(0);
  }
  int res1=read(fd,carte_recuperee.nbreTete,sizeof(carte_recuperee.nbreTete));
  if (res1 <0) {
    perror("Erreur:reception read");
    exit(0);
  }

    indice++;
    //printf("%d\n",indice );
  close(fd);
}
 int mettre_zero(int lgn,int col)
 {
   int compt=0;
   for (int  i = 0; i <= col; i++) {
     *tableau_carte_jouees[lgn][i].valeurN =0;
     compt+=*tableau_carte_jouees[lgn][i].nbreTete;
     *tableau_carte_jouees[lgn][i].valeurN =0;
   }
   return compt;
 }
 int souhait_de_ligne(int i,int j,char f[30]) {
   int compt =0;

   if (strcmp(f,"Robot")) {
     compt = mettre_zero(i,j);
   }else{
     char choix[sizeof("Donner l'indice de ligne et de colonne de votre choix")];
     strcpy(choix,"Donner l'indice de ligne et de colonne de votre choix");
     fd = open(myfifo,O_WRONLY);
          if(write(fd,choix,sizeof(choix))<0)
          {
            perror("Erreur:write");
          }
    close(fd);
    int a=0;int b=0;
    fd = open(myfifo,O_RDONLY);
         if(read(fd,&a,sizeof(a))<0)
         {
           perror("Erreur:read choix");
         }
         if(read(fd,&b,sizeof(b))<0)
         {
           perror("Erreur:read choix");
         }
   close(fd);

     compt = mettre_zero(a,b);
   }
   return compt;
 }
int* derniere_carte_de_la_ligne()
{
    static int tab[4]={0};
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 5; j++) {
        if (*tableau_carte_jouees[i][j].valeurN !=0 && *tableau_carte_jouees[i][j+1].valeurN ==0)
            {  tab[i]=j;
              break;}
        }
        //printf("%d\n",tab[i] );

    }
    return tab;
}

void compare_cartes(char nomJ[30]) {

  int *tab;
  tab=derniere_carte_de_la_ligne();
  int res =*(tableau_carte_jouees[0][tab[0]].valeurN);int k=0;int h=0;int score =0;
  for (int i = 1; i < 4; i++)
  {
      if (*tableau_carte_jouees[i][tab[i]].valeurN !=0)
      {
        if(abs(*carte_recuperee.valeurN - res) > abs(*carte_recuperee.valeurN - *(tableau_carte_jouees[i][tab[i]].valeurN)))
            {
              res = *(tableau_carte_jouees[i][tab[i]].valeurN);
                k=i;h=tab[i];
            }
      }
  }

  //si c'est la sixième carte
  if ((h+1)==6) {
    //mettre zero
    score = mettre_zero(k,h);
    *tableau_carte_jouees[k][0].valeurN =*carte_recuperee.valeurN;
    *tableau_carte_jouees[k][0].nbreTete=*carte_recuperee.nbreTete;
  }else{
    //si la carte jouée à une valeur plus petite que la valeur des cartes presentes
    /*if (res > *carte_recuperee.valeurN) {//cette partie ne fonctionne pour celà on là commentée
      score = souhait_de_ligne(k,h,nomJ);
      *tableau_carte_jouees[k][0].valeurN =*carte_recuperee.valeurN;
      *tableau_carte_jouees[k][0].nbreTete=*carte_recuperee.nbreTete;
    }else{*/
      *tableau_carte_jouees[k][h+1].valeurN =*carte_recuperee.valeurN;
      *tableau_carte_jouees[k][h+1].nbreTete=*carte_recuperee.nbreTete;
      score = *carte_recuperee.nbreTete;
    //}

  }


  if (strcmp(nomJ,"Robot")) {
    scoreJoueur_R += score;
  }else{
    scoreJoueur_h += score;
  }

  if (scoreJoueur_h >=66 || scoreJoueur_R >=66) {
    printf("%s %d\n",nomJoueur_h,scoreJoueur_h );
    printf("%s %d\n",nomJoueur_R,scoreJoueur_R );
    flags=1;
  }

}

static void initialise_plateau(/* arguments */) {
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

         if (*tableau_carte_jouees[lgn][col].valeurN !=0) {

            printf(" %d  %d |",*(tableau_carte_jouees[lgn][col].valeurN),*(tableau_carte_jouees[lgn][col].nbreTete));
          }

        putchar('\n');
        putchar('+');

        for (col = 1; col <= P4_COLONNES; ++col)
            printf("-------+");

        putchar('\n');
    }

}

//les cartes à envoyer à un nomJoueur
static void  envoieCarte(char* fichier,char nom[30])
{
    scoreJoueur_h=0;scoreJoueur_R=0;
    fd = open(fichier,O_RDONLY);
    if (fd<0) {
      perror("Erreur d'ouverture open");
      exit(0);
    }
    read(fd,bufferjoueur1,sizeof(bufferjoueur1));
    printf("%s\n",bufferjoueur1 );
    strcpy(nom, bufferjoueur1);
    close(fd);
    fd = open(fichier,O_WRONLY);
    for (int i = tete; i < (tete+10); i++) {
      write(fd,p.tableau[i].valeurN,sizeof(p.tableau[i].valeurN));
      //sleep();
      write(fd,p.tableau[i].nbreTete,sizeof(p.tableau[i].nbreTete));
    //  sleep(5);
    }
    close(fd);
    tete +=10;

}
void envoie_les_cartes_deja_jouees(/* arguments */) {
  fd = open(myfifo,O_WRONLY);
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 6; j++)
     {
       if(write(fd,tableau_carte_jouees[i][j].valeurN,sizeof(tableau_carte_jouees[i][j].valeurN))<0)
       {
         perror("Erreur:write");
       }
       //sleep();
       if(write(fd,tableau_carte_jouees[i][j].nbreTete,sizeof(tableau_carte_jouees[i][j].nbreTete))<0)
       {
         perror("Erreur:write");
       }
    }
  }
  close(fd);
}
void fin_dune_manche(/* arguments */) {
  printf("Fin du manche: %d\n",manche );
  //envoieCarte(myfifo,nomJoueur_h);
  //envoieCarte(myfifo_R,nomJoueur_R);
  printf("Joueur :%s score :%d\n",nomJoueur_h,scoreJoueur_h );
  printf("Joueur :%s score :%d\n",nomJoueur_R,scoreJoueur_R );
  fd = open(myfifo,O_WRONLY);
  for (int i = tete; i < (tete+10); i++) {
    write(fd,p.tableau[i].valeurN,sizeof(p.tableau[i].valeurN));
    //sleep();
    write(fd,p.tableau[i].nbreTete,sizeof(p.tableau[i].nbreTete));
  //  sleep(5);
  }
  close(fd);
  tete +=10;
  fd = open(myfifo_R,O_WRONLY);
  for (int i = tete; i < (tete+10); i++) {
    write(fd,p.tableau[i].valeurN,sizeof(p.tableau[i].valeurN));
    //sleep();
    write(fd,p.tableau[i].nbreTete,sizeof(p.tableau[i].nbreTete));
  //  sleep(5);
  }
  close(fd);
  tete +=10;
  manche++;
}

void commencer_le_jeu(/* arguments */) {

  remplissagePateau();
  initialise_plateau();
  envoieCarte(myfifo,nomJoueur_h);
  envoieCarte(myfifo_R,nomJoueur_R);

  //envoie_les_cartes_deja_jouees();

  while (1) {
    recevoir_une_carte(myfifo);
    compare_cartes(nomJoueur_h);
    initialise_plateau();
    recevoir_une_carte(myfifo_R);
    compare_cartes(nomJoueur_R);
    initialise_plateau();
    if(indice%20==0)
    {
      fin_dune_manche();
      //printf("JE suis dedans\n" );
    }
    //envoie_les_cartes_deja_jouees();
    if (flags) {
      liberer_tableau_carte_jouees();
      break;
    }
  }

}
ls -l|grep -v '^l'|grep -v '^d'|wc -l
int main(int argc, char const *argv[]) {
  mkfifo(myfifo, 0666);
  mkfifo(myfifo_R, 0666);
  commencer_le_jeu();
  return 0;
}
