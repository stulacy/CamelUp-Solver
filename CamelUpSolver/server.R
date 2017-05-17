library(shiny)
library(solver)
library(DiagrammeR)

NUM_SQUARES <- 16
MAX_POSSIBLE_SQUARE <- 18
CAMEL_COLOURS <- c('blue', 'white', 'orange', 'green', 'yellow')
CAMELS <- paste(CAMEL_COLOURS, 'camel', sep='_')
TRAPS <- paste(c('forward', 'backward'), 'trap', sep='_')
N_SIMS <- 1000

# TODO display fonts more cleanly
# TODO handle white camel = beige background?
# TODO Don't resize whole diagram when change colour
# TODO Have stripes running vertically
# TODO More appropriate forward trap colour
# TODO Key with trap colour somewhere

is_camel <- function(x) {
    !is.null(x) && strsplit(x, "_")[[1]][2] == 'camel'
}

is_trap <- function(x) {
    !is.null(x) && strsplit(x, "_")[[1]][2] == 'trap'
}

raw_diagram <- "
digraph 'camelup'
{
    graph [pad='0.75', ranksep='0.25', nodesep='0.25'];
    node [fontname='FreeSans',fontsize=20,shape=square, width=2, height=2, style=striped, fillcolor=white, gradientangle=90];
    edge [style=invis];
    
    rankdir = 'TB';
    
    subgraph cluster_top {
        style=invis;
        {
            rank='same';
            Node_15 [label='@@15', fillcolor='@@31'];
            Node_16 [label='@@16', fillcolor='@@32'];
            Node_1 [label='@@1', fillcolor='@@17'];
            Node_2 [label='@@2', fillcolor='@@18'];
            Node_3 [label='@@3', fillcolor='@@19'];
            Node_15 -> Node_16 ;
            Node_16 -> Node_1 ;
            Node_1 -> Node_2 ;
            Node_2 -> Node_3 ;
        }
    }
    subgraph cluster_right {
        style=invis;
        Node_4 [label='@@4', fillcolor='@@20']; 
        Node_5 [label='@@5', fillcolor='@@21']; 
        Node_6 [label='@@6', fillcolor='@@22'];
        Node_3 -> Node_4 ;
        Node_4 -> Node_5 ;
        Node_5 -> Node_6 ;
    }
    subgraph cluster_bottom {
        style=invis;
        {
            rank='same';
            Node_11 [label='@@11', fillcolor='@@27'];
            Node_10 [label='@@10', fillcolor='@@26'];
            Node_9 [label='@@9', fillcolor='@@25'];
            Node_8 [label='@@8', fillcolor='@@24'];
            Node_7 [label='@@7', fillcolor='@@23'];
            
            Node_11 -> Node_10 [dir='back'];
            Node_10 -> Node_9 [dir='back'];
            Node_9 -> Node_8 [dir='back'];
            Node_8 -> Node_7 [dir='back'];
            Node_6 -> Node_7 ;
        }
    }
    subgraph cluster_left {
        style=invis;
        Node_12 [label='@@12', fillcolor='@@28'];
        Node_13 [label='@@13', fillcolor='@@29'];
        Node_14 [label='@@14', fillcolor='@@30'];
        
        Node_14 -> Node_13 [dir='back'];
        Node_13 -> Node_12 [dir='back'];
        Node_12 -> Node_11 [dir='back'];
        Node_15 -> Node_14 [dir='back'];
    }
    
}
"

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
        
        tiles[[paste(input$tile)]] <- c(tiles[[paste(input$tile)]], input$tileoccupant)
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
        if (is_trap(tiles[[paste(in_tile)]])) {
            freetiles <- free_tiles()
            updateSelectInput(session, "tile", selected=min(as.numeric(freetiles)), 
                              choices=sort(as.numeric(freetiles)))
        } else {
            possible_items <- unplaced_camels()
            if (is.null(tiles[[paste(in_tile)]])) {
                possible_items <- c(possible_items, TRAPS)
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
        if (is.null(input$tileoccupant) || input$tileoccupant == '') return()
        actionButton("addoccupant", "Add")
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
    output$testout <- renderPrint({
        tiles_l <- reactiveValuesToList(tiles)
        tiles_l[paste(seq(NUM_SQUARES))]
    })
    
    output$boardstate <- renderGrViz({
        trap_labels <- paste(sapply(1:NUM_SQUARES, function(t) {
            index <- paste0("[", t, "]: ") 
            contents <- if (!is.null(tiles[[paste(t)]])) {
                #paste0(' (', paste(tiles[[paste(t)]], collapse=' ,'), ')')
                # TODO Keep this in or just use colours?
                paste('')
            } else ""
            paste0(index, " '", t, contents, "'")
        }), collapse='\n')
        
        camel_labels <- paste(sapply(1:NUM_SQUARES, function(t) {
            index <- paste0("[", t+16, "]: ") 
            contents <- tiles[[paste(t)]]
            if (is.null(contents)) {
                contents_str <- 'white'
            } else {
                if (is_trap(contents)) {
                    if (strsplit(contents, "_")[[1]][1] == 'forward') {
                        contents_str <- 'lawngreen'
                    } else {
                        contents_str <- 'red' 
                    }
                } else {
                    # Interesting bit!
                    colours <- gsub("_camel", "", contents)
                    contents_str <- paste(colours, collapse=':')
                }
            }
            # TODO Debugging
            paste0(index, "'", contents_str, "'")
        }), collapse='\n')
        
        full <- paste(raw_diagram, trap_labels, camel_labels, sep='\n')

        grViz(full)
    })
    
    output$probs <- renderTable({
        res <- probs()
        res <- data.frame(res)
        res <- cbind(CAMEL_COLOURS, res)
        colnames(res) <- c("Camel", "Leg Win %", "Overall Win %", "Overall Loss %")
        res
    })
})
