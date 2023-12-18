# Flappy Bird on TM4C123
My project is to create a clone of the then popular Flappy Bird mobile game on the TM4C123 in which a player controls a bird, attempting to fly between columns of green pipes without hitting them. The game is rendered on the TFT display, featuring high quality transparent sprites. The bird would be controlled by a tactile button press.

DEMO: View the finish product [here](https://youtu.be/Iz33P4g_INo)

### The project implements the following threads, semaphores, schedulers, and buffers, inter-process communication:
1. Threads
- Update_Bird (update bird position and check for collision)
- Update_Pipes (spawn and update ceiling and ground pipe arrays)
- Check_ButtonPress (check for button press)
- Update_Score
2. Semaphores
- Button_Debounce
- Interface_TFT
- Buffers
- Button_FIFO
- Score_FIFO
### The project implements the following data structures: 
- Data Structures
- Bird struct (tracks birdPosY and birdSpeed)
- Pipe struct (tracks X and Y position)
- Arrays for ceiling pipes and ground pipes

![Testing transparent sprite rendering with scaling](https://raw.githubusercontent.com/zCriminalArtist/EEL4745C-Final-Project/master/rendering.png "Testing transparent sprite rendering with scaling")
