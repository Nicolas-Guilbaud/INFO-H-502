
# Résumé global

- [x] Setup
    - [x] create the project with the basic structure (CMAKE, etc)
    - [x] create a basic scene with a cube
- [ ] Physics
    - [ ] Mecanic system
    - [x] Collision detection
    - [ ] Light
- [ ] File loading
    - [x] Load a mesh
    - [ ] Load a texture
    - [x] Load a shader
    - [ ] Load the entire scene
- [ ] Interactions:
    - [ ] Launch the ball
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
- Restructurer le code orienté objet, plus particulièrement la classe object.h qui doit être scindée en sous-classes
- Nicolas:
  - [ ] ajouter le cubemap

- Loup:
  - [ ] régler problème de framerate
  - [ ] régler interactions quilles-sol
  
