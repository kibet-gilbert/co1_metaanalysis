################################################################################
# interHaplotypeDistMatrix function  - (matrix)
#
# Author: Heidi Lischer
# Date: 10.2008
################################################################################


interHaplotypeDistMatrix <- function(xmlText, Labels, timeAttr, popAttr,
                                       outfile=outfiles){
  # convert string data (half out of a full matrix) to a numeric matrix --------
  # split string -------
  tagData2 <- as.character(xmlText)

  tagData3 <- strsplit(tagData2, "\n")
  tagMatrix <- as.matrix(as.data.frame(tagData3))
  tagMatrix <- gsub(" + ", " ", tagMatrix)   # trim white space
  tagMatrix <- subset(tagMatrix, tagMatrix[,1] != "") #trim empty lines
  tagMatrix <- tagMatrix[2:(nrow(tagMatrix))]
  Data <- strsplit(tagMatrix, " ")
  
  Row <- length(Data)

  # to numeric matrix ----------
  Matrix <- matrix(c(0, rep(NA, Row-1)), ncol=1, nrow=Row)
  numericList <- as.numeric(Matrix)
  numericMatrix <- t(as.matrix(numericList))

  for(n in 2:(Row)){
    nextrow <- as.matrix(as.data.frame(Data[[n]][3:(n+1)]))
    nextrow <- rbind(nextrow, matrix(c(0, rep(NA, Row-n)), ncol=1, 
                      nrow=(Row-(n-1))))
    numericList <- as.numeric(nextrow)
    numericMatrix <- rbind(numericMatrix, t(as.matrix(numericList)))
  }  
  
  
  
  # graphic---------------------------------------------------------------------
  # Mirror matrix (left-right)
  mirror.matrix <- function(x) {
    xx <- as.data.frame(x);
    xx <- rev(xx);
    xx <- as.matrix(xx);
    xx;
  }

  # Rotate matrix 270 clockworks
  rotate270.matrix <- function(x) {
    mirror.matrix(t(x))
  }

  DistanceMatrix <- rotate270.matrix(numericMatrix)

  nCol <- ncol(DistanceMatrix)
  nRow <- nrow(DistanceMatrix)

  # draw matrix plot ----------------
   ColorRamp <- colorRampPalette(c("white", "steelblue1", "blue3"))
 
   outfileGraphic <- paste(outfile, "interHapDistMatrix ", popAttr, "_",
                           timeAttr, ".png", sep="")
   
   # save graphic  
   png(outfileGraphic, width=1300, height=1300, res=144)
   
      smallplot <- c(0.874, 0.9, 0.18, 0.83)
      bigplot <- c(0.13, 0.85, 0.14, 0.87)
      
      old.par <- par(no.readonly = TRUE)
      
        #draw legend --------------------------------
        par(plt = smallplot)
      
        # get legend values
        Min <- min(DistanceMatrix, na.rm=TRUE)
        Max <- max(DistanceMatrix, na.rm=TRUE)
        binwidth <- (Max - Min) / 64
        y <- seq(Min + binwidth/2, Max - binwidth/2, by = binwidth)
        z <- matrix(y, nrow = 1, ncol = length(y))
      
        image(1, y, z, col = ColorRamp(64),xlab="", ylab="", axes=FALSE)
      
            # adjust axis if only one value exists
            if(Min == Max){
              axis(side=4, las = 2, cex.axis=0.8, at=Min, labels=round(Min, 2))
            } else {
              axis(side=4, las = 2, cex.axis=0.8)
            }
      
            box()
            mtext(text="Number of pairwise differences", side=4,line=2.2,font=2)
      
            
            
        #draw main graphic ------------------------------
        par(new = TRUE, plt = bigplot)
      
        image(c(1:nCol), c(1:nRow), DistanceMatrix, col=ColorRamp(64),
                main=paste("Inter-haplotypic distance matrix", popAttr,
                sep="\n"), xlab="Haplotype", ylab="Haplotype", axes=FALSE)

            cexAxis <- 0.8
            if(nCol > 56) {
                cexAxis <- 0.7
            }
                
            axis(1, at = c(1:nCol),labels=c(Labels[1:ncol(Labels)]),
                   cex.axis=cexAxis, las=2)
            axis(2, at = c(1:nRow), labels=c(Labels[ncol(Labels):1]),
                   cex.axis=cexAxis, las=2)
            box()
            
      par(old.par)  #reset graphic parameters
   
   dev.off()


   return(numericMatrix) #retrun matrix values
}