Project: Fireworks
Rabin Ranabhat

The project's main focus is to simulate particle effects and do things like fireworks. When starting the program, you can see the fireworks start.

Things to consider:
1. When the firework is rising, it has a trail. It has been achieved by using a buffer that will store the last 10 positions of the firework. In each frame, the buffer is rendered, however, the alpha value applied for blending will be highest for the most recent spot and lowest for the oldest position in the buffer.
2. On right clicking the screen and selecting the rendering type 'Trails', the exploding particles of the fireworks will also show a trail before vanishing.
3. On exploding, the fireworks will affect the ambient color of the rendered scene. I have left the boundary for the pool white so that this effect can be easily seen.
4. The water is the feature I am most proud of. If you navigate into the pool, you can see the water moving. This is best viewed by pressing the 'q' button, that will increase the height of the camera. On the water surface, I have applied a texture that makes the water simulation even more prominent. Look closely at the water surface, the texture MOVES along with the water. You can toggle the texture on/off by pressing the 't' button. Very proud of this feature. Took a long time to implement.
5. The water also shows the reflection of the trees around the pool. I couldn't figure out how to make the reflections move like the water surface. Nonetheless, that's another feature I am proud of in this project.
6. I have also included the fog feature in the scene. This feature can be toggled on/off by pressing the button 'f'.
7. I am also proud of the tree objects around the pool. Even though I didn't create the objects from scratch, I modified the .mtl file so that the tree would render properly along with proper texture being applied to the tree trunk and the leaves.
8. I have also included another object for decoration purposes around the pool. However, you have to press the button 'w' to see the objects because it slows down my computer considerably. If your machine is up to the par, press the button and please enjoy!
9. I have changed the loadtexbmp.c file where you can flag it to use mipmap function when loading the texture or not use it. The flag to use the mipmap option is '1', other '0' can be used as the flag.

NOTE: Sorry the code isn't pretty! I wish I had more time to pretty it up.

The following keys perform the following functions:
1. Right click on the screen shows the options that will let you control the parameters for the fireworks.
2. Left key: Will rotate left.
3. Right key: Will rotate right.
4. Back key: Will move back.
5. Forward key: Will move forward.
6. Page Up: Will look up.
7. Page Down: Will look down.
8. 'T/t': Will toggle the texture on the water surface.
9. 'W/w': Will display the extra pillar objects on the sides of the pool.
10. 'F/f': Will toggle the display of fog.

