
# Projet Voxel Terrain Génératif 🌍

Ce projet en C++ avec OpenGL, GLFW3 et Glad recrée un terrain voxel procédural inspiré de Minecraft. Le monde est divisé en chunks, chaque chunk étant généré dynamiquement à mesure que le joueur se déplace. Le système optimise la génération et le rendu en ne créant que les faces visibles des chunks, tout en incluant des éléments comme des biomes, du relief et du foliage.

## Fonctionnalités ✨

- **Load/Unload des Chunks** 🧩 : Les chunks sont chargés ou déchargés dynamiquement en fonction de la position de la caméra, permettant ainsi un monde évolutif sans surcharge de mémoire.
- **Frustum Culling** 👁️ : Seules les parties du monde visibles par la caméra sont rendues, ce qui améliore les performances.
- **Biomes** 🌵🏔️ : Deux biomes sont disponibles : désert et montagnes.
- **Relief Procédural** 🌄 : Le relief du terrain est généré à l'aide de Perlin Noise, créant des variations naturelles sur le terrain.
- **Foliage** 🌿 : Un peu de végétation est ajoutée au terrain pour rendre l'environnement plus vivant.
- **Optimisation des Meshs** 🏗️ : Chaque chunk utilise un seul mesh, et seules les faces visibles sont générées, réduisant ainsi le nombre de vertices et améliorant les performances.

![frame_0_21_38f](https://github.com/user-attachments/assets/f036406a-a9c0-432a-8808-8ad177eb6853)
![frame_0_13_38f](https://github.com/user-attachments/assets/92e6703f-29cc-4acb-be53-fa744eecd13a)
