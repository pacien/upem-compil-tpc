# UPEM / Compilation / Projet

Compilateur TPC (Très Petit C).


## Compilation

* Compiler le compilateur : `make all`
* Générer le rapport PDF : `make out/rapport.pdf` (requiert `pandoc`)
* Exécuter un test : `make test FILE_TEST=some_test_ok && out/some_test_ok`


## Dépendances

* Analyseur lexical : flex
* Analyseur syntaxique : bison


## Licence

Projet distribué sous licence Creative Commons BY-NC-SA 4.0.
Tous droits réservés aux auteurs du projet.

