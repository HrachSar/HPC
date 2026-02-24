
Assignment 1: Dice Game with POSIX Barriers
Objective

Implement a multi-threaded dice game, where multiple players roll a dice in each round. The game is synchronized using POSIX barriers, ensuring all players finish rolling before determining the winner of each round.
Task Breakdown

    The game consists of P players (threads).
    Each player rolls a six-sided dice (rand() % 6 + 1).
    All players must wait at a POSIX barrier before proceeding to determine the round winner.
    After all players have rolled their dice, the winner of the round is displayed.
    The game runs for R rounds.
    At the end of all rounds, the player with the most wins is declared the overall winner.

Requirements

    Use pthread_barrier_t to ensure all players finish rolling before finding the winner.
    If needed use pthread_mutex_t to protect shared data.
    Use rand() to generate dice rolls.

Assignment 2: Multiplayer Game Lobby Synchronization
Objective:

Simulate a multiplayer game lobby, where all players must be ready before the game starts.
Task:

    Create N player threads.
    Each player simulates "getting ready" (sleeping for a random time).
    Players must wait at a barrier before the game starts.
    Once all players are ready, they print "Game Started!".

Assignment 3: Weather Data Collection System
Objective:

Implement a distributed weather station where sensors synchronize data collection before processing it. 
Task:

    Create S sensor threads.
    Each sensor:
        Collects temperature data. Simulate some values.
        Waits at a barrier for all sensors to finish.
        Once all sensors collect data, they proceed to processing.
        Calculate the average temperature for the given period
    The main thread initializes and destroys the barriers.

Assignment 4: Multi-Stage Pipeline Simulation
Objective:

Implement a 3-stage pipeline where worker threads must synchronize at each stage.
Task:

    Create M threads.
    Each thread performs three stages of execution.
    Synchronize threads at each stage using POSIX barriers.
    Either use one barrier and reset it between stages, or three separate barriers.


