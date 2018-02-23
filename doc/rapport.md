---
title: "UPEM / Compilation / Projet"
author: [Pacien TRAN-GIRARD, Adam NAILI]
date: 2018-02-20
...

#Rapport 

##Analyse lexicale
L'analyse lexicale est réalisée avec Flex et est contenue dans le fichier `tpc.lex`.
*Les lexèmes ont été définis au sein de ce fichier:*

|      Symbole      |                   Description                   | Token associé |
|:-----------------:|:-----------------------------------------------:|:-------------:|
|         =         |             opérateur d'affectation             |               |
|         +         |               opérateur d'addition              |     ADDSUB    |
|         -         |    opérateur de soustraction ou moins unaire    |     ADDSUB    |
|         *         |                  multiplication                 |    DIVSTAR    |
|      / et %       |     division et reste de la division entière    |    DIVSTAR    |
|         !         |                négation booléenne               |               |
|       ==, !=      |             opérateur de comparaison            |       EQ      |
|     <,>,<=,>=     |             opérateur de comparaison            |     ORDER     |
|         &&        |                opérateur booléen                |      AND      |
|         ||        |                opérateur booléen                |       OR      |
|       ; et ,      |          le point virgule et la virgule         |               |
| (, ), {, } [ et ] |  les parenthèses, les accolades et les crochets |               |

- Certains symboles ne sont pas associés à des tokens. En effet ils sont utilisés directement dans la grammaire, entourés de simple guillemets. (par exemple `'='`)

- Les identificateurs sont constitués d'au moins une lettre, puis éventuellement de chiffres, et/ou d'"underscore" (`_`). Ils sont identifiés par le token `IDENT`

- Gestion de la casse

- Les types sont `entier` et `caractere` et renvoient le token `TYPE`

//TODO


`print`,`reade` et `readc` sont des identificateurs qui appartiennent au langage tpc, ils ne doivent pas être utilisé pour déclarer des variables. Nous avons donc créer des tokens spécifiques pour leur gestion dans la grammaire.

##Analyse syntaxique
//TODO
##Améliorations enviseageables

- Récupération sur erreur

- Numérotation des lignes 

//TODO