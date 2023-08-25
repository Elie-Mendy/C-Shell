/*************************************************************
# Nom ......... : menshell.c
# Auteur .......: Elie MENDY
# Licence ..... : réalisé dans le cadre du cours de SE (21/22)
# Compilation.. : gcc -Wall menshell.c -L/usr/include -lreadline -o menshell
# Usage........ : pour exécuter : menshell
#*************************************************************/

#include "sys.h"

//*************************************************************
//* DEFINITION DES PRIMITIVES
//*************************************************************

//* ----- CONSTANTES ------

enum
{
  MaxLigne = 1024,       // longueur max d'une ligne de commandes
  MaxMot = MaxLigne / 2, // nb max de mot dans la ligne
  MaxCommandes = 100,    // nb max de commandes par ligne
  MaxDirs = 100,         // nb max de repertoire dans PATH
  MaxPathLength = 512,   // longueur max d'un nom de fichier
};

//* ------ STRUCTURE COMMANDE ------

typedef struct command
{
  char *argv[MaxLigne];
} * command;

//* ----- VARIABLES GLOBALES -----

// [ligne] acceuillera la ligne de commande lue après le prompte
char ligne[MaxLigne];

// [copyLigne] acceuillera copie de la ligne de commande
char copyLigne[MaxLigne];

// [mot] contiendra un découpage de la ligne par mots
// sera utilisée pour le prétraitement et le lancement des commandes perso
char *mot[MaxMot];

// [commande] vecteur de chaines utilisé pour l'execution d'une commande
// est remplie par le parsing() des chevrons sur une ldc
char *commande[MaxMot];

// [lstCommandes] vecteur de chaines qui contiendra une
// liste de commandes (sous forme de chaines)
// sera remplie par le découpage() de la copie de la ldc par pipes "|"
char *lstCommandes[MaxCommandes];

// [lstCommandesDecoupees] vecteur qui contiendra une liste
// de commandes (sous forme de vecteur de chaines)
// sera remplie par le découpage() de chaque commande présentes dans le vecteur lstCommandes[]
// le decoupage de chaque commande se fera selon le separateur donné dnas le cour " \t\n"
char *lstCommandesDecoupees[MaxCommandes][MaxLigne];

// entiers utilisés commes indices pour itérer sur les différentes structures
int nbMot, nbCommandes, i, tmp, process;

// stockage des noms des fichiers de redirections
char
    input[128] = {0},  // redirection de l'entrée standard
    output[128] = {0}, // redirection de la sortie standard
    error[128] = {0};  // redirection des erreurs

//* ----- PROTOTYPES DES FONCTIONS -----

// logger de message pour l'utilisateur
void usage(char *);

// commande perso - changement de directory
int moncd(int, char *[]);

// decoupage d'une ligne de commande - remplissage d'un vecteur de mots
int decouper(char *, char *, char *[], int background);

// identification des chevrons sur une ligne de commande
void parsingChevrons(char *[], int);

// lancer l'arborescence de processus enfant(s)
int forkPipes(int n, struct command *cmd, int background);

// redirections en cas de pipe(s) + execution commande
int spawnProc(int in, int out, struct command *cmd);

// lancer une saisie utilisateur
int saisirLDC(char *str);

// lancer le vidage du buffer
void viderBuffer();


//*************************************************************
//* EXECUTION DU MAIN
//*************************************************************

int main(int argc, char *argv[])
{

  //* ----- BOUCLE D'EVALUATION -----

  /* lecture et traitement de chaque ligne de commande  */
  // for (printf(PROMPT); fgets(ligne, sizeof ligne, stdin) != 0; printf(PROMPT))
  for (; saisirLDC(ligne);)
  {
    //* ----- GESTION LANCEMENT BACKGROUND -----
    // toggle pour le lancement d'une commande en background
    int background = 0;
    if (ligne[strlen(ligne) - 1] == '&')
    {
      // modification du toggle
      background = 1;
      // supression du caractere '&' à la fin de la LDC
      ligne[strlen(ligne) - 2] = '\0';
    }

    //* ----- COPIE + DECOUPAGE (de la ldc) -----
    // copie de la ligne de commande (necessaire pour la redécouper par la suite)
    strcpy(copyLigne, ligne);

    // découpage de la ligne + récupération du nombre de mot
    nbMot = decouper(ligne, " \t\n", mot, MaxMot);

    //* ----- PRE-TRAITEMENT -----
    // si ligne vide
    if (mot[0] == 0)
    {
      continue;
    }

    // [commande perso] moncd()
    if (strcmp(mot[0], "cd") == 0)
    {
      moncd(nbMot, mot);
      continue;
    }

    // [commande perso] exit()
    if (strcmp(mot[0], "exit") == 0)
    {
      exit(1); // on sort
    }

    //* ----- LANCEMENT PROCESSUS ENFANT -----
    tmp = fork();

    //! ERREUR FORK 
    if (tmp < 0) // == -1 : Erreur
    {
      perror("fork");
      continue; // relance du prompt
    }

    //! PROCESSUS PARENT
    else if (tmp != 0) // tmp == PID de l'enfant
    {
      // attente du la fin du processus
      // uniquement si lancement au premier plan
      if (!background)
        waitpid(tmp, NULL, 0);
      continue; // relance du prompt
    }

    //! PROCESSUS ENFANT
    nbCommandes = decouper(copyLigne, "|", lstCommandes, MaxCommandes);

    //* ----- INITIALISATION D'UNE STRUCTURE COMMANDE -----
    struct command cmd[MaxCommandes];

    //* ----- REMPLISSAGE DE LA STRUCTURE COMMANDE -----
    for (int c = 0; c < nbCommandes; c++)
    {
      decouper(lstCommandes[c], " \t\n", lstCommandesDecoupees[c], MaxCommandes);

      for (int i = 0; lstCommandesDecoupees[c][i]; i++)
      {
        cmd[c].argv[i] = lstCommandesDecoupees[c][i];
      }
    }

    //* ----- EXECUTION DES COMMANDDES DE LA LDC -----
    //? mecanisme d'execution -->  voir fonctions fork_pipes() et spawn_proc()
    forkPipes(nbCommandes, cmd, background);

    //* ----- COMMANDE NON TROUVEE ----
    fprintf(stderr, "%s: not found\n", commande[0]);
    exit(1);
  }

  //* ----- SORTIE DU SHELL -----
  printf("Bye\n");
  return 0;
}

//*************************************************************
//* DEFINITION DE FONCTIONS
//*************************************************************


/*
Nom 	    : usage
Objectif    : logger de message d'erreurs pour l'utilisateur
*/
int saisirLDC(char *str)
{
  char *buffer;
  // affichage du prompt
  // methode permettant l'autocompletion des noms de fichiers
  buffer = readline("\n>>> ");
  add_history(buffer);
  strcpy(str, buffer);

  return 1;
}

/*
Nom 	    : viderBuffer
*/
void viderBuffer()
{
  while (getchar() != '\n')
    ;
}

/*
Nom 	    : usage
Objectif    : logger de message d'erreurs pour l'utilisateur
*/
void usage(char *message)
{
  fprintf(stderr, "%s\n", message);
  exit(1);
}

/*
Nom 	    : decouper()
Objectif    : decoupe une chaine en mots
*/
int decouper(char *ligne, char *separ, char *mot[], int maxmot)
{
  int i; // compteur de mots

  // decoupe de la ligne + remplissage du vecteur 'mot'
  mot[0] = strtok(ligne, separ);
  for (i = 1; mot[i - 1] != 0; i++)
  {
    if (i == maxmot)
    {
      fprintf(stderr, "Err. decouper() : trop de mots\n");
      mot[i - 1] = 0;
      break;
    }
    mot[i] = strtok(NULL, separ);
  }

  // retour du nombre de mots parsés
  return i - 1;
}

/*
Nom 	    : moncd()
Objectif    : changement de répertoire
*/
int moncd(int ac, char *av[])
{
  char *dir;
  int t;

  //* ----- TRAITEMENT DES ARGS -----

  // si aucun arg donné à ma cmd : on retourne dans le home
  if (ac < 2)
  {
    dir = getenv("HOME"); // récupèration de la valeur de $home
    if (dir == 0)
    {
      dir = "/tmp";
    }
  }

  // si
  else if (ac > 2)
  {
    fprintf(stderr, "usage: %s [dir]\n", av[0]);
    return 1;
    // sinon, on va dans le repertoire donné
  }
  else
  {
    dir = av[1];
  }

  /* FAIRE LE BOULOT */
  t = chdir(dir);
  if (t < 0)
  { // test de l'appel systeme chdir
    perror(dir);
  }
  return 0;
}

/*
Nom 	    : parsingChevrons()
Objectif    :   identification des chevrons sur une ligne de commande
                effectuer les redirections impliqués par les chevrons
*/
void parsingChevrons(char *ligne[], int background)
{

  //* -----  DECLARATION DES TOOGLES (chevrons) -----

  // toggle d'indication de présence
  // de chevron(s) sur une ligne de commande
  int chevron = 0;

  // définition de toggles (un pour chaque chevron)
  int
    in = 0,      //      <
    out = 0,     //      >
    outEnd = 0,  //      >>
    err = 0,     //      2>
    errEnd = 0,  //      2>>
    andOne = 0,  //      2&>1
    andTwo = 0,  //      1&>1
    and = 0,     //      &>
    andEnd = 0;  //      &>>

  //* -----  PARSING DE LA LDC -----

  // pour chaque chevron identifié sur la ldc :
  // - activation du toggle correspondant
  // - sauvegarde de l'emplacement de la redirection (input/output/error)
  for (int i = 0; ligne[i] != NULL; i++)
  {
    if (!strcmp(ligne[i], "<"))
    {
      chevron = 1;
      strcpy(input, ligne[i + 1]);
      in = 1;
    }

    if (!strcmp(ligne[i], ">"))
    {
      chevron = 1;
      strcpy(output, ligne[i + 1]);
      out = 1;
    }

    if (strcmp(ligne[i], "2>") == 0)
    {
      chevron = 1;
      strcpy(error, ligne[i + 1]);
      err = 1;
    }

    if (strcmp(ligne[i], ">>") == 0)
    {
      chevron = 1;
      strcpy(output, ligne[i + 1]);
      outEnd = 1;
    }

    if (strcmp(ligne[i], "2>>") == 0)
    {
      chevron = 1;
      strcpy(error, ligne[i + 1]);
      errEnd = 1;
    }

    if (strcmp(ligne[i], "1>&2") == 0)
    {
      chevron = 1;
      andOne = 1;
    }

    if (strcmp(ligne[i], "2>&1") == 0)
    {
      chevron = 1;
      andTwo = 1;
    }

    if (strcmp(ligne[i], "&>") == 0)
    {
      chevron = 1;
      strcpy(output, ligne[i + 1]);
      and = 1;
    }

    if (strcmp(ligne[i], "&>>") == 0)
    {
      chevron = 1;
      strcpy(output, ligne[i + 1]);
      andEnd = 1;
    }

    // construction du vecteur ldc contenant tous les mots qui précèdent le 1er chevron
    if (!chevron)
    {
      commande[i] = strdup(ligne[i]);
    }
  }

  //* -----  MISE EN PLACE DE OU DES REDIRECTION(S) -----
  //? & lancement en arrière plan
  if (background)
  {
    // redirection vers '/dev/null' 
    int fd;
    if ((fd = open("/dev/null", O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0)
    {
      perror("Stdout : impossible d'ouvrir le fichier\n");
      exit(1);
    }

    if (dup2(fd, STDOUT_FILENO) < 0 || dup2(fd, STDERR_FILENO) < 0)
    {
      perror("Fils : erreur lors de la duplication du descripteur ");
      exit(1);
    }

    assert(close(fd) >= 0);
  }

  //? < - redirection stdin
  if (in)
  {
    int fd0;
    if ((fd0 = open(input, O_RDONLY, 0)) < 0)
    {
      perror("Stdin : impossible de lire le fichier\n");
      exit(0);
    }
    if (dup2(fd0, STDIN_FILENO) < 0)
    {
      perror("Fils : erreur lors de la duplication du descripteur ");
      exit(1);
    }
    assert(close(fd0) >= 0);
  }

  //? > - redirection stdout
  if (out)
  {

    int fd1;
    if ((fd1 = open(output, O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0)
    {
      perror("Stdout : impossible d'ouvrir le fichier\n");
      exit(1);
    }

    if (dup2(fd1, STDOUT_FILENO) < 0)
    {
      perror("Fils : erreur lors de la duplication du descripteur ");
      exit(1);
    }

    /* 2>&1 - redirection de stderr en association avec stdout */
    if (andTwo)
    {
      if (dup2(fd1, STDERR_FILENO) < 0)
      {
        perror("Fils : erreur lors de la duplication du descripteur ");
        exit(1);
      }
    }

    assert(close(fd1) >= 0);
  }

  //? >> - redirection stdout en fin de fichier
  if (outEnd)
  {
    int fd;
    if ((fd = open(output, O_WRONLY | O_CREAT | O_APPEND, 0666)) < 0)
    {
      perror("Stdout : impossible d'ouvrir le fichier\n");
      exit(0);
    }
    if (dup2(fd, STDOUT_FILENO) < 0)
    {
      perror("Fils : erreur lors de la duplication du descripteur ");
      exit(1);
    }

    /* 2>&1 - redirection de stderr en association avec stdout */
    if (andTwo)
    {
      if (dup2(fd, STDERR_FILENO) < 0)
      {
        perror("Fils : erreur lors de la duplication du descripteur ");
        exit(1);
      }
    }

    assert(close(fd) >= 0);
  }

  //? 2> - redirection stderr
  if (err)
  {

    int fd2;
    if ((fd2 = open(error, O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0)
    {
      perror("Stderr : impossible d'ouvrir le fichier\n");
      exit(0);
    }

    if (dup2(fd2, STDERR_FILENO) < 0)
    {
      perror("Fils : erreur lors de la duplication du descripteur ");
      exit(1);
    }

    /* 1>&2 - redirection de stdout en association avec stderr */
    if (andOne)
    {
      if (dup2(fd2, STDOUT_FILENO) < 0)
      {
        perror("Fils : erreur lors de la duplication du descripteur ");
        exit(1);
      }
    }

    assert(close(fd2) >= 0);
  }

  //? 2>> - redirection stderr en fin de fichier
  if (errEnd)
  {

    int fd2;
    if ((fd2 = open(error, O_WRONLY | O_CREAT | O_APPEND, 0666)) < 0)
    {
      perror("Stderr : impossible d'ouvrir le fichier\n");
      exit(0);
    }

    if (dup2(fd2, STDERR_FILENO) < 0)
    {
      perror("Fils : erreur lors de la duplication du descripteur ");
      exit(1);
    }

    /* 1>&2 - redirection de stdout en association avec stderr */
    if (andOne)
    {
      if (dup2(fd2, STDOUT_FILENO) < 0)
      {
        perror("Fils : erreur lors de la duplication du descripteur ");
        exit(1);
      }
    }

    assert(close(fd2) >= 0);
  }

  //? &> - redirection stdin et stdout au même endroit
  if (and)
  {
    int fd;
    if ((fd = open(output, O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0)
    {
      perror("Stdout : impossible d'ouvrir le fichier\n");
      exit(0);
    }
    if (dup2(fd, STDOUT_FILENO) < 0 || dup2(fd, STDERR_FILENO) < 0)
    {
      perror("Fils : erreur lors de la duplication du descripteur ");
      exit(1);
    }
    assert(close(fd) >= 0);
  }

  //? &>> - redirection stdin et stdout au même endroit en fin de fichier
  if (andEnd)
  {
    int fd;
    if ((fd = open(output, O_WRONLY | O_CREAT | O_APPEND, 0666)) < 0)
    {
      perror("Stdout : impossible d'ouvrir le fichier\n");
      exit(0);
    }
    if (dup2(fd, STDOUT_FILENO) < 0 || dup2(fd, STDERR_FILENO) < 0)
    {
      perror("Fils : erreur lors de la duplication du descripteur ");
      exit(1);
    }

    assert(close(fd) >= 0);
  }
}

/*
Nom 	    : spawnProc()
Objectif    : effectuer les redirections en cas de pipes
              execution de la commande
*/
int spawnProc(int in, int out, struct command *cmd)
{
  //! initialisation d'un pid
  pid_t pid;

  //! lancement d'un processus enfant
  if ((pid = fork()) == 0)

  //! execution dans processus enfant
  {

    //* ----- REDIRECTION ENTREE STANDARD -----

    if (in != 0)
    {
      dup2(in, 0);
      close(in);
    }

    //* ----- REDIRECTION SORTIE STANDARD -----

    if (out != 1)
    {
      dup2(out, 1);
      close(out);
    }

    //* ----- EXECUTION DE LA COMMANDE -----

    parsingChevrons(cmd->argv, 0);
    return execvp(commande[0], (char *const *)commande);
  }

  //! si parent: retourne le pid
  return pid;
}

/*
Nom 	    : forkPipes()
Objectif    : lancer l'arborescence de processus enfant
            correspondant aux commandes présentes sur la ldc (séparées par des pipes " | ")
*/
int forkPipes(int n, struct command *cmd, int background)
{
  int i, in, fd[2];

  //? Le premier processus doit obtenir son entrée à partir du descripteur de fichier original 0.
  in = 0;

  //? boucle d'execution de chaque commande sauf la dernière étape du pipeline.
  for (i = 0; i < n - 1; ++i)
  {
    pipe(fd);

    //? report de 'in' (de l'iteration précédente) dans fd[1] le file descriptor d'ecriture du pipe l'itération précédente
    spawnProc(in, fd[1], cmd + i);

    //? fermeture du file descriptor d'ecriture
    close(fd[1]);

    //? capture de la valeur de fd[0] --> le processus enfant lira à partir de ce file descriptor
    in = fd[0];
  }

  //? définition de stdin comme étant l'entrée de lecture du pipe précédent
  if (in != 0)
    dup2(in, 0);

  //* ----- EXECUTION DE LA COMMANDE -----
  //? si pipeline sur la ldc cette execution correspond
  //? a celle de la dernière commande apres le pipeline

  parsingChevrons(cmd[i].argv, background);
  return execvp(commande[0], (char *const *)commande);
}

