# TP1 : formats sonores et forme d'onde

## Exercice 1
``sox BitterEnd.wav out.aiff``
``file BitterEnd.wav`` affiche l'entête du fichier.

   > BitterEnd.wav: RIFF (little-endian) data, WAVE audio, Microsoft PCM, 16 bit, stereo 44100 Hz.

## Exercice 2

``sox BitterEnd.wav out.raw``, diff de 44 octets, l'entête = 44 octets

## Exercice 3

``sox -r 44100 -c 2 -b 16 -e signed-integer son.raw truc.wav``

## Exercice 4

``sox BitterEnd.wav -c 1 BitterEnd2.wav`` différence de taille de 712

## Exercice 5
Intuition : aucune
``sox BitterEnd.wav -b 8 BitterEnd8Bit.wav``
``sox BitterEnd.wav -c 1 BitterEndMono.wav``
Les tailles sont similaires. En supprimant un canal on ralentie le son

# Affichage de l'onde sonore

1. On traite le son trame par trame
