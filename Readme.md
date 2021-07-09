# 3DSjoypad
A 3DS Homebrew app to use the console as a controller for pc,
currently in progress.

As devkitpro documentation suggest many of the source used here are from devkitPro/3ds-examples but modified, 
so please go to check they awesome job - https://github.com/devkitPro/3ds-examples.

### __current state__
- tracking of the keys and circle pad in a http server, done.
- decide on a driver or an open source library to simulate a pc controller device, in process.

## Dependencies
- devkitPro and it's tools installed i recommend following https://devkitpro.org/wiki/Main_Page guides,
but feel free to use any other guide you want.
- Node js installed
- and of course your should have your 3ds with homebrew installed on it.

##  Usage
- The ip addres used in the main.c http_post() function has to be your local ip address 
in order to connect the 3ds to the node server.
then from the project root dir :
    - `$ npm install`
    - `$ npm start`

- Generate the 3ds-src.3dsx from root/3ds-src dir with:
    - `make`
- Use 3ds-link to use the app in your 3ds or put the 3ds-src.3dsx file in the sd card 
and execute it with homebrew.


