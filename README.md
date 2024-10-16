# Snake in Raylib
#### This is created out of fun in my spare time.

#### It visually resembles the Snake game offered by Google on the browser.

## For compilation, first install [Raylib](https://github.com/raysan5/raylib) and then use the included Makefile

## Controls:
- WASD or arrows keys for movement
- ESC to close game

## Game implementations:

- Snake movement is done only when near a grid-square's center (it would have been easier to have the snake move a full square at a time but this way it has smoother motion). Because of this restriction, memorising only one direction at a time would feel laggy (since a player could quickly tap 'left' and 'up' but have snake only go left). This is why up to 2 distinct directions are saved at a time to make it feel more responsive (for instance hitting 'left' and 'down' when going upwards will have snake turn around when it can so that it sticks to the grid).

- The snake is a singly linked list of snake segments. When eating the food, a new segment is added on top of the snake head, followed by a pause of a few frames in which the head moves forward but the rest of the body stands still (that way the segments are spaced out correctly). This is similarly seen in Google's implementation as eating does not instantly add a segment at the tail of the snake (which is a benefit since this way there are no concerns if/where the segment would be placed).
