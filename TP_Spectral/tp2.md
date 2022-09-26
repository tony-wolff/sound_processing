### Exercice 9
> Calculer les temps de calcul nécessaires à la DFT et la FFT lors de l’exécution du programme pour une trame du fichier toms.aiff. Les résultats sont-ils exploitables? Pourquoi?

Evaluer les résultat un a un ne donne pas une bonne valeur de la différence de calcul entre les deux algorithmes. Il vaut mieux mesurer les performances sur l'entièreté du traitement du fichier.


### Exercice 10
> Quelle est la complexité théorique des deux algorithmes testés ?
DTF : O(n²) : quadratique, pas bon pour avec des échantillons de 44100
FFT : O(nlog(n)) : très bonne performance même en temps réel.

### Exercice 11
avec dft sur toms.aiff : 11.1159 secondes

### Exercice 12
n² = 1024*1024 = 1048576, ca donne 11 secondes
n*log(n) = 3082, produit en croix donne 0.032 secondes

### Exercice 13
temps pratique de fft : 0.5353
parce qu'il n'y pas que le calcul de fft, il y a l'affichage et l'appel de cartesian to polar, et puis le temps théorique n'est qu'une estimation d'un paradigme mathématique.

### Exercice 14
Des fois on a un indice de 10 pour l'amplitude max, des fois on a 1014, parce que la sinusoide est conjugé-symétrique.
L'indice correspond aux échantillons. On arrête la recherche du maximum à FRAME_SIZE/2.
La vrai fréquence c'est index*Fe/FRAME_SIZE
On obtient une fréquence d'environ 430Hz, pas très précis par rapport à 410Hz.
Notre précision = Frequence trouvé + ou - Fe/2*FRAME_SIZE, c'est l'erreur.
Pour diminuer cette erreur, on peut augmenter FRAME_SIZE, et ainsi améliorer la précision.

Dilemme entre précision temporelle vs précision fréquentielle
Analyse de rythme : éléments rapides donc petit bach. Précision fréquentielle
tonalité d'un morceau : temps plus long donc frame_size plus grande. Précision temporelle

On veut quand même un précision temporelle avec nos 1024 échantillons, on arrange artificiellement l'échantillonnage en rajoutant
des 0 à la fin des 1024 échantillons jusqu'à 44100 (1s d'échantillons). On gagne en précision fréquentielle mais on fait des fft lourdes.
R.Q : si on a une précision réduite avec FRAME_SIZE, l'interpolation ne sera pas très utile.

D'après la théorie on devrait avoir une impulsion de Dirac en 440Hz, hors nous sommes dans le monde discret et on a pas de sinus de durée infinie.
convolution entre l'impuslion de dirac et la transformé de fourier du signal rectangulaire ==> on obtient un sinus cardinal centré sur
l'impulsion de Dirac.
Les lobes secondaires nous embêtent, on peut croire qu'il y a plusieurs sinusoïde. Le lobe principal doit être étroit.
On change le signal rectangulaire par différentes fonctions pour voir différents résultats sur l'impulsion obtenue.