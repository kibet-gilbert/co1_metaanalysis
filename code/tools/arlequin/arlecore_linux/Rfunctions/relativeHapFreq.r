################################################################################
# relativeHapFreq function -  (lines)
#
# Author: Heidi Lischer
# Date: 10.2008
################################################################################


relativeHapFreq <- function(xmlText, timeAttr, outfile=outfiles) {
  # convert string data (table with names) to a numeric matrix -----------------
  # split string----
  tagData2 <- as.character(xmlText)

  tagData3 <- strsplit(tagData2, "\n")
  tagMatrix <- as.matrix(as.data.frame(tagData3))
  tagMatrix <- gsub(" + ", "\t", tagMatrix)   # trim white space
  tagMatrix <- subset(tagMatrix, tagMatrix[,1] != "") #trim empty lines

  # pop names ------------
  Names <- tagMatrix[1]
  Names <- strsplit(Names, "\t")
  Names <- as.matrix(as.data.frame(Names))
  Names <- subset(Names, Names[,1] != "")
  Names <- Names[2:nrow(Names)]


  #data-------------------------------------
  tagMatrix <- tagMatrix[4:nrow(tagMatrix)]
  Data <- strsplit(tagMatrix, "\t")

  Row <- length(Data)
  
  # to numeric matrix----  
  i <- 1
  Matrix <- as.matrix(as.data.frame(Data[i]))
  Matrix <- subset(Matrix, Matrix[,1] != "") 
  
  # jump over lines without data
  while(length(Matrix) == 1){
    i <- i + 1
    Matrix <- as.matrix(as.data.frame(Data[i]))  
    Matrix <- subset(Matrix, Matrix[,1] != "") 
  }
  
  HapNames <- Matrix[1,]
  Matrix <- Matrix[2:nrow(Matrix),]
  Matrix <- as.numeric(Matrix)
  numericMatrix <- t(as.matrix(Matrix))

  if(i <= Row){
    for(n in (i+1):(Row)){
      nextrow <- as.matrix(as.data.frame(Data[n]))
  
      # jump over empty lines and lines without data
      if((nextrow[1,1] != "") & (length(nextrow) != 1)){
        nextrowHapNames <- nextrow[1,]
        nextrow <- nextrow[2:nrow(nextrow),]
        nextrow <- as.numeric(nextrow)
        
        numericMatrix <- rbind(numericMatrix, t(as.matrix(nextrow)))
        HapNames <- rbind(HapNames, nextrowHapNames)
      }
    }
  }

  
  # draw graphic ---------------------------------------------------------------
  #get number of plots
  nGraphics <- ncol(numericMatrix)
  nRowGraphic <- ceiling(sqrt(nGraphics))

  #define size of the labels
  labSize <- 1
  if(nRowGraphic != 1){
    if(nRowGraphic == 2){
      labSize <- 0.85
    }
    else{
      labSize <- 0.75
    }
  }

  outfileGraphic <- paste(outfile, "relativeHapFreq ", timeAttr, ".png", sep="")

  #save graphic
  png(outfileGraphic, width=1300, height=1300, res=144)

    op <- par(oma=c(0,0,2,0), mfrow=c(nRowGraphic, nRowGraphic))

      #draw graphic for each pop---------------
      Nrow <- nrow(numericMatrix)
      max_x <- max(numericMatrix)

      for(i in 1:ncol(numericMatrix)){

        if((nRowGraphic == 1 && Nrow < 88) || (nRowGraphic == 2 && Nrow < 48) ||
            (nRowGraphic == 3 && Nrow < 36) || (nRowGraphic == 4 && Nrow < 30) ||
            (nRowGraphic >= 5 && Nrow < 16)){

            op1 <- par(mar=c(3, 3.5, 2, 1), mgp=c(3, 0, 0))

            barplot(numericMatrix[,i] ,width=0.8, space=0.2, main=Names[[i]],
                     col="blue3", pch=21, xlab="", ylim=c(0, max_x + (max_x/100*5)),
                     names.arg=HapNames, cex.axis=0.83, cex.names=0.83,
                     axes=FALSE)

            axis(side=2, mgp=c(0,0.85,0))
            mtext("Relative haplotype frequency", side=2 , line=2, cex=labSize)
            mtext("Haplotype", side=1, line=1.4, cex=labSize)

            box()
        } else {
            op1 <- par(mar=c(3, 3.5, 2, 1), mgp=c(3, 0.9, 0))

            plot(numericMatrix[,i], main=Names[[i]], col="blue3", pch=21,
                 xlab="", ylab="", ylim=c(0, max_x + (max_x/100*5)), ps=0.8,
                 axes=FALSE)

            axis(side=2)
            axis(side=1, at=c(1:Nrow), labels=HapNames, mgp=c(3, 0.4, 0))
            mtext("Relative haplotype frequency", side=2 , line=2, cex=labSize)
            mtext("Haplotype", side=1, line=1.6, cex=labSize)

            box()
        }
        par(op1)   # reset par settings
      }
       title("Haplotype frequencies in populations", outer=TRUE, cex.main=1.5)

    par(op)   # reset par settings

  dev.off()
}