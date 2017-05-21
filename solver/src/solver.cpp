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
        for (int j=0; j < 16; ++j) {
            if (boardstate(j, i) > 0) {
                camel_positions.push_back(j);
                continue;
            }
        }
    }
    
    //for (auto it = camel_positions.begin(); it != camel_positions.end(); ++it) {
    //    Rcpp::Rcout << "Camel is on tile: " << (*it) << "\n";
    //}
    
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
        
        //Rcpp::Rcout << "\nLEG: " << leg_num << "\n";
        //Rcpp::Rcout << "Dice stack: " << shuff_dice << "\n";
        
        while (!dice_stack.empty()) {
            //Rcpp::Rcout << "\n";
            int rolled_camel = dice_stack.back();
            dice_stack.pop_back();
            //Rcpp::Rcout << "Dice colour : " << rolled_camel << "\n";
            // get camel tile id from camel_positions
            int camel_position = camel_positions[rolled_camel];
            //Rcpp::Rcout << "This camel is currently on position: " << camel_position << "\n";
            
            // obtain camelStack associated with this dice
            std::vector<int> new_camels = board.get_camelstack(camel_position, rolled_camel);
            
            int roll = ceil(R::runif(0, 1) * 3);
            //Rcpp::Rcout << "Dice roll: " << roll << "\n";
            int new_loc = board.move_camels(new_camels, camel_position + roll);
            
            // Update position of camels
            for (auto it = new_camels.begin(); it < new_camels.end(); ++it) {
                //Rcpp::Rcout << "Updating location for camel " << (*it) << " to " << new_loc << "\n";
                camel_positions[(*it)] = new_loc;
            }
            
            race_won = new_loc >= 16;
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
    
    //Rcpp::Rcout << "Race has been won!\n";
    //Rcpp::Rcout << results;
    return;
}

// [[Rcpp::export]]
NumericMatrix solve(IntegerMatrix boardstate, IntegerVector input_dice, int num_sims) {
    
    IntegerMatrix results(5, 3);
    for (int i = 0; i < num_sims; i++) {
        simulate_round(boardstate, input_dice, results);
    }
    
    NumericMatrix foo(results);
    return foo / num_sims;
}
