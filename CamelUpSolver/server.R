library(shiny)
library(solver)

NUM_SQUARES <- 14
CAMEL_COLOURS <- c('blue', 'white', 'orange', 'green', 'yellow')
CAMELS <- paste('camel', CAMEL_COLOURS, sep='_')
TRAPS <- paste('trap', c('forward', 'backward'), sep='_')

is_camel <- function(x) {
    strsplit(x, "_")[[1]][1] == 'camel'
}

is_trap <- function(x) {
    strsplit(x, "_")[[1]][1] == 'camel'
}
        
shinyServer(function(input, output) {
    
    ########################### LOGIC ####################################
    # Setup tiles with nothing on
    # Rather than using proper data structures, I'll just use strings
    # to represent tile occupants, in the form <type>_<value>, i.e.:
    # 'trap_forward', 'trap_backward', 'camel_blue', 'camel_orange' etc...
    tiles <- reactiveValues()
    for (i in seq(NUM_SQUARES)) {
        tiles[[paste(i)]] <- NULL
    }
    
    observeEvent(input$addoccupant, {
        if (input$addoccupant < 1) return()
        
        tiles[[paste(input$tile)]] <- c(tiles[[paste(input$tile)]], input$tileoccupant)
    })
    
    probs <- eventReactive(input$run, {
        # TODO Run simulation
        withProgress({
            Sys.sleep(5)
            res <- solve()
        }, message="Running simulation")
        res
        
        # TODO Create binary matrix with camels and traps to indicate game state
        
        # Separate vector of booleans for dice rolled. Will use ints to refer to camels in C++
        
        # Pass these into cpp function and receive a 3x5 matrix with rows:
            # Leg winning probabilities
            # Overall winning probabilitiy
            # Overall losing probability
            # And columns given by camel
    })
    
    ########################### SIDE PANEL ####################################
    
    
    output$selecttile <- renderUI({
        if (is.null(tiles)) return()
        
        tiles_l <- reactiveValuesToList(tiles)
        tiles_ord <- tiles_l[paste(seq(NUM_SQUARES))]
        # Can add values to any tile that doesn't have a trap on it already
        non_trap_tiles <- tiles_ord[sapply(tiles_ord, function(x) {
            is.null(x) || is_trap(x)
            }
        )]
        
        selectInput("tile", "Add item to tile...", choices=names(non_trap_tiles))
    })
    
    output$addoccupant <- renderUI({
        in_tile <- input$tile
        if (is.null(in_tile)) return()
        
        possible_items <- unplaced_camels()
        
        # If don't have camel on square then can add trap too 
        if (is.null(tiles[[paste(in_tile)]])) {
            possible_items <- c(possible_items, TRAPS)
        }
        
        if (length(possible_items) == 0) {
            p("Unable to place any further items on this tile.")
        } else {
            item_list <- list(
                selectInput("tileoccupant", "Occupant", possible_items),
                actionButton("addoccupant", "Add")
            )
            do.call(tagList, item_list)
        }
    })
    
    output$rolleddice <- renderUI({
        checkboxGroupInput("rolleddice", "Select dices that have already been rolled", 
                           choices=CAMEL_COLOURS)
    })
    
    # Returns names of camels that have been placed.
    # TODO Should this be a function or reactive?
    placed_camels <- reactive({
        tile_occupants <- unlist(reactiveValuesToList(tiles))
        placed_camels <- tile_occupants[sapply(tile_occupants, is_camel)]
    })
    
    # Returns names of camels that haven't yet been placed.
    # TODO Should this be a function or reactive?
    unplaced_camels <- reactive({
        tile_occupants <- unlist(reactiveValuesToList(tiles))
        placed_camels <- tile_occupants[sapply(tile_occupants, is_camel)]
        setdiff(CAMELS, placed_camels)
    })
  
    output$runbutton <- renderUI({
        if (length(unplaced_camels()) > 0) return()
        
        actionButton("run", "Calculate probabilities")
    })
    
    ########################### MAIN PANEL ####################################
    output$testout <- renderPrint({
        tiles_l <- reactiveValuesToList(tiles)
        tiles_l[paste(seq(NUM_SQUARES))]
    })
    
    output$boardstate <- renderUI({
        p("Some clever way of using DiagrammeR to draw the board out with the camels")
    })
    
    output$probs <- renderUI({
        probs()
    })
})
