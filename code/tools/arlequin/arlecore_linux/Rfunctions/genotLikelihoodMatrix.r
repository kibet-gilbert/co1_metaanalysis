################################################################################
# genotLikelihoodMatrix function  - (points)
#
# Author: Heidi Lischer
# Date: 10.2008
################################################################################


genotLikelihoodMatrix <- function(xmlText, popList, timeAttr, popAttr,
                                     outfile=outfiles) {
  # convert string data (full matrix) to a numeric matrix ----------------------
  # split string -------
  tagData2 <- as.character(xmlText)
  
  tagData3 <- strsplit(tagData2, "\n")
  tagMatrix <- as.matrix(as.data.frame(tagData3))
  tagMatrix <- gsub(" + ", "\t", tagMatrix)   # trim white space
  tagMatrix <- subset(tagMatrix, tagMatrix[,1] != "") #trim empty lines
  tagMatrix <- tagMatrix[4:(nrow(tagMatrix))]
  Data <- strsplit(tagMatrix, "\t")
  
  Row <- length(Data)
  
  # to numeric matrix --------
  Matrix <- as.matrix(as.data.frame(Data[1]))
  Matrix <- subset(Matrix, Matrix[,1] != "")
  Matrix <- Matrix[2:(nrow(Matrix))]
  Matrix <- as.numeric(Matrix)
  numericMatrix <- t(as.matrix(Matrix))
  
  for(n in 2:(Row)){
    nextrow <- as.matrix(as.data.frame(Data[n]))
    nextrow <- subset(nextrow, nextrow[,1] != "")
    nextrow <- nextrow[2:(nrow(nextrow))]
    nextrow <- as.numeric(nextrow)
    numericMatrix <- rbind(numericMatrix, t(as.matrix(nextrow)))
  }

  
  #get index of the pop of interest in the popList
  for(i in 1:length(popList)){
    popList[[i]] <- gsub("/|:", "-", popList[[i]])
    if(popList[[i]] == popAttr){
      pop <- i
      popName <- popList[[i]]
    }
  }
  
  #get the values for the pop of interest    
  pop1Values <- numericMatrix[, pop]
  
  #get the matrix without the pop of interst  
  popMatrix <- numericMatrix[, -pop]
  popMatrix <- as.matrix(popMatrix)
  
  #get the popList without the pop of interest
  popList <- popList[- pop]
  
  
  
  # draw graphic ---------------------------------------------------------------
  #get number of plots
  nGraphics <- ncol(popMatrix)
  nRowGraphic <- ceiling(sqrt(nGraphics))
  
  #define size of the labels
  labSize <- 1
  if(nRowGraphic != 1){
    if(nRowGraphic == 2){
      labSize <- 0.83
    }
    else{
      labSize <- 0.66
    }
  }
  
  
  outfileGraphic <- paste(outfile, "genotLikelihoodMatrix ", popAttr, "_",
                           timeAttr, ".png", sep="")  
  
  #save graphic
  png(outfileGraphic, width=1300, height=1300, res=144)

    op <- par(oma=c(0,0,2,0), mfrow=c(nRowGraphic, nRowGraphic))

      #draw graphic for each pop---------------
      for(i in 1:ncol(popMatrix)){ 
       
        min_x <- min(pop1Values, popMatrix[,i])
        max_x <- max(pop1Values, popMatrix[,i])
        
        op1 <- par(mar=c(3, 3.3, 2, 1.2), mgp=c(3, 0.9, 0))

            plot(popMatrix[,i], pop1Values, main=popList[[i]], col="blue3", pch=21,
                 xlab="", ylab="", xlim=c(min_x, max_x), ylim=c(min_x, max_x), ps=0.8,
                 axes=TRUE)

            lines(c(min_x, max_x), c(min_x ,max_x))
           # axis(side=2)
           # axis(side=1)
          #  mtext("Allele size range", side=2 , line=2, cex=labSize)
           # mtext("Locus", side=1, line=1.6, cex=labSize)

            box()
            par(op1)   # reset par settings
      }
      title(paste("Population assignment test -  ", popName, "  vs. other populations")
            , outer=TRUE, cex.main=1.5)

    par(op)   # reset par settings

  dev.off()

}
        
