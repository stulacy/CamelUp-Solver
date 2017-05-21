library(shiny)
library(solver)
library(DiagrammeR)
library(parallel)
library(data.table)

NUM_SQUARES <- 16
MAX_POSSIBLE_SQUARE <- 18
CAMEL_COLOURS <- c('blue', 'green', 'orange', 'white', 'yellow')
CAMELS <- paste(CAMEL_COLOURS, 'camel', sep='_')
TRAPS <- paste(c('forward', 'backward'), 'trap', sep='_')
N_INNER_SIMS <- 1000
N_OUTER_SIMS <- 1000
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
        dice_rolled <- which(!CAMEL_COLOURS %in% input$rolleddice)-1
        
        withProgress(message="Running simulation", value=0, max=N_OUTER_SIMS, {
            res <- lapply(1:N_OUTER_SIMS, function(i) {
                incProgress(1, message = i)
                solve(gamestate, dice_rolled, N_INNER_SIMS)
            })
            comb_res <- rbindlist(lapply(res, as.data.frame), idcol = "sim")
            setnames(comb_res, c("sim", "Leg", "Overall win", "Overall loss"))
            comb_res[, Camel := rep(seq_along(CAMELS), N_OUTER_SIMS)]
            res_long <- melt(comb_res, id.vars=c("sim", "Camel"))
            summary <- res_long[, list(mean=mean(value), 
                                       upper=quantile(value, 0.95),
                                       lower=quantile(value, 0.05)),
                                by=list(Camel, variable)]
            summary[, Camel := CAMEL_COLOURS[Camel]]
            summary[, output := sprintf("%.2f (%.2f - %.2f)", mean*100, lower*100, upper*100)]
            summary[, c('mean', 'upper', 'lower') := NULL]
            dcast(summary, Camel ~ variable, value.var="output")
        })
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
        addbut <- actionButton("addoccupant", "Add")
        rstbut <- actionButton("resetbutton", "Reset")
        item_list <- list(addbut, rstbut)
        do.call(tagList, item_list)
    })
    
    observe({
        occ <- input$tileoccupant
        no_occupants <- all(sapply(reactiveValuesToList(tiles), is.null))
        if (is.null(occ) || occ == '') {
            shinyjs::disable("addoccupant")
        } else {
            shinyjs::enable("addoccupant")
        }
        if (no_occupants) {
            shinyjs::disable("resetbutton")
        } else {
            shinyjs::enable("resetbutton")
        }
    })
    
    output$rolleddice <- renderUI({
        checkboxGroupInput("rolleddice", "Select dice that have already been rolled", 
                           choices=CAMEL_COLOURS)
    })
    
    observe({
        if (length(unplaced_camels()) > 0) {
            shinyjs::disable("run")
        } else {
            shinyjs::enable("run")
        }
    })
    output$runbutton <- renderUI({
        if (length(unplaced_camels()) > 0) return()
        
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
    
    output$probs <- DT::renderDataTable({
        res <- probs()
        colnames(res) <- c("Camel", "Leg Win %", "Overall Win %", "Overall Loss %")
        res
        }, options=list(dom='t'
                        ),
        rownames=FALSE)
})
