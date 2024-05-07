# COMP3015-CW2
 

The COMP3015-CW2 project was created using the provided Project_Template in Visual Studio 2022 Community (Version 17.9.6) on Windows 11 (Version 23H2 OS Build 22631.3527).
The user can find the exe in the folder 'COMP3015-CW2 Exe' within the DLE submitted .zip file.

This project originated from my previous submission for the CW1 project, with different features being added/removed. The expansions since the first project are: Shadow Maps, Physical Based Rendering (PBR). The model used was the same as the model used in CW1, which is a streetlight that I created myself with Blender.

The code works through the scenebasic_uniform.cpp and scenebasic_uniform.h files, as well as through the use of different shader files consisting of: basic_uniform.vert, basic_uniform.frag, solid.frag, solid.vert, PBR.frag, and PBR.vert. The three shaders used in the project are prog, solidProg and PBRprog. They all handle different shaders in the project and have their own progs within the code to keep them separated.

The shaders that I chose to implement were the Shadow Map and the PBR. These added details to my streetlight model such as a shadow, as well as giving it a metallic look with PBR.
Some issues were with the texture mapping and the freecam implementation. The textures would be rendered on top of the shadows so they were not visible in the scene, they were removed to show the shadows. The freecam clashed with the other camera views set in the code and the shadows were rendered from the freecam when looking around which made the scene look bad.

I have achieved a customly implemented model which looks amazing with PBR shaders added to them, as well as the shadows being cast off of them.

Video link - https://youtu.be/FBwm_KU7KBU
