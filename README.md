
# Projet Voxel Terrain Génératif 🌍

**⚠️ :** Ce projet est en cours de développement et certaines fonctionnalités peuvent être incomplètes, bugguées ou sujettes à des modifications. N'hésitez pas à partager vos retours ou suggestions !

##

Ce projet en C++ avec OpenGL, GLFW3 et Glad recrée un terrain voxel procédural inspiré de Minecraft. Le monde est divisé en chunks, chaque chunk étant généré dynamiquement à mesure que le joueur se déplace. Le système optimise la génération et le rendu en ne créant que les faces visibles des chunks, tout en incluant des éléments comme des biomes, du relief et du foliage.

## Fonctionnalités ✨

- **Load/Unload des Chunks** 🧩 : Les chunks sont chargés ou déchargés dynamiquement en fonction de la position de la caméra, permettant ainsi un monde évolutif sans surcharge de mémoire.
- **Frustum Culling** 👁️ : Seules les parties du monde visibles par la caméra sont rendues, ce qui améliore les performances.
- **Biomes** 🌵🏔️ : Deux biomes sont disponibles : désert et montagnes.
- **Relief Procédural** 🌄 : Le relief du terrain est généré à l'aide de Perlin Noise, créant des variations naturelles sur le terrain.
- **Foliage** 🌿 : Un peu de végétation est ajoutée au terrain pour rendre l'environnement plus vivant.
- **Optimisation des Meshs** 🏗️ : Chaque chunk utilise un seul mesh, et seules les faces visibles sont générées, réduisant ainsi le nombre de vertices et améliorant les performances.

## 📸 Screenshots  
![MinecraftVideo1](https://github.com/user-attachments/assets/9d28a5a9-cc18-4034-92b4-048dce0d816f)
![frame_0_04_9f](https://github.com/user-attachments/assets/9d9ea5e4-e45b-4940-9bb7-8a31441c4eda)
![frame_0_22_7f](https://github.com/user-attachments/assets/9fee71eb-dc6f-48ab-9cbc-1ff1f797d860)
![frame_0_37_56f](https://github.com/user-attachments/assets/6170b2e7-8dcf-4618-b8f9-201095abfc52)
![frame_0_40_26f](https://github.com/user-attachments/assets/1ecbd093-daa9-4c9a-8a3e-761cb7ae3cfa)

## 🔄 Roadmap

Voici les prochaines évolutions du projet :

- ⚡ Génération en multithread pour améliorer les performances.
- 🏞️ Modifier la manière de gérer les textures pour pouvoir intégrer des mipmaps
- 🌟 Amélioration du système de lumière pour un rendu plus réaliste.
- 👤 Ajout d'un player avec un système de physique basique et de collisions avec les blocks.
- ⚒ Pouvoir casser et ajouter des blocks pour une interaction plus immersive.
- ♻️ Refactorisation du code pour corriger les warnings et les fuites de mémoire.

Stay tuned pour les prochaines mises à jour ! 🚀
