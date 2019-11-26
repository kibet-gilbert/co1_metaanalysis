################################################################################
# pairwiseDiffMatrix function  -  (matrix)
#
# Author: Heidi Lischer
# Date: 10.2008
################################################################################


pairwiseDiffMatrix <- function(xmlText, Labels=NULL, timeAttr,outfile=outfiles){
  # convert string data (half matrix) to a numeric matrix ----------------------
  # split string---------
  tagData2 <- as.character(xmlText)

  tagData3 <- strsplit(tagData2, "\n")
  tagMatrix <- as.matrix(as.data.frame(tagData3))
  tagMatrix <- subset(tagMatrix, tagMatrix[,1] != "") #trim empty lines
  tagMatrix <- tagMatrix[2:nrow(tagMatrix)]
  tagMatrix  <- gsub(" + ", " ", tagMatrix)   # trim white space
  Data <- strsplit(tagMatrix, " ")

  Row <- length(Data)

  #to numeric matrix ---------------------
  Matrix <- as.matrix(as.data.frame(Data[1]))
  Matrix <- subset(Matrix, Matrix[,1] != "")
  Matrix <- Matrix[2:nrow(Matrix),]
  numericList <- as.numeric(Matrix)
  numericMatrix <- t(as.matrix(numericList))

  if(Row >= 2){
      for(n in 2:(Row)){
        nextrow <- as.matrix(as.data.frame(Data[n]))
        nextrow <- subset(nextrow, nextrow[,1] != "")
        nextrow <- nextrow[2:nrow(nextrow),]
        numericList <- as.numeric(nextrow)
        numericMatrix <- rbind(numericMatrix, t(as.matrix(numericList)))
      }
  }
  
  
  nCol <- ncol(numericMatrix)
  

      # get underMatrix ------------------------------------------------------------
      underMatrix <- matrix(NA, ncol=nCol, nrow=1)
  if(nCol >= 2){
      for(i in 2:nCol){
        nextrow <- numericMatrix[i,1:(i-1)]
        nextrow <- cbind(t(nextrow), matrix(NA, ncol=nCol-(i-1), nrow=1))

        underMatrix <- rbind(underMatrix, nextrow)
      }
  }

      # get upperMatrix ------------------------------------------------------------
      upperMatrix <- matrix(NA, ncol=1, nrow=1)
      if(nCol >= 2){
      upperMatrix <- cbind(upperMatrix, t(numericMatrix[1,2:nCol]))

      if(nCol > 2){
        for (i in 3:nCol){
          nextrow <- matrix(NA, ncol=(i-1), nrow=1)
          nextrow <- cbind(nextrow, t(numericMatrix[(i-1), i:nCol]))

          upperMatrix <- rbind(upperMatrix, nextrow)
        }
      }
      upperMatrix <- rbind(upperMatrix, matrix(NA, ncol=nCol, nrow=1))
    }


  # get diagonalMatrix ---------------------------------------------------------
  diagonalMatrix <- numericMatrix[1,1]
  diagonalMatrix <- cbind(diagonalMatrix, matrix(NA, ncol=nCol-1, nrow=1))

  if(nCol >= 2){
      for(i in 2:nCol){
        nextrow <- matrix(NA, ncol=(i-1), nrow=1)
        nextrow <- cbind(nextrow, t(numericMatrix[i,i]))
        nextrow <- cbind(nextrow, matrix(NA, ncol=ncol(numericMatrix)-i, nrow=1))

        diagonalMatrix <- rbind(diagonalMatrix, nextrow)
      }
  }



  # graphic --------------------------------------------------------------------
  a <- ncol(numericMatrix)
  b <- nrow(numericMatrix)

  x <- c(1:a)
  y <- c(1:b)

  # define colors
  colorRampBlue <- colorRampPalette(c("white", "steelblue1", "blue3"))
  colorRampGreen <- colorRampPalette(c("white", "green3", "darkgreen"))
  colorRampOrange <- colorRampPalette(c("white", "orange", "orangered2"))
  

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


      underMatrix <- rotate270.matrix(underMatrix)
      upperMatrix <- rotate270.matrix(upperMatrix)
      diagonalMatrix <- rotate270.matrix(diagonalMatrix)


  # draw graphic --------------------------------------------
  outfileGraphic <- paste(outfile, "pairwiseDiffMatrix ", timeAttr, ".png",
                            sep="")  
  
  #save graphic
  png(outfileGraphic, width=1300, height=1300, res=144)

    # devide plot region in 4 parts
    def.par <- par(no.readonly = TRUE) # save default, for resetting...
      layout(rbind(c(1,2), c(1,3), c(1,4)),
             heights=rbind(c(2,1), c(2,1), c(2,1)),
             respect=rbind(c(0,1), c(0,0), c(0,0)))

    # draw matrixe plots --------------------
      op <- par(mar=c(7.6, 6.8, 8.6, 0.6))
        image(x, y, underMatrix, col=colorRampBlue(64), xlab="", ylab="",
               axes = FALSE)
        image(x,y, upperMatrix, col=colorRampGreen(64), xlab="", ylab="",
               axes = FALSE, add=TRUE)
        image(x,y, diagonalMatrix, col=colorRampOrange(64), xlab="", ylab="",
               axes = FALSE, add=TRUE)
           box()
           mtext(text="Average number of pairwise differences",
                   line=4.5, cex=1.2, font=2)

            #add labels
            if(is.null(Labels)){
              axis(1, at = c(1:a))
              axis(2, at = c(1:b), labels=c(b:1))
              mtext(text="Population", side=1, line=3)
              mtext(text="Population", side=2, line=3)
            }else{
              axis(1, at = c(1:a), labels=Labels[1:ncol(Labels)], cex.axis=1.1,
                     las=2)
              axis(2, at = c(1:b), labels=Labels[ncol(Labels):1], cex.axis=1.1,
                     las=2)
            }
      par(op)


    # draw legends----------------------------
    # upper legend --------
      op2 <- par(mar=c(0, 1.2, 4.2, 6.2))
        if(nCol >= 2){
            # define parameters
            Min <- min(upperMatrix, na.rm=TRUE)
            Max <- max(upperMatrix, na.rm=TRUE)
            binwidth <- (Max - Min) / 64
            y <- seq(Min + binwidth/2, Max - binwidth/2, by = binwidth)
            z <- matrix(y, nrow = 1, ncol = length(y))

            image(1, y, z, col = colorRampGreen(64), axes=FALSE)

                # adjust axis if only one value exists
                if(Min == Max){
                  axis(side=4, las = 2, at=Min, labels=round(Min, 2), cex.axis=1.1)
                } else {
                  axis(side=4, las = 2, cex.axis=1.1)
                }

                mtext(text="between populations", side=4, line=4, cex=1)
                box()
        }
      par(op2)


    # diagonal legend --------
      op3 <- par(mar=c(1.3, 1.2, 1.9, 6.2))
      
        # define parameters
        Min <- min(diagonalMatrix, na.rm=TRUE)
        Max <- max(diagonalMatrix, na.rm=TRUE)
        binwidth <- (Max - Min) / 64
        y <- seq(Min + binwidth/2, Max - binwidth/2, by = binwidth)
        z <- matrix(y, nrow = 1, ncol = length(y))
       
        image(1, y, z, col = colorRampOrange(64), axes=FALSE)
        
            # adjust axis if only one value exists
            if(Min == Max){         
              axis(side=4, las = 2, at=Min, labels=round(Min, 2), cex.axis=1.1)
            } else {
              axis(side=4, las = 2, cex.axis=1.1)
            }
             
           mtext(text="within populations", side=4, line=4, cex=1)
           box()
      par(op3)


    # under legend --------
      op4 <- par(mar=c(3.6, 1.2, 0.6, 6.2))
        if(nCol >= 2){
            # define parameters
            Min <- min(underMatrix, na.rm=TRUE)
            Max <- max(underMatrix, na.rm=TRUE)
            binwidth <- (Max - Min) / 64
            y <- seq(Min + binwidth/2, Max - binwidth/2, by = binwidth)
            z <- matrix(y, nrow = 1, ncol = length(y))

            image(1, y, z, col = colorRampBlue(64), axes=FALSE)

                # adjust axis if only one value exists
                if(Min == Max){
                  axis(side=4, las = 2, at=Min, labels=round(Min, 2), cex.axis=1.1)
                } else {
                  axis(side=4, las = 2, cex.axis=1.1)
                }

               mtext(text="Nei's distance (d)", side=4, line=4, cex=1)
               box()
        }
      par(op4)


    par(def.par)  #reset to default
    
  dev.off()

}