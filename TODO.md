
# Résumé global

- [x] Setup
    - [x] create the project with the basic structure (CMAKE, etc)
    - [x] create a basic scene with a cube
- [ ] Physics
    - [ ] Mecanic system
    - [ ] Collision detection
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
   - [ ] familiarisation avec les fonctionnalités git (fetch, checkout, merge) via la page https://learngitbranching.js.org/?locale=en_US
   - [ ] prise en compte de l'influence de la concavité des quilles dans la configuration de détection des collision
   - [ ] implémentation d'une scène dynamique

- Nicolas:
   - [x] reformattage des fichiers .obj: corriger les faces à 4 sommets de la boule de bowling, scinder le triplet de quilles
   - [ ] alternativement prendre en compte les faces à 4 sommets dans le parser de fichiers .obj

## Résumé
