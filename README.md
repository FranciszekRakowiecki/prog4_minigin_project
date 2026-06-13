# Linkie to projecto
https://github.com/FranciszekRakowiecki/prog4_minigin_project

# Design choices:

## Input system

I mainly was inspired by the unity input system and having working with the old one and transitioning to the new one I believe I am most comfortable with this setup of still having bool checks for key presses, on on frame key press and release instead of relying strictly on the OnPerformed events. This forms a much simpler approach to taking input, instead of having a callback that needs to be freed this format leaves one extra thing to think about. The callback was much more appealing when there existed only one instance that kept check of key presses in the ```PlayerInputManager``` class inside the game folder. 

Another issue with the callbacks is checking if the key is held down. If something like the WSAD input were to instead use callbacks, it would mean that there would have to exist two callbacks to each set the state of the key inside the instance (not super optimal i think just imo) and i already rule out calling in a callback for "key is still pressed" because well its a callback that contains information about the key being pressed or what gamepad the gamepad button is pressed or mouse.

There are use cases for both but i have settled on centralizing my approach to make use of these state bools stored on the InputKey or InputButton or GamepadButton. 

### PlayerInputManager + Handler

The game input manager that I wrote out for managing active players in the game was a really simple approach to keeping track of what kind of input device is being used and at the same time normalizing the output to a single vec2 or bool. I just think I designed it well instead of having all devices being checked I instead just check for the player input before initializing the handler.

## Audio system

When considering the final product for the engine I felt like the use of the base sdl audio system was plenty for making it work with the game as I expected all the sounds to be played in mono, therefore there was no need for complex solutions for playing audio. The game also is strictly 2D and and arcade game, there is no true reason to implement something like locational audio or extra processing for sound effects.

## Renderer

So one of the main design choices I settled on is to not have a general purpose renderer component instead simply drawing shapes for the level out of one component just using the Renderer singleton made this setup simplified. Consider this: if i wanted to draw each cube of the level I'd have to create a renderer component for each of the cubes. This is well stupid. Obviously there is a way to buffer the cube locations into a texture and draw one texture in one sweep but I didn't think of it until writing this just now. But there is no reason on why this apporach is bad either well obviously its not reusable outside of the Game project, but is it going to be? does it have to? This approach gave me freedom to render the tank how I wanted or the level how I wanted.

TextRenderer on the other hand is much more versatile and expensive to draw by creating a rect every frame. This is a good approach and delivers a good setup for efficiently managing the Text class. This is more fit for a component because it stores a lot of data that needs to persist for reuse.