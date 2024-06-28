---
cover: .gitbook/assets/shell.webp
coverY: 0
---

# Introduction

#### Objectif

Le programme `menshell.c` est un shell minimaliste développé dans un but pédagogique pour illustrer la structure et le fonctionnement d'un shell. Ce projet a pour but de fournir une compréhension approfondie des concepts fondamentaux du développement en C, en particulier ceux liés à la manipulation des processus, à la gestion de la mémoire et aux appels système. En développant ce shell, il est possible de se familiariser avec des concepts avancés tels que le découpage de chaînes, la gestion des redirections, et la création de processus enfants.

#### Démarche

Le développement du shell `menshell` a suivi une démarche structurée pour assurer une compréhension progressive et approfondie des différents concepts. Voici les principales étapes de cette démarche :

1. **Définition des constantes et structures de données** :
   * Définition des constantes pour les tailles maximales des lignes de commande, des mots, des commandes, etc.
   * Création de la structure `command` pour stocker les arguments des commandes.
2. **Gestion des entrées utilisateur** :
   * Lecture des lignes de commande saisies par l'utilisateur.
   * Découpage des lignes en mots et en commandes.
3. **Implémentation des commandes internes** :
   * Développement des commandes internes comme `cd` et `exit`.
4. **Gestion des processus** :
   * Utilisation des appels système `fork`, `execvp` et `waitpid` pour créer et gérer des processus enfants.
   * Implémentation de la gestion des redirections (entrée, sortie, erreurs) et des pipes.
5. **Traitement des redirections et des pipes** :
   * Identification et traitement des chevrons pour les redirections (`<`, `>`, `>>`, `2>`, `2>>`, etc.).
   * Mise en place des pipes pour permettre l'exécution de commandes enchaînées.

#### Conclusion

Le projet `menshell` est une excellente opportunité pour illustrer et comprendre les concepts clés de la programmation système en C. Il offre une expérience pratique et une compréhension approfondie de la manière dont les shells et les systèmes d'exploitation fonctionnent au niveau du code. Ce projet met en évidence l'importance de la gestion des processus, des entrées/sorties et de la manipulation des chaînes de caractères dans le développement de logiciels système.

En réalisant ce projet, on acquiert des compétences précieuses qui seront utiles dans de nombreux domaines de l'informatique, y compris le développement de logiciels, l'administration système, et la recherche en informatique.
