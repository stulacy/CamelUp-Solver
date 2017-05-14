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

// [[Rcpp::export]]
NumericMatrix solve(IntegerMatrix boardstate, IntegerVector input_dice) {
    
    Rcpp::Rcout << "Game state:\n" << boardstate << "\n";
    Rcpp::Rcout << "Dice rolled: " << input_dice << "\n";
    
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
    
    for (auto it = camel_positions.begin(); it != camel_positions.end(); ++it) {
        Rcpp::Rcout << "Camel is on tile: " << (*it) << "\n";
    }
    
    Board board(boardstate);
    int rolled_camel, roll, camel_position, new_loc;
    std::vector<int> new_camels;
    CamelStack* this_camel; 
    bool race_won = false;
    int leg_num = 0;
    IntegerVector shuff_dice;
    std::vector<int> dice_stack;
    
    while (! race_won) {
        
        // Shuffle dice
        if (leg_num == 0) {
            shuff_dice = shuffle(input_dice);
        } else {
            shuff_dice = shuffle(Rcpp::IntegerVector::create(0, 1, 2, 3, 4));
        }
        Rcpp::Rcout << "Dice stack: " << shuff_dice << "\n";
        dice_stack = Rcpp::as<std::vector<int> >(shuff_dice);
        
        leg_num++;
        
        Rcpp::Rcout << "Leg number: " << leg_num << "\n";
        
        while (!dice_stack.empty()) {
            Rcpp::Rcout << "\n";
            rolled_camel = dice_stack.back();
            dice_stack.pop_back();
            Rcpp::Rcout << "Dice colour : " << rolled_camel << "\n";
            // get camel tile id from camel_positions
            camel_position = camel_positions[rolled_camel];
            Rcpp::Rcout << "This camel is currently on position: " << camel_position << "\n";
            
            // obtain camelStack associated with this dice (board.getCamel())
            this_camel = board.getCamel(camel_position, rolled_camel);
            Rcpp::Rcout << "Retrieved camel stack which lives in: " << this_camel << "\n";
            
            roll = ceil(R::runif(0, 1) * 3);
            Rcpp::Rcout << "Dice roll: " << roll << "\n";
            new_loc = board.move_camels(this_camel, camel_position + roll);
            
            // Update position of camels
            new_camels = this_camel->getCamels();
            for (auto it = new_camels.begin(); it < new_camels.end(); ++it) {
                Rcpp::Rcout << "Updating location for camel " << (*it) << " to " << new_loc << "\n";
                camel_positions[(*it)] = new_loc;
            }
            
            race_won = new_loc >= 16;
            Rcpp::Rcout << "Race won: " << race_won << "\n";
        }
    }
    
    Rcpp::Rcout << "Race has been won!\n";
    
    
    // for each camel calculate:
    //   - whether won leg
    //   - whether won race
    //   - whether race was won and in last place
    
    NumericMatrix out(3, 5);
    out(0, 2) = 5.2;
    out(1, 3) = 4.3;
    out(2, 0) = 3.6;

    return out;
}
