# Objectif 

Le but est de paralléliser le produit matrice-matrice avec les instructions SIMD, des intervalles 2D et
paralléliser avec OpenMP.

Le produit matriciel C=AB consiste à faire la somme du produit élément par élément d’une ligne de A
avec une colonne de B. Donc, chaque élément de C est calculé de la manière suivante :

# Détails du projet

Le projet est compilé avec CMake.

Deux exécutables sont compilés, soit « test_gemm » et « gemm_benchmark », pour tester et mesurer la
performance, respectivement.

Le test sans argument exécute tous les tests. Pour exécuter un test en particulier, on peut le spécifier en
ligne de commande. Le nom est composé de la fonction et de la taille de matrice. Voici un exemple
pour exécuter seulement le test SIMD avec une taille de matrice 8 :

```
./test_gemm simd_
```

Les fichiers logs contiennent le résultat de chaque matrice et le résultat
de référence. Chaque test produit un fichier de log à son nom.

Le fichier « gemm.cpp » contient les optimisations. Voici les descriptions :

- gemm_basic : algorithme de référence, n’est pas optimisé pour la rapidité, mais produit le bon
    résultat. Utile pour vérifier les autres implémentations.
- gemm_simd : utilisation des instructions SIMD (Intel SSE)
- gemm_blocked : décomposition en bloc 2D pour optimiser l’utilisation de la cache
- gemm_omp : parallélisation avec OpenMP
- gemm_combo : combinaison des techniques SIMD, bloc 2D et OpenMP

 Quelques indications sur la structure de données Matrix :

- On suppose uniquement des matrices carrées (hauteur == largeur).
- La matrice contient des nombres simple précision de type « float »
- La taille des matrices est toujours un multiple de 4 nombres (correspond à un registre SIMD de
    128-bit).
- Les éléments d’une colonne sont contigus en mémoire. Il s’agit de la convention « Column-
    major ». Voir figure 1. Donc, il est plus efficace de parcourir les éléments par colonnes plutôt
    que par ligne. Aussi, le chargement d’un registre SIMD depuis une adresse fera en sorte de
    charger les éléments de cette colonne.
- Le tableau de la matrice est aligné sur 64 octets, ce qui est plus que suffisant pour utiliser les
    instructions vectorielles alignées sur 128-bits.




