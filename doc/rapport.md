---
title: "UPEM / Compilation / Projet / Rapport"
author: [Pacien TRAN-GIRARD, Adam NAILI]
date: 2018-02-20
...

# Usage
Après un `make`, `./tcompil < prog.tpc [-o prog.asm]`
	
Pour faciliter les tests, utiliser `make test FILE_TEST=test_file`
test_file est en réalité un fichier d'extension .tpc placé dans le répertoire res. (exemple: res/test_file.tpc).
Ceci génère un executable `test_file` dans le répertoire out.

# Fonctions fournies par le langage

## Fonctions de lecture

- `reade(entier e)`

	Fonction de lecture sur l'entrée standard de valeur de type `entier`. On passe en paramètre un `entier` a l'adresse duquel sera stocké l'entier lu.

- `readc(caractere c)`

	Fonction de lecture sur l'entrée standard de valeur de type `caractere`. On passe en paramètre un `caractere` a l'adresse duquel sera stocké le caractere lu.

## Fonction d'écriture
- `print(<entier|caractere> exp)`

	Fonction d'écriture sur la sortie standard qui se charge d'identifier le type de l'expression passée en paramètre.

# Analyse lexicale

L'analyse lexicale est réalisée avec Flex et est contenue dans le fichier `tpc.lex`.

## Lexèmes

Les lexèmes ont été définis au sein de ce fichier :

|             Symbole            |                   Description                  | Token associé |
|:------------------------------:|:----------------------------------------------:|:-------------:|
|               `=`              |             opérateur d'affectation            |               |
|               `+`              |              opérateur d'addition              |    `ADDSUB`   |
|               `-`              |    opérateur de soustraction ou moins unaire   |    `ADDSUB`   |
|               `*`              |                 multiplication                 |   `DIVSTAR`   |
|           `/` et `%`           |    division et reste de la division entière    |   `DIVSTAR`   |
|               `!`              |               négation booléenne               |               |
|           `==`, `!=`           |            opérateur de comparaison            |      `EQ`     |
|      `<`, `>`, `<=` ,`>=`      |            opérateur de comparaison            |    `ORDER`    |
|              `&&`              |                opérateur booléen               |     `AND`     |
|              `||`              |                opérateur booléen               |      `OR`     |
|           `;` et `,`           |         le point virgule et la virgule         |               |
| `(`, `)`, `{`, `}`, `[` et `]` | les parenthèses, les accolades et les crochets |               |

- Certains symboles ne sont pas associés à des tokens.
  En effet ils sont utilisés directement dans la grammaire, entourés de simple guillemets. (par exemple `'='`)
  
- Les identificateurs sont constitués d'au moins une lettre, puis éventuellement de chiffres, et/ou d'"underscore" (`_`).
  Ils sont identifiés par le token `IDENT`

- Gestion de la casse

- Les types sont `entier` et `caractere` et renvoient le token `TYPE`

- Les mots-clés comme `if else return` (auquels on associe les tokens suivants `IF ELSE RETURN`) doivent être écrit en majuscules et ne peuvent pas être utilisés comme identificateur. 

- Les commentaires sont délimités par `/*` et `*/` et ne peuvent pas être imbriqués.

- `print`,`reade` et `readc` sont des identificateurs qui appartiennent au langage tpc, ils ne doivent pas être utilisé pour déclarer des variables.
  Nous avons donc créer des tokens spécifiques pour leur gestion dans la grammaire (respectivement les tokens `PRINT`, `READE` et`READC`)


# Analyse syntaxique

La grammaire est celle fournie sur la plateforme elearning.

## Résolution des conflits

Cette grammaire génère un conflit empiler/réduire pour la règle

```yacc
Instr	: IF '('Exp')' Instr
		| IF '('Exp')' Instr ELSE Instr
```

Par exemple :

```
IF '('Exp')' IF '('Exp')' Instr ELSE Instr
```

Faut il empiler `ELSE` ? Ou réduire `IF '('Exp')' Instr` ?
Pour résoudre le conflit, il faut gérer les `precedences`.
Nous avons choisi de rentre le `ELSE` plus précédant que `')'` qui est le dernier terminal.
Ainsi, Bison va choisir d'empiler en priorité.

Un autre conflit existe, sur l'associativité de `,`:

```yacc
ListExp	: ListExp ',' Exp
		| Exp ';'
```

Par exemple :

```yacc
ListExp ',' ListExp ',' ListExp
```

Doit-on reduire à gauche ? Ou a droite ?
Dans les langages tel que le C, l'associativité se fait à gauche.
On déclare donc `%left ,` pour l'indiquer à Bison.


# Analyse sémantique

Le compilateur gère l'analyse sémantique.
Par exemple l'opération `tab[1] * 3` est légale mais `'a' * 'b'` ne l'est pas. La grammaire l'accepte, mais l'analyse sémantique à parti des attributs interdits.
De la même manière des caractères ne peuvent pas être des booléens.


# Génération de code cible

## Gestion de la mémoire

Pour des raisons de simplicité, il a été choisi de travailler principalement sur la pile. Son utilisation simplifie le changement de contexte à l'entrée et à la sortie de fonctions, en plus de permettre la réduction aisée des expressions lors de l'exécution.

## Fonctions

### Fonction principale

Le programme doit être doté d'une fonction `void main(void)` qui sera le point d'entrée du programme.
La forme alternative `entier main(void)` est aussi acceptée par convention. La valeur retournée est ignorée et l'exécutable renverra toujours `0`.


### Passage de paramètres

Il a été choisi d'effectuer le passage des paramamètres simples (de types `entier` et `caractere`) par copie pour des raisons de simplicité et de sécurité, pour permettre une certaine isolation des contextes lors de l'entrée dans une fonction.

Tous les paramètres sont implicitement convertis en type `entier`, la vérification du typage des paramètres s'avérant difficile. De plus, il n'est pas possible de passer un tableau en paramètre par référence.


### Retour de valeur

Les fonction valuées retournent leur valeur en suivant les conventions d'appel des fonctions C, c'est-à-dire en la plaçant dans le registre `rax`. L'appelant récupère ensuite cette valeur pour pouvoir la stocker dans la pile.

L'analyse statique des branchements étant un problème relativement complexe, il a été choisi de retourner une valeur par défaut (`-1`) en cas d'absence d'instruction de retour dans une fonction valuée.


# Améliorations enviseageables

## Récupération sur erreur

Nous pourrions envisager de parser entièrement le fichier et de ne pas s'arrêter dès la première erreur de syntaxe comme le fait `gcc`.
Il est possible de réaliser ceci en modifiant le code de `yyerror()` par exemple.

## Suppression du code partiellement généré

Une erreur de compilation entraîne l'arrêt de la génération de code, pouvant laisser un fichier de sortie partiellement généré. Il serait préférable de le supprimer en cas d'erreur.
Cette gestion s'appuierait sur `yyerror()` et le retour de la fonction `yyparse()`. Lorsque le code de retour du compilateur est identifiable à une erreur, on supprimerait le .asm généré afin de ne pas fournir à l'utilisateur un programme assembleur frelaté. 
