#include <Rcpp.h>
#include <stack>
#include <math.h>
#include "board.h"
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
NumericMatrix solve(IntegerMatrix boardstate, IntegerVector dice) {
    
    Rcpp::Rcout << "Game state:\n" << boardstate << "\n";
    Rcpp::Rcout << "Dice rolled: " << dice << "\n";
    
    // Shuffle dice
    IntegerVector shuff_dice;
    shuff_dice = shuffle(dice);
    Rcpp::Rcout << "Dice stack: " << shuff_dice << "\n";
    std::vector<int> dice_stack;
    dice_stack = Rcpp::as<std::vector<int> >(shuff_dice);
    
    // TODO Get stack working as stack!
    
    // Setup board with camel stacks 
    Board board(boardstate);
    int rolled_dice;
    int roll;
    
    // Main loop
    // while dice_remaining:
    while (!dice_stack.empty()) {
        // draw a dice
        rolled_dice = dice_stack.back();
        // cross off dice from vector
        dice_stack.pop_back();
        Rcpp::Rcout << "Rolled dice: " << rolled_dice << "\n";
        //   roll it
        roll = ceil(R::runif(0, 1) * 3);
        Rcpp::Rcout << "Dice roll: " << roll << "\n";
        
        //   obtain camelStack associated with this dice (board.getCamel())
        //   runevent(camelStack, int, boardstate) (how to code this?)
    }
    
    //
    // runevent():
    //   - backwards_trap = false
    //   - find camelstack that contains this dice
    //   - find index where camel is located
    //   - if not at bottom of stack, then form new stack
    //   - if forward trap on curr_position + dice:
    //       - curr_position = curr_position + 1
    //   - if backward trap on curr_position + dice:
    //       - curr_position = curr_position - 1
    //       - backwards_trap = true
    //   - add_to_tile(this_stack, tile, backwards_trap)
    
    
    // add_to_tile(stack, tile, backwards_trap):
    //   - If no stack already on tile, just add to main data structure
    //   - Else:
    //       - If backwards_trap:
    //           - Set new_stack below existing stack
    //       - Else:
    //           - Set new_stack on top of existing stack
    //   - Add combined stack to tile
     
    
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
