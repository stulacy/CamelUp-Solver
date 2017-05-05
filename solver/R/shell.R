
# 2 camels on both tile 1 and 2 with last one on 3. One forward trap on tile 4.
gamestate <- matrix(FALSE, nrow=16, ncol=7)
gamestate[1, c(1, 3)] <- TRUE
gamestate[2, c(2, 4)] <- TRUE
gamestate[3, 5] <- TRUE
gamestate[4, 6] <- TRUE

# Second dice has been rolled
dice <- logical(5)
dice[2] <- TRUE

#solve(gamestate, dice)
