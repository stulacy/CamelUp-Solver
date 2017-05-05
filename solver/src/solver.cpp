#include <Rcpp.h>
#include "board.h"
using namespace Rcpp;

// [[Rcpp::export]]
NumericMatrix solve(IntegerMatrix boardstate, LogicalVector dice) {
    
    Rcpp::Rcout << "Game state:\n" << boardstate << "\n";
    Rcpp::Rcout << "Dice rolled: " << dice << "\n";
    
    Board board(boardstate);
    
    // TODO Think about how to organise C++ code in terms of what objects are required (CamelStack, Trap)
    // TODO Make this more OO
    // One function at simulation level:
    //
    // Has global variable of board? Or pass around like DES?
    // 
    // while dice_remaining:
    //   draw a dice
    //   roll it
    //   runevent(boardstate) (how to code this?)
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
