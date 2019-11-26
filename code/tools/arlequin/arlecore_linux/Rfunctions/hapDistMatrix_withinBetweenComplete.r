################################################################################
# hapDistMatrix_withinBetweenComplete function  - (matrix)
#
# Author: Heidi Lischer
# Date: 10.2008
################################################################################


hapDistMatrix_withinBetweenComplete <- function(hapDistMatrixLabels,
                                hapDistMatrix, absHapMatrix, timeAttr, outfile){
                                
 #get pop names, hap names and absHapFrequeny matrix
 popNames <- absHapMatrix[1, 2:ncol(absHapMatrix)]
 hapNames <- absHapMatrix[2:nrow(absHapMatrix), 1]
 hapNames <- gsub(" ", "", hapNames)   # trim white space
 absHapFreq <- absHapMatrix[2:nrow(absHapMatrix), 2:ncol(absHapMatrix)]
 
 
 #get list of all haplotypes in all populations --------------------------------
 popSizeList <- list()
 hapList <- c(NULL)
 s <- 1
 for ( i in 1:ncol(absHapFreq)){
    popSize <- 0
    for(n in 1:length(absHapFreq[,i])){
      absFreq <- as.numeric(absHapFreq[n, i])
      if(absFreq != 0){
        for(t in 1: absFreq){
          hapList[s] <- hapNames[n]
          s <- s + 1
          popSize <- popSize + 1
        }
      }
    }
    popSizeList[[i]] <- popSize 
 }


  # get whole DistanceMatrix (upper and under) ---------------------------------
  dimnames(hapDistMatrix) <- list(hapDistMatrixLabels, hapDistMatrixLabels)
  
  wholeDistanceMatrix <- hapDistMatrix
  for(x in 1:ncol(wholeDistanceMatrix)){
   twholeDistanceMatrix <- t(wholeDistanceMatrix)
   wholeDistanceMatrix[x,(x:ncol(wholeDistanceMatrix))]<-twholeDistanceMatrix[x,
                                                  (x:ncol(wholeDistanceMatrix))]
  }
  
  
  #get complete distance matrix (all populations included) ---------------------
  Length <- length(hapList)
  
  #get all rows
  distanceMatrix <- wholeDistanceMatrix[hapList[1],]   
  for(i in 2:Length){
    distanceMatrix <- rbind(distanceMatrix, wholeDistanceMatrix[hapList[i],])
  }
  
  #get all columns and include only half matrix
  distanceMatrixComplete <- distanceMatrix[, hapList[1]]
  for(i in 2:Length){
    nextCol <- as.matrix(distanceMatrix[i:Length,hapList[i]])
    distanceMatrixComplete <- cbind(distanceMatrixComplete, rbind( matrix(NA,
                                                 ncol=1, nrow=(i-1)), nextCol))
  }
  

  
  #draw graphic ----------------------------------------------------------------
  # Mirror matrix (left-right)----
  mirror.matrix <- function(x) {
    xx <- as.data.frame(x);
    xx <- rev(xx);
    xx <- as.matrix(xx);
    xx;
  }

  # Rotate matrix 270 clockworks----
  rotate270.matrix <- function(x) {
    mirror.matrix(t(x))
  }

  distanceMatrixComplete <- rotate270.matrix(distanceMatrixComplete)


  # draw matrix -------------------------------
   ColorRamp <- colorRampPalette(c("white", "steelblue1", "blue3"))

   nCol <- ncol(distanceMatrixComplete)
   nRow <- nrow(distanceMatrixComplete)


   outfileGraphic <- paste(outfile, "hapDistMatrix_withinBetweenComplete ", 
                             timeAttr, ".png", sep="")

   #save graphic
   png(outfileGraphic, width=1300, height=1300, res=144) 
   
      smallplot <- c(0.874, 0.9, 0.18, 0.83)
      bigplot <- c(0.13, 0.85, 0.14, 0.87)
      
      old.par <- par(no.readonly = TRUE)
      
      
        #draw legend -----------------------------------
        par(plt = smallplot)
      
        # get legend values
        Min <- min(distanceMatrixComplete, na.rm=TRUE)
        Max <- max(distanceMatrixComplete, na.rm=TRUE)
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
            mtext(text="Number of pairwise differences", side=4,line=2.2,font=2)
      
      
            
        #draw main graphic -----------------------------
        par(new = TRUE, plt = bigplot)
      
        image(c(1:nCol), c(1:nRow), distanceMatrixComplete, col=ColorRamp(64),
               main="Molecular distances within and between populations",
               ylab="", xlab="", axes=FALSE)
               
            box()
            
            # add lines which seperate the populations
            nRowShort <- 0
            for(i in 1: (length(popSizeList)-1)){
              nRowShort <- nRowShort + popSizeList[[i]]
              nRowLong <- nrow(distanceMatrixComplete) - nRowShort
              
              lines(c(0, nRowShort + 0.5),c(nRowLong + 0.5, nRowLong + 0.5), lty=2)
              lines(c(nRowShort + 0.5, nRowShort + 0.5),c(0, nRowLong + 0.5), lty=2)
            }
            
            lines(c(0, nrow(distanceMatrixComplete)),
                  c(nrow(distanceMatrixComplete), 0), lty=2)
            
            # add population indexes
            nRow <- 0
            Legend <- c(NULL)
            for(i in 1: length(popNames)){
              nRow2 <- popSizeList[[i]]
              rowText <- nRow + (nRow2 /2)
              nRow <- nRow + nRow2
              
              mtext(side = 1, at = rowText + 0.5, line = 0.5, text =popNames[i],
                     cex=0.8, las = 2)
              mtext(side = 2, at = nrow(distanceMatrixComplete) - rowText + 0.5,
                     line = 0.7, text = popNames[i], cex=0.8, las=2, , las = 2)
     
            }

        par(old.par)  #reset graphic parameters

   dev.off()

}