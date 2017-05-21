#include <Rcpp.h>
#include <stack>
#include <math.h>
#include "board.h"
#include <iostream>
using namespace Rcpp;

// wrapper around R's RNG such that we get a uniform distribution over
// [0,n) as required by the STL algorithm
// Code taken from http://gallery.rcpp.org/articles/stl-random-shuffle/
inline int randWrapper(const int n) { return floor(unif_rand()*n); }

Rcpp::IntegerVector shuffle(Rcpp::IntegerVector a) {
    // clone a into b to leave a alone
    Rcpp::IntegerVector b = Rcpp::clone(a);
    std::random_shuffle(b.begin(), b.end(), randWrapper);
    return b;
}

void simulate_round(IntegerMatrix boardstate, IntegerVector input_dice, IntegerMatrix& results) {
    // Setup board with camel stacks 
    std::vector<int> camel_positions;
    for (int i=0; i < 5; ++i) {
        for (int j=0; j < boardstate.nrow(); ++j) {
            if (boardstate(j, i) > 0) {
                camel_positions.push_back(j);
                continue;
            }
        }
    }
    
    Board board(boardstate);
    bool race_won = false;
    int leg_num = 0;
    
    while (! race_won) {
        
        // Shuffle dice
        IntegerVector shuff_dice;
        if (leg_num == 0) {
            shuff_dice = shuffle(input_dice);
        } else {
            shuff_dice = shuffle(Rcpp::IntegerVector::create(0, 1, 2, 3, 4));
        }
        std::vector<int> dice_stack = Rcpp::as<std::vector<int> >(shuff_dice);
        
        leg_num++;
        
        while (!dice_stack.empty()) {
            int rolled_camel = dice_stack.back();
            dice_stack.pop_back();
            // get camel tile id from camel_positions
            int camel_position = camel_positions[rolled_camel];
            
            // obtain camelStack associated with this dice
            std::vector<int> new_camels = board.get_camelstack(camel_position, rolled_camel);
            
            int roll = ceil(R::runif(0, 1) * 3);
            int new_loc = board.move_camels(new_camels, camel_position + roll);
            
            // Update position of camels
            for (auto it = new_camels.begin(); it < new_camels.end(); ++it) {
                camel_positions[(*it)] = new_loc;
            }
            
            race_won = new_loc > 15;
            if (race_won) {
                break;
            }
        }
        
        if (leg_num == 1) {
            int max_tile =*std::max_element(camel_positions.begin(), camel_positions.end());
            int leg_winner = board.get_camel_from_stack(max_tile); 
            results(leg_winner, 0)++;
        }
            
    }
    
    int max_tile =*std::max_element(camel_positions.begin(), camel_positions.end());
    int winner = board.get_camel_from_stack(max_tile); 
    results(winner, 1)++;
    int min_tile =*std::min_element(camel_positions.begin(), camel_positions.end());
    int loser = board.get_camel_from_stack(min_tile, false); 
    results(loser, 2)++;
    return;
}

//' Estimate camel win probabilities for Camel Up via simulation.
//' 
//' Estimates outcome probabilities for the boardgame Camel Up for a given board 
//' state using Monte Carlo Simulation.
//' 
//' @param boardstate A \code{16 x 7} integer matrix representing the current board state.
//' The rows correspond to the tiles, with the columns representing each of the 5 camels
//' and the two types of traps. Empty tiles are denoted by 0, while non-empty tiles have a positive
//' integer. Camel stacks are indicated by ascending numbers, with a value of 1 indicating a camel on
//' the bottom of the stack, 2 meaning the next highest camel and so on. Traps always have a value of 1.
//' The order of the 7 columns are as follows:
//' \enumerate{
//'     \item{Blue camel}
//'     \item{Green camel}
//'     \item{Orange camel}
//'     \item{Yellow camel}
//'     \item{White camel}
//'     \item{Forward trap}
//'     \item{Backward trap}
//' }
//' @param unrolled_dice An integer vector representing the dice that haven't been rolled yet this leg.
//' Integers map to camels in alphabetical order, i.e.:
//' \enumerate{
//'     \item{Blue camel}
//'     \item{Green camel}
//'     \item{Orange camel}
//'     \item{Yellow camel}
//'     \item{White camel}
//' }
//' 
//' @param num_sims The number of Monte Carlo simulations to run. Defaults to 1000.
//' @return A \code{5 x 3} matrix of floats representing the probabilities of 3 outcomes 
//' for the 5 camels. The 3 columns represent the following outcomes in order:
//' 
//' \enumerate{
//'     \item{Leg winner}
//'     \item{Overall winner}
//'     \item{Overall loser }
//' }
//' 
//' The five rows represent the camels in the same alphabetical order as in the \code{boardstate} and
//' \code{unrolled_dice} parameters.
//' 
//' @examples
//' library(camelsolve)
//' 2 camels on both tile 1 and 2 with last one on 3. One backwards trap on tile 4.
//' gamestate <- matrix(FALSE, nrow=16, ncol=7)
//' gamestate[1, 1] <- 1  # Blue camel on tile 1, bottom of stack
//' gamestate[2, 2] <- 1  # Green camel on tile 2, bottom of stack
//' gamestate[1, 3] <- 2  # Orange camel on tile 1, on top of blue camel
//' amestate[2, 4] <- 2   # Yellow camel on tile 2, on top of green camel
//' gamestate[3, 5] <- 1  # White camel on tile 3, alone
//' 
//' gamestate[4, 7] <- 1  # Backwards trap on tile 4
//' 
//' # Only the second dice has been rolled, i.e. the green dice
//' dice <- c(1, 3, 4, 5)
//' 
//' camelsolve(gamestate, dice)
//' 
// [[Rcpp::export]]
NumericMatrix camelsolve(IntegerMatrix boardstate, IntegerVector unrolled_dice, int num_sims=1000) {
    
    IntegerVector dice_0index = unrolled_dice - 1;
    IntegerMatrix win_numbers(5, 3);
    
    for (int i = 0; i < num_sims; i++) {
        simulate_round(boardstate, dice_0index, win_numbers);
    }
    
    NumericMatrix win_proportion(win_numbers);
    return win_proportion / num_sims;
}
