Bowling scene project
======================

This project is part of the Virtual reality course (INFOH-502) at ULB. It implements a mini bowling game with the OpenGL library in C++.

# Clone the project

To clone the project with the libraries, just type:

```bash
git clone --recursive https://github.com/Nicolas-Guilbaud/INFO-H-502.git
```

# Build with C-Make

This project uses C-Make to build the binaries. Simply load the CMakeLists.txt then build as usual.

# Controls

The scene has 3 static cameras and a free camera (4th). 
These can be rotated with the mouse.

Some keys are used to switch between them, navigate with the free camera, launch the ball and restart the game.

| Key   | Usage                                           |
|-------|-------------------------------------------------|
| TAB   | Change to the next camera                       |
| A     | Move the free camera to the left                |
| S     | Move the free camera backwards                  |
| D     | Move the free camera to the right               |
| W     | Move the free camera forwards                   |
| LEFT  | Rotate the free camera to the left              |
| RIGHT | Rotate the free camera to the Right             |
| ENTER | Launch the ball. Works only on the first camera |
| R     | Restart the game.                               |

# Usefull links used during the project

- [Bowling and pins object files](https://free3d.com/fr/3d-model/-bowling-ball-v1--953922.html)
- [The cubemap texture](https://www.humus.name/index.php?page=Textures)
- [The UV texture for the ground](https://ambientcg.com/a/Planks009)
- [How to perform UV map editing on blender](https://www.youtube.com/watch?v=qa_1LjeWsJg)
