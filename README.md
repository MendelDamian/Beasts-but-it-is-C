# Beasts but it's C
The game was written as a project of "Operating Systems" classes to consolidate knowledge of threads and IPC.


## Authors

- [@MendelDamian](https://www.github.com/MendelDamian)


## Gameplay

The game is played on a 2D grid. Each player controls a single unit, which can move around the grid.
The goal is to earn as many points as possible by picking up cash tiles and bring them to the campsite.
The players can also attack each other, which will result in dropping the cash they are carrying.
To make game harder, there are also some beasts on the map, which will attack the players if they are close enough.


## Keybindings
### Server
- `q`/`Q` - close the server
- `b`/`B` - spawn a beast
- `c` - spawn a coin
- `t` - spawn a treasure
- `T` - spawn a large treasure

### Client
- `q`/`Q` - quit the game
- `w`/`W`/`&uarr;` - move up
- `s`/`S`/`&darr;` - move down
- `a`/`A`/`&larr;` - move left
- `d`/`D`/`&rarr;` - move right

## Demo

![Server preview](demo/server-preview.jpg)
![Client preview](demo/client-preview.jpg)


## Tech stack

- UNIX Sockets
- Threads
- ncurses - graphics library


## Build

To build this project run

```bash
  make
```
If you want to build with debug flags, run

```bash
  make DEBUG=1
```

## Run Locally

Clone the project

```bash
  git clone https://github.com/MendelDamian/Beasts-but-it-is-C
```

Go to the project directory

```bash
  cd Beasts-but-it-is-C
```

Start the game by using Makefile

```bash
  make run
```
or directly
```bash
  ./Beasts
```

