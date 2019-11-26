################################################################################
# ancestralPopulationSize function  -  (matrix)
#
# Author: Heidi Lischer
# Date: 10.2008
################################################################################


ancestralPopulationSize <- function(xmlText, Labels=NULL, timeAttr,
                                     outfile=outfiles) {
  # convert string data (half matrix) to a numeric matrix ----------------------
  # split string ---------
  tagData2 <- as.character(xmlText)

  tagData3 <- strsplit(tagData2, "\n")
  tagMatrix <- as.matrix(as.data.frame(tagData3))
  tagMatrix  <- gsub("[*]", "", tagMatrix)
  tagMatrix <- subset(tagMatrix, tagMatrix[,1] != "") #trim empty lines
  tagMatrix <- tagMatrix[2:nrow(tagMatrix)]
  tagMatrix  <- gsub(" + ", " ", tagMatrix)   # trim white space
  Data <- strsplit(tagMatrix, " ")

  Row <- length(Data)

  # to numeric matrix --------------
  Matrix <- as.matrix(as.data.frame(Data[1]))
  Matrix <- subset(Matrix, Matrix[,1] != "")
  Matrix <- rbind(Matrix, matrix(NA, ncol=1, nrow=Row-1))
  Matrix <- Matrix[2:nrow(Matrix),]
  numericList <- as.numeric(Matrix)
  numericMatrix <- t(as.matrix(numericList))

  if(Row >= 2){
    for(n in 2:(Row)){
      nextrow <- as.matrix(as.data.frame(Data[n]))
      nextrow <- subset(nextrow, nextrow[,1] != "")
      nextrow <- rbind(nextrow, matrix(NA, ncol=1, nrow=(Row-n)))
      nextrow <- nextrow[2:nrow(nextrow),]
      numericList <- as.numeric(nextrow)
      numericMatrix <- rbind(numericMatrix, t(as.matrix(numericList)))
    }
  }



  # graphic --------------------------------------------------------------------
  ColorRamp <- colorRampPalette(c("white", "steelblue1", "blue3"))

  #----Mirror matrix (left-right)----
  mirror.matrix <- function(x) {
    xx <- as.data.frame(x);
    xx <- rev(xx);
    xx <- as.matrix(xx);
    xx;
  }

  #----Rotate matrix 270 clockworks----
  rotate270.matrix <- function(x) {
    mirror.matrix(t(x))
  }

  Matrix <- rotate270.matrix(numericMatrix)


  # draw matrix --------------------------
  outfileGraphic <- paste(outfile, "ancestralPopulationSize ", timeAttr,".png",
                            sep="")  
  
  # save graphic
  png(outfileGraphic, width=1300, height=1300, res=144)
  
    smallplot <- c(0.874, 0.9, 0.18, 0.83)
    bigplot <- c(0.13, 0.85, 0.14, 0.87)
    
    old.par <- par(no.readonly = TRUE)
    
    
      # draw legend -------------------------------------
      par(plt = smallplot)
    
      # get legend values
      Min <- min(Matrix, na.rm=TRUE)
      Max <- max(Matrix, na.rm=TRUE)
      binwidth <- (Max - Min) / 64
      y <- seq(Min + binwidth/2, Max - binwidth/2, by = binwidth)
      z <- matrix(y, nrow = 1, ncol = length(y))
    
      image(1, y, z, col = ColorRamp(64),xlab="", ylab="", axes=FALSE)
    
          #adjust axis if only one value exists
          if(Min == Max){
            axis(side=4, las = 2, cex.axis=0.8, at=Min, labels=round(Min, 2))
          } else {
            axis(side=4, las = 2, cex.axis=0.8)
          }
    
          box()
          mtext(text=expression(bold(relative~population~size~(theta[0]))),
                side=4, line=2.5, font=2, cex=1.1)
    
          
          
      # draw main graphic ---------------------------------
      a <- ncol(numericMatrix)
      b <- nrow(numericMatrix)
    
      x <- c(1:a)
      y <- c(1:b)

      par(new = TRUE, plt = bigplot)
    
      image(x,y,Matrix, col=ColorRamp(64), main="Ancestral population sizes",
                xlab="", ylab="", axes=FALSE)
          box()
          
          #add labels
          if(is.null(Labels)){
            axis(1, at = c(1:a))
            axis(2, at = c(1:b), labels=c(b:1))
            mtext(side = 1, at =(a/2), line = 2.5, text = "Population", cex=1,
                   font=2)
            mtext(side = 2, at =(b/2), line = 2.7, text = "Population", cex=1,
                   font=2)
          }else{
            axis(1, at = c(1:a), labels=Labels[1:ncol(Labels)], cex.axis=0.75,
                   las=2)
            axis(2, at = c(1:b), labels=Labels[ncol(Labels):1], cex.axis=0.75,
                   las=2)
          }
          
          
    par(old.par)  #reset graphic parameters 

  dev.off()

}