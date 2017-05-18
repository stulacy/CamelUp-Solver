library(shiny)
library(solver)
library(DiagrammeR)

NUM_SQUARES <- 16
MAX_POSSIBLE_SQUARE <- 18
CAMEL_COLOURS <- c('blue', 'white', 'orange', 'green', 'yellow')
CAMELS <- paste(CAMEL_COLOURS, 'camel', sep='_')
TRAPS <- paste(c('forward', 'backward'), 'trap', sep='_')
N_SIMS <- 1000
TILE_COLOUR <- 'khaki1'

# TODO Have stripes running vertically

is_camel <- function(x) {
    !is.null(x) && strsplit(x, "_")[[1]][2] == 'camel'
}

is_trap <- function(x) {
    !is.null(x) && strsplit(x, "_")[[1]][2] == 'trap'
}

neaten_str <- function(raw) {
    # Neatens a string of form 'descriptor_type' to 'Descriptor type'
    splt <- strsplit(raw, '_')[[1]]
    paste(paste0(toupper(substring(splt[1], 1, 1)), 
                 substring(splt[1], 2)), splt[2])
}

shinyServer(function(input, output, session) {
    
    ########################### LOGIC ####################################
    # Setup tiles with nothing on
    # Rather than using proper data structures, I'll just use strings
    # to represent tile occupants, in the form <type>_<value>, i.e.:
    # 'trap_forward', 'trap_backward', 'camel_blue', 'camel_orange' etc...
    tiles <- reactiveValues()
    graph <- reactiveValues(nodes=create_node_df(16, label=seq(16),
                                                pos=c("2, 4!",
                                                      "3, 4!",
                                                      "4, 4!",
                                                      "4, 3!",
                                                      "4, 2!",
                                                      "4, 1!",
                                                      "4, 0!",
                                                      "3, 0!",
                                                      "2, 0!",
                                                      "1, 0!",
                                                      "0, 0!",
                                                      "0, 1!",
                                                      "0, 2!",
                                                      "0, 3!",
                                                      "0, 4!",
                                                      "1, 4!"),
                                                fontname="FreeSans",
                                                fontsize="20",
                                                fontcolor="black",
                                                fixedsize="true",
                                                shape="square",
                                                width="1",
                                                height="1",
                                                style="striped",
                                                fillcolor=TILE_COLOUR))
    
    for (i in seq(NUM_SQUARES)) {
        tiles[[paste(i)]] <- NULL
    }
    
    unplaced_camels <- reactive({
        tile_occupants <- unlist(reactiveValuesToList(tiles))
        placed_camels <- tile_occupants[sapply(tile_occupants, is_camel)]
        setdiff(CAMELS, placed_camels)
    })
    
    free_tiles <- reactive({
        tile_occupants <- reactiveValuesToList(tiles)
        placed_traps <- names(tile_occupants)[!sapply(tile_occupants, is_trap)]
    })
  
    observeEvent(input$addoccupant, {
        if (input$addoccupant < 1) return()
        
        in_tile <- input$tile
        occupant <- input$tileoccupant
        
        # Update display
        if (is_trap(occupant)) {
            if (strsplit(occupant, "_")[[1]][1] == 'forward') {
                colour <- 'lawngreen'
                lab <- '+'
            } else {
                colour <- 'red' 
                lab <- '-'
            }
            graph$nodes[graph$nodes$id == in_tile, 'label'] <- lab
        }
        
        if (is_camel(occupant)) {
            colour <- gsub("_camel", "", occupant)
            # If tile already has camel then just append new camel
            if (!is.null(tiles[[in_tile]])) {
                colour <- paste(graph$nodes[graph$nodes$id == in_tile, 'fillcolor'], 
                                colour, sep=':')
            }
        }
        graph$nodes[graph$nodes$id == in_tile, 'fillcolor'] <- colour
        
        tiles[[in_tile]] <- c(tiles[[in_tile]], occupant)
    })
    
    observeEvent(input$resetbutton, {
        for (i in seq(NUM_SQUARES)) {
            tiles[[paste(i)]] <- NULL
        }
        
        graph$nodes$label <- seq(NUM_SQUARES)
        graph$nodes$fillcolor <- TILE_COLOUR
    })
    
    probs <- eventReactive(input$run, {
        if (input$run < 1) return()
        
        # Game state is represented by binary matrix with 
        # tiles on rows and camels and trips as cols
        cols <- c(CAMELS, TRAPS)
        gamestate <- matrix(FALSE, nrow=MAX_POSSIBLE_SQUARE, ncol=length(cols))
        
        for (i in seq(MAX_POSSIBLE_SQUARE)) {
            tile_occupants <- tiles[[paste(i)]]
            col_indices <- sapply(tile_occupants, function(occ) which(cols == occ))
            if (length(col_indices) > 0)
                gamestate[i, col_indices] <- seq_along(col_indices)
        } 
        
        withProgress({
            res <- solve(gamestate, which(!CAMEL_COLOURS %in% input$rolleddice)-1, N_SIMS)
            res <- res / N_SIMS * 100
        }, message="Running simulation")
    })
    
    ########################### SIDE PANEL ####################################
    output$selecttile <- renderUI({
        selectInput("tile", "Add item to tile...", choices=seq(NUM_SQUARES))
    })
    
    # When change tile, update list to display appropriate options
    observe({
        in_tile <- input$tile
        if (is.null(reactiveValuesToList(tiles))) return()
        if (is.null(in_tile)) return()
        
        # Else, obtain list of possible values and update tile occupants accordingly
        # If don't have camel on square then can add trap too 
        if (is_trap(tiles[[in_tile]])) {
            freetiles <- free_tiles()
            updateSelectInput(session, "tile", selected=min(as.numeric(freetiles)), 
                              choices=sort(as.numeric(freetiles)))
        } else {
            possible_items <- unplaced_camels()
            if (is.null(tiles[[in_tile]])) {
                if (!is_trap(tiles[[paste(as.numeric(in_tile)+1)]])) {
                    if (!(as.numeric(in_tile) > 1 && is_trap(tiles[[paste(as.numeric(in_tile)-1)]]))) {
                        possible_items <- c(possible_items, TRAPS)
                    }
                }
            }
            updateSelectInput(session, "tileoccupant", 
                              choices=setNames(possible_items, sapply(possible_items, neaten_str)))
                
        }
    })
    
    output$selectoccupant <- renderUI({
        options <- c(CAMELS, TRAPS)
        selectInput("tileoccupant", "Occupant", setNames(options,
                                                         sapply(options, neaten_str)))
    })
    
    output$addoccupant <- renderUI({
        occ <- input$tileoccupant
        no_occupants <- all(sapply(reactiveValuesToList(tiles), is.null))
        addbut <- if (is.null(occ) || occ == '') NULL else actionButton("addoccupant", "Add")
        rstbut <- if (no_occupants) NULL else actionButton("resetbutton", "Reset")
        
        item_list <- list(addbut, rstbut)
        do.call(tagList, item_list)
    })
    
    output$rolleddice <- renderUI({
        checkboxGroupInput("rolleddice", "Select dice that have already been rolled", 
                           choices=CAMEL_COLOURS)
    })
    
    output$runbutton <- renderUI({
        if (length(unplaced_camels()) > 0) return()
        
        actionButton("run", "Calculate probabilities")
    })
    
    ########################### MAIN PANEL ####################################
    output$boardstate <- renderGrViz({
        graph <- create_graph(graph$nodes)
        render_graph(graph)
    })
    
    output$probsheader <- renderUI({
        if (is.null(probs()))
            return ()
        
        h3("End game probabilities")
    })
    
    output$probs <- renderTable({
        res <- probs()
        res <- data.frame(res)
        res <- cbind(CAMEL_COLOURS, res)
        colnames(res) <- c("Camel", "Leg Win %", "Overall Win %", "Overall Loss %")
        res
    })
})
