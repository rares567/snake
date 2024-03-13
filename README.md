# Snake in Raylib
This is created out of fun in my spare time.

It visually resembles the Snake game offered by google on the browser.

Snake movement is done only when near a grid-square's center (it would have been easier to have the snake move a full square at a time but this way it has smoother motion). Because of this restriction, memorising only one direction at a time would feel laggy (since a player could quickly tap 'left' and 'up' but have snake only go left). This is why up to 2 distinct directions are saved at a time to make it feel more responsive (for instance hitting 'left' and 'down' when going upwards will have snake turn around when it can so that it sticks to the grid). This functionality seems to be existent even in Google's browser version (which may contriute to its fluidity).
