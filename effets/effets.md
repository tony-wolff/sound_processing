# Les effets en son

Conversion d'un fichier audio stereo en mono
> ```sox fichier.wav -c 1 fichier_mono.wav```

Exemple d'un applyplugin :
applyplugin BitterEndMono.wav out.wav /usr/lib/ladspa/flanger_1191 flanger 6.325 2.5 0.33437 0

**VocalRemover**\
Création d'un effet karaoké (vocal remover).
Comme la voix est supposé centré (cad aussi forte à droite qu'à gauche), pour supprimer celle-ci on fait le canal audio droit - canal audio gauche. Bien sûr ce n'est pas magique et ca atténue les autres instruments centrés.

> ```applyplugin BitterEnd.wav BitterEndKarak.wav vc.so vocalremover```

**Amplitude**\
Modifier la fonction run, modifier l'intensité du son (augmenter/diminuer), on multiplier chaque échantillon par un réel (> 1 on augment le vol, <1 on diminue le volume)

> ```applyplugin BitterEnd.wav bitterEndAmplified.wav amp.so plugin 0.5```

diminue le volume (avec 0.5)

**NoiseGate**\
Calcul de l'energie sur la trame...
Si energie > certain seuil, on laisse passer le son, sinon on output du silence

> ```applyplugin TP_NoiseGate.wav out_gated.wav noisegate.so plugin 0.01```
