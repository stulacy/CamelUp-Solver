library(shiny)
library(shinyjs)
library(DiagrammeR)

shinyUI(fluidPage(
  shinyjs::useShinyjs(),
  titlePanel("CamelUp Solver"),
  
  sidebarLayout(
    sidebarPanel(
        h3("Specify current game state"),
        p("Specify what camels and traps are currently lying on which tiles. Probabilities can be
           only be run once all 5 camels have been placed onto the board."),
        uiOutput("selecttile"),
        uiOutput("selectoccupant"),
        uiOutput("addoccupant"),
        br(),
        uiOutput("rolleddice"),
        hr(),
        actionButton("run", "Calculate probabilities")
    ),
    
    mainPanel(
       h3("Game board"),
       grVizOutput("boardstate"),
       hr(),
       uiOutput("probsheader"),
       DT::dataTableOutput("probs")
    )
  )
))
