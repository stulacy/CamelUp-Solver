library(shiny)

shinyUI(fluidPage(
  
  titlePanel("CamelUp Solver"),
  
  sidebarLayout(
    sidebarPanel(
        h3("Specify current game state"),
        p("Specify what camels and traps are currently lying on which tiles. Probabilities can be
           only be run once all 5 camels have been placed onto the board."),
        uiOutput("selecttile"),
        uiOutput("selectoccupant"),
        uiOutput("addoccupant"),
        uiOutput("rolleddice"),
        hr(),
        uiOutput("runbutton")
    ),
    
    mainPanel(
       h3("Outputs"),
       p("This is where the probabilities for various outputs will be displayed"),
       verbatimTextOutput("testout"),
       uiOutput("boardstate"),
       tableOutput("probs")
    )
  )
))
