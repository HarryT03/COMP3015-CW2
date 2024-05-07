# COMP3015-CW2
 
How does the user interact with your executable? How do you open and control the software
you wrote (exe file)?
-----------------------------------------------------------------------------------------
• How does the program code work? How do the classes and functions fit together and who
does what?
-----------------------------------------------------------------------------------------
• What makes your shader program special and how does it compare to similar things? (Where
did you get the idea from? What did you start with? How did you make yours unique? Did you
start with a given shader/project?)
-----------------------------------------------------------------------------------------
• Include a link to a video report that details the above but also goes into more detail on:
o Anything else which will help us understand how your shader works.
-----------------------------------------------------------------------------------------
o Are there any software engineering issues, such as the trade-off between performance
and good practice?
-----------------------------------------------------------------------------------------
o A (brief) evaluation of what you think you have achieved, and what (if anything) you
would do differently, knowing what you now know. Don’t be modest but be realistic!
-----------------------------------------------------------------------------------------

The COMP3015-CW2 project was created using the provided Project_Template in Visual Studio 2022 Community (Version 17.9.6) on Windows 11 (Version 23H2 OS Build 22631.3527).

This project originated from my previous submission for the CW1 project, with different features being added/removed. The expansions since the first project are: Shadow Maps, Physical Based Rendering (PBR). The model used was the same as the model used in CW1, which is a streetlight that I created myself with Blender.

The code works through the scenebasic_uniform.cpp and scenebasic_uniform.h files, as well as through the use of different shader files consisting of: basic_uniform.vert, basic_uniform.frag, solid.frag, solid.vert, PBR.frag, and PBR.vert. The three shaders used in the project are prog, solidProg and PBRprog.

The shaders that I chose to implement were the Shadow Map and the PBR. These added details to my streetlight model such as a shadow, as well as giving it a metallic look with PBR.
