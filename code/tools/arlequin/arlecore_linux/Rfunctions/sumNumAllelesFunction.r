################################################################################
# sumNumAllelesFunction function  -  (lines)
#
# Author: Heidi Lischer
# Date: 11.2008
################################################################################


sumNumAllelesFunction <- function(xmlText,timeAttr, outfile=outfiles){ 
  # convert string data (table with names) to a numeric matrix -----------------
  # split string --------
  tagData2 <- as.character(xmlText)

  tagData3 <- strsplit(tagData2, "\n")
  tagMatrix <- as.matrix(as.data.frame(tagData3))
  tagMatrix <- gsub(" + ", "\t", tagMatrix)   # trim white space
  tagMatrix <- subset(tagMatrix, tagMatrix[,1] != "") #trim empty lines
  
  # names ------------------
  Names <- tagMatrix[2]
  Names <- strsplit(Names, "\t")
  Names <- as.matrix(as.data.frame(Names))
  Names <- Names[2:(nrow(Names)-2)]

  # data ----------------------------------
  tagMatrix <- tagMatrix[4:(nrow(tagMatrix)-4)]
  Data <- strsplit(tagMatrix, "\t")

  Row <- length(Data)

  #to numeric matrix --------
  Matrix <- as.matrix(as.data.frame(Data[1]))
  Matrix <- subset(Matrix, Matrix[,1] != "")
  Matrix <- Matrix[1:(nrow(Matrix)-3)]
  Matrix <- as.numeric(Matrix)
  numericMatrix <- t(as.matrix(Matrix))

  if(Row >= 2){
    for(n in 2:(Row)){
      nextrow <- as.matrix(as.data.frame(Data[n]))
      nextrow <- subset(nextrow, nextrow[,1] != "")
      nextrow <- nextrow[1:(nrow(nextrow)-3)]
      nextrow <- as.numeric(nextrow)
      numericMatrix <- rbind(numericMatrix, t(as.matrix(nextrow)))
    }
  }


  # draw graphic ---------------------------------------------------------------
  #get number of plots
  nGraphics <- ncol(numericMatrix)-1
  nRowGraphic <- ceiling(sqrt(nGraphics))

  #define size of the labels
  labSize <- 1
  if(nRowGraphic != 1){
    if(nRowGraphic == 2){
      labSize <- 0.9
    }
    else{
      labSize <- 0.75
    }
  }

  outfileGraphic <- paste(outfile, "sumNumAlleles ", timeAttr, ".png", sep="")

  #save graphic
  png(outfileGraphic, width=1300, height=1300, res=144)

    op <- par(oma=c(0,0,2,0), mfrow=c(nRowGraphic, nRowGraphic))

      #draw graphic for each pop---------------
      Nrow <- nrow(numericMatrix)
      max_x <- max(numericMatrix[, 2:ncol(numericMatrix)])

      for(i in 2:ncol(numericMatrix)){

        if((nRowGraphic == 1 && Nrow < 88) || (nRowGraphic == 2 && Nrow < 48) ||
            (nRowGraphic == 3 && Nrow < 36) || (nRowGraphic == 4 && Nrow < 30) ||
            (nRowGraphic >= 5 && Nrow < 16)){

            op1 <- par(mar=c(3, 3.5, 2, 1), mgp=c(3, 0, 0))

            barplot(numericMatrix[,i] ,width=0.8, space=0.2, main=Names[[i-1]],
                     col="blue3", pch=21, xlab="", ylim=c(0, max_x + (max_x/100*5)),
                     names.arg=numericMatrix[,1], cex.axis=0.83, cex.names=0.83,
                     axes=FALSE)

            axis(side=2, mgp=c(0,0.85,0))
            mtext("Number of alleles", side=2 , line=2, cex=labSize)
            mtext("Locus", side=1, line=1.4, cex=labSize)

            box()
        } else {
            op1 <- par(mar=c(3, 3.5, 2, 1), mgp=c(3, 0.9, 0))

            plot(numericMatrix[,i], main=Names[[i-1]], col="blue3", pch=21,
                 xlab="", ylab="", ylim=c(0, max_x + (max_x/100*5)), ps=0.8,
                 axes=FALSE)

            axis(side=2)
            axis(side=1, at=c(1:Nrow), labels=numericMatrix[,1], mgp=c(3, 0.4, 0))
            mtext("Number of alleles", side=2 , line=2, cex=labSize)
            mtext("Locus", side=1, line=1.6, cex=labSize)

            box()
        }
        par(op1)   # reset par settings
      }
       title("Number of alleles at different loci", outer=TRUE, cex.main=1.5)

    par(op)   # reset par settings

  dev.off()

}