
# Résumé global

- [x] Setup
    - [x] create the project with the basic structure (CMAKE, etc)
    - [x] create a basic scene with a cube
- [ ] Physics
    - [x] Mecanic system
    - [x] Collision detection
    - [ ] Light
- [ ] File loading
    - [x] Load a mesh
    - [x] Load a texture
    - [x] Load a shader
    - [ ] Load the entire scene
- [x] Interactions:
    - [x] Launch the ball
- [ ] others:
    - [ ] code the logic behind the game
    - [ ] portal 2 logic (used in the level design)

# Réunion 6/02/2025

## Objectifs

- Loup:
    - [x] se pencher sur le dynamicWorld -> transfert auto des momentums
- Nicolas:
    - [ ] quille et boule de bowling
      - [ ] meshes
      - [ ] textures
    - [x] débugger l'erreur de compilation

## Résumé

- Loup: 
    - transition enclenchée du btCollisionWorld vers le btDynamicsWorld et rétrocompatibilité avec scène de base
    - documentation concernant le rendu OpenGL d'une scène dynamique
- Nicolas:
    - résolution de problèmes de parser (segmentation faults)
    - obtention des meshes, erreurs rencontrées et commit dans une nouvelle branche (textures)
 
 # Réunion 13/02/2025

## Objectifs
- Loup:
   - familiarisation avec les fonctionnalités git (fetch, checkout, merge) via la page https://learngitbranching.js.org/?locale=en_US
   - prise en compte de l'influence de la concavité des quilles dans la configuration de détection des collision
   -  implémentation d'une scène dynamique

- Nicolas:
   - reformattage des fichiers .obj: corriger les faces à 4 sommets de la boule de bowling, scinder le triplet de quilles
   - alternativement prendre en compte les faces à 4 sommets dans le parser de fichiers .obj

## Résumé

### Nicolas 

Le mesh des quilles et de la boule de bowling a été triangularisé sous blender.

Début des textures mais ça ne fonctionne pas

Il faudrait nettoyer le code et l'architecture.  
-> peut-être faire un diagrame UML.

### Loup:

Scène dynamique terminée.

Tuto git réalisé

Objets concaves: séparer en objets convexes
-> risque de problème avec les textures  
-> attendre d'avoir les textures

Avoir 1 classe mère Object et 2 classes filles pour distinguer les objets concaves et convexes

Risque sur les prochaines tâches: l'adhérence au sol

# Réunion 20/02/2025

## Objectifs

- [x] Se procurer le chapitre 9

- Loup:
  - [x] setup la piste avec les quilles
    - [x] avoir un sol
    - [x] avoir des quilles en triangle
    - [x] faire rouler boule de bowling vers les quilles
- Nicolas:
  - [x] textures sur les meshes
  - [ ] cubemap

## Résumé
- Nicolas a ajouté les textures aux objets, il s'est brièvement penché sur la question du cubemap mais il faut d'abord restructurer le code (en collaboration le samedi 22/02) 
- Loup a créé le latex pour le rapport et configuré une scène de bowling basique. Cependant problème de framerate dû au temps de calcul et interactions quilles-sol non-réalistes
- Entrevue avec l'assistante lors de la séance de questions-réponses: procuration du chapitre 9 du livre de référence, conseils pour fluidifier la scène dynamique

# Réunion 27/02/2025

## Objectifs

- [x] Restructurer le code orienté objet, plus particulièrement la classe object.h qui doit être scindée en sous-classes
- Nicolas:
  - [x] ajouter le cubemap

- Loup:
  - [x] régler problème de framerate
  - [x] régler interactions quilles-sol

## Résumé

Les hitbox des quilles sont désormais cubiques  
-> tester avec des hitbox cylindriques

Cela a également réglé les problèmes de framerate

Le cubemap a été ajouté sur une autre branche (doit être merged)

Fcts basiques qu'il reste à implémenter:

- Game logic:
  - choix direction + force
- plusieurs caméras statiques (en + de la caméra libre)
- réflection/refraction

Avant de continuer les lumières, il a été convenu de lire le chapitre 9
et d'en faire un résumé pour préparer la défense orale

# Réunion 06/03/2025

## Objectifs

- [x] Lecture Chapitre 9

Loup:
    - [ ] Revoir les labos (FBO, lumières, textures)
    - [x] Setup les caméras (1 libre, plusieurs statiques)
Nicolas:
    - [ ] Textures: sol, cubemap
    - [ ] Game logic

## Résumé
- Début du chapitre 9 lu par Nicolas (jusque 9.5) et survolé par Loup (9.3, 9.4) qui souhaite aller à l'essentiel étant donné les instructions vagues du professeur

Loup:
    - Trop ambitieux de refaire tous les labos en une semaine. Système de 4 caméras set up dont une amovible. 
Nicolas:
    - Pas eu le temps de réfléchir à l'implémentation de la Game logic de manière substantielle. A dégoté un mesh d'une 100aine d'objets représenatant un complexe de loisir avec des pistes de bowling. 

# Réunion 13/03/2025

## Objectifs

- [ ] Lecture ciblée du Chapitre 9 au vu de programmer les shaders d'ici à la fin du mois. En particulier , ceux sur la microgeometry (9.6), la reflectance (9.5) et la BRDF for surface reflection (9.8).

Loup: 
    - [x] Régler bug de désaccord entre hitbox et rendered mesh
Nicolas:
    - [x] Implémentation d'un mesh texturisé pour le sol
    - [x] Implémentation de la cubemap

## Résumé (12/03/2025)

Lecture du chapitre 9:

- 9.6: pas beaucoup de contenu
- 9.8: besoin de lire la section 9.7 pour comprendre d'où vient l'équation

implémentation de la section 9.5 (réflectance Fresnel) par Loup

Problème de boule de bowling buguée  
-> résolu en ajoutant un offset dans la hitbox

Quilles oscillent à nouveau  
-> dû à l'inertie

# Réunion 20/03/2025

## objectifs

Loup: 
  - [ ] Lancer de la balle
  - [ ] Game logic
  - [ ] Régler l'oscillation des quilles (à voir si c'est réglé avec le sol)
  - [x] Ecriture Fresnel dans le rapport
Nicolas:
  - [ ] Chap 9.8
    - [ ] Implémentation
    - [ ] Ecriture dans le rapport
  - [x] Mirroir FBO

## résumé

Nicolas:
    - pas eu l'occasion de travailler sur le chap. 9.8 car malade
    - implémentation d'un mirroir dans une branche séparée à l'aide d'un FBO
    - ajout de fonctions utilitaires pour manipuler les modèles d'objets
Loup:
    - un peu écrit dans le rapport
    - a ajouté la reflection et la refraction malgré meshes mal rendues

# Réunion 27/03/2025

## objectifs

[ ] Merge branche mirroir et refactor du code (reflectable et refractable object classes, régler duplication du rendering ds main, modifier constructeur du rigidObject et appeler setRigidObject en privé) 

Nicolas:
    - [ ] documentation au vu d'implémenter le physically-based rendering (specular reflection) d'ici au 03/04

Loup: 
    - [x] régler reflection/ refraction sur meshes fonctionnelles
    - [ ] hitbox sphérique pour la balle de bowling
    - [ ] réflechir à la game logic au vu de son implémentation d'ici au 03/04

## résumé

### Loup:

- refactor du code a corrigé la réflection/réfraction
- résolution problème des quilles dans une autre branche  
-> initialisation des boxes doivent être à moitié dans le sol    
-> shift du modèle associé à la hitbox pour que ça soit visuellement sur la piste
- hitbox: boule de bowling = cube -> glisse

Game logic:

- appuyer sur enter lance la balle
- vecteur front de la camera = direction du lancer
- comptage points se fera avec une machine à état  
-> besoin de trouver comment détecter les quilles tombées
- compteur: dans la console puis dans un coin de l'écran


### Nicolas:

partie sur le BRDF rédigé dans le rapport
-> à implémenter

# Réunion 03/04/25
## Objectifs:

### Loup:

- [ ] avoir l'effet de la balle qui roule
- [x] game logic:
    - [x] détection des quilles tombées
    - [x] calcul des points
    - [ ] affichage des points

### Nicolas:

- [x] implémenter le BRDF

## Résumé:

### Loup:
- pas d'effet de balle qui roule car incertitude sur la valeur des fps, affichage instable
- game logic comme state machine dans la boucle de rendering avec bug d'affichage lié à la transition du rendu statique vers dynamique
- calcul des points ne prenant pas en compte strikes et spares
- affichage des points dans le terminal mais pas sur l'écran à ce stade

### Nicolas:
- implémentation du physically-based rendering via la BRDF: facteur de la réfléctance de Fresnel avec Schlick's approximation, facteur de masking-shadowing, facteur de distribution GGX

# Réunion 10/04/25
## Objectifs: 

### Loup: 

- [x] détecter collisions boules-quilles comme marqueur de transition entre rendu dynamique/statique des modèles quilles et balles
- [ ] implémenter rendu de texte dans le plan de la caméra pour l'affichage des points

### Nicolas:
- [x] modifier les paramètres de la BRDF pour un éclairage plus contrasté et lumineux
- [x] réparer le redimensionnement dynamique de la fenêtre (viewport)
- [x] refactor la game logic dans une classe à part au vu de l'implémentation de mécanismes plus compliqués la semaine d'après comme les strikes et les spares

### Pour plus tard:
- amélioration luminosité par l'ajout de plusieurs sources
- écriture du rapport, screenshots des features basiques
- écriture d'un README.md
- création d'une vidéo mp4 et présentation des features principales
