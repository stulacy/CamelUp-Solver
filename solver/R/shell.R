library(RcppArmadillo)

# 2 camels on both tile 1 and 2 with last one on 3. One backwards trap on tile 4.
gamestate <- matrix(FALSE, nrow=18, ncol=7)
gamestate[1, 1] <- 1
gamestate[2, 2] <- 1
gamestate[1, 3] <- 2
gamestate[2, 4] <- 2
gamestate[3, 5] <- 1
gamestate[4, 7] <- 1

# Second dice has been rolled
dice <- c(1, 3, 4, 5) - 1

set.seed(3)
solve(gamestate, dice, 100)


