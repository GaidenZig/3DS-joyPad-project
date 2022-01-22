# 3DSjoypad
A 3DS Homebrew app to use the console as a controller for pc, using a node js server to connect the console with a vjoy driver.

As devkitpro documentation suggest many of the source used here. came from devkitPro/3ds-examples, i just adapt them to make this work,
so please go to check they awesome job - https://github.com/devkitPro/3ds-examples.

### __current state__
- previsional version, that can be run with node js npm commands.
- trying to improve the performance in the button's press reaction.

## Dependencies
- vjoy https://sourceforge.net/projects/vjoystick/
- devkitPro and it's tools installed i recommend following https://devkitpro.org/wiki/Main_Page guides,
but feel free to use any other guide you want.
- Node js installed
- and of course your should have your 3ds with homebrew installed on it.

##  Usage
- make an .env file on the root folder and write in it a "VJOY_BUILD" variable pointing to 
your JoyMonitor.exe directory. Example of a variable path:

    - `VJOY_BUILD = D:/Program Files/vJoy/x64`

then from the project root dir :
    - `$ npm install`
    - `$ npm run dev`
- then if everything goes well, JoyMonitor.exe has to be opened and the server listening.

- Generate the 3ds-src.3dsx from root/3ds-src dir with:
    - `make`
- Use 3ds-link to use the app in your 3ds or put the 3ds-src.3dsx file in the sd card 
and execute it with homebrew.
- finally once with the app open in 3ds, press the A to write your pc's local ip adress were you previously run the node js server.


