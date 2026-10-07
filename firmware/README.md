# Snake : clavier USB et panneaux HUB75

Le firmware pilote deux dalles 64 × 32, assemblées en écran logique 64 × 64.
Les boutons physiques ne sont pas initialisés : les GPIO de l'écran restent
réservés à HUB75. Le brochage et la rotation des dalles sont dans `src/Config.h`.

## Compiler et utiliser

Depuis le dossier `firmware` :

```text
pio run
pio run -t upload
pio device monitor -b 115200
```

Brancher le port USB natif de l'ESP32-S3 (marqué **USB**). Si plusieurs ports
sont disponibles, préciser `--upload-port PORT` pour l'envoi du firmware et
`--port PORT` pour le moniteur. Fermer le moniteur avant un nouvel envoi.

Utiliser un terminal série qui transmet chaque caractère immédiatement,
sans attendre Entrée et sans contrôle de flux. Le moniteur PlatformIO permet
de transmettre les touches directement ; ne pas activer son mode de saisie
par ligne. Les commandes fonctionnent aussi en majuscules :

| Touche | Action |
| --- | --- |
| Q | Tourner à gauche par rapport au déplacement actuel |
| D | Tourner à droite par rapport au déplacement actuel |
| T | Lancer la mire de diagnostic pendant six secondes |

Les autres caractères et fins de ligne sont ignorés. Si plusieurs commandes
de direction sont reçues dans une même lecture, la dernière est retenue.
Maintenir une touche peut provoquer des répétitions émises par le PC.

## Vérifier l'affichage

La mire affiche successivement, une seconde par étape : noir, rouge, vert,
bleu, moitié logique haute blanche, moitié logique basse blanche. Le nom de
l'étape est affiché dans le terminal. Le jeu est suspendu pendant la mire ;
les commandes reçues sont ignorées et une nouvelle partie démarre à la fin.

Vérifier que le noir éteint tous les pixels, que chaque couleur couvre les
deux dalles et que les moitiés blanches correspondent à leur position dans
l'assemblage. Puis vérifier Q/D et le passage du Snake entre les dalles.
La rotation de la seconde dalle reste celle de la configuration existante.

Si des bandes persistent, comparer le câblage réel au brochage HUB75 de
`src/Config.h`, notamment R2/G2/B2 sur GPIO 7/15/16, et vérifier la masse
commune entre ESP32 et panneaux. La mire permet de constater le défaut ;
elle ne corrige pas un problème de câblage ou un type de dalle incompatible.
