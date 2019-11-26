################################################################################
# expectedHapFreq function  - (lines)
#
# Author: Heidi Lischer
# Date: 10.2008
################################################################################


expectedHapFreq <- function(xmlText, obsHapFrequency, timeAttr, popAttr,
                             outfile=outfiles){

  # convert string data (table with names) to a numeric matrix -----------------
  # split string --------
  tagData2 <- as.character(xmlText)

  tagData3 <- strsplit(tagData2, "\n")
  tagMatrix <- as.matrix(as.data.frame(tagData3))
  tagMatrix <- gsub(" + ", " ", tagMatrix)   # trim white space
  tagMatrix <- subset(tagMatrix, tagMatrix[,1] != "") #trim empty lines
  tagMatrix <- tagMatrix[7:(nrow(tagMatrix)-1)]
  Data <- strsplit(tagMatrix, " ")

  Row <- length(Data)-1
  
  # to numeric matrix -----------
  Matrix <- as.matrix(as.data.frame(Data[1]))
  Matrix <- subset(Matrix, Matrix[,1] != "")
  Matrix <- as.numeric(Matrix)
  numericMatrix <- t(as.matrix(Matrix))

  for(n in 2:(Row)){
    nextrow <- as.matrix(as.data.frame(Data[n]))
    nextrow <- subset(nextrow, nextrow[,1] != "")
    nextrow <- as.numeric(nextrow)
    numericMatrix <- rbind(numericMatrix, t(as.matrix(nextrow)))
  }
  numericMatrix <- numericMatrix[,2:3]
  


  # graphic --------------------------------------------------------------------
  expHapFrequency <- numericMatrix[,1]
  sdExp <- numericMatrix[,2]
  sdExpUp <- expHapFrequency + sdExp
  sdExpLo <- expHapFrequency - sdExp
  
  yMin <- min(obsHapFrequency, expHapFrequency, sdExpLo)
  yMax <- max(obsHapFrequency, expHapFrequency, sdExpUp)

  
  outfileGraphic <- paste(outfile, "obsExpHapFreq ", popAttr, "_", timeAttr,
                           ".png", sep="")
  
  #save graphic
  png(outfileGraphic, width=1300, height=1300, res=144)
  
    plot(obsHapFrequency, type="l", xlab="Allele", ylab="Relative frequency",
          ylim=c(yMin, yMax), main=paste("Haplotype Frequencies", popAttr,
          sep="\n"), lwd=2, xaxt="n")
          
      axis(1, 1:length(expHapFrequency), cex.axis=0.9)
      lines(expHapFrequency, col="red3")
      lines(sdExpUp, lty=2, col="red3")
      lines(sdExpLo, lty=2, col="red3")
      legend("topright", c("observed", "expected", "expected +/- 1 sd"),
             lty=c(1,1,2), lwd=c(2,1,1), bty="n", col=c("black", "red3", "red3"),
             cex=0.9)


  dev.off()

}