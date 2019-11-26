################################################################################
# mismatch_Graphic function  - (lines)
#
# Author: Heidi Lischer
# Date: 10.2008
################################################################################


mismatch_Graphic <- function(mismatchMatrix, mismatch_0.01,
                                mismatch_0.05, mismatch_0.1,
                                spatial=TRUE, timeAttr, popAttr, outfile){

  # get max --------------------------------------------------------------------
  if(!is.null(mismatch_0.01)){
    Max <-  max(mismatch_0.01[, 5], mismatch_0.01[, 4],
                 mismatch_0.01[, 2])
  } else {
    if(!is.null(mismatch_0.05)){
        Max <-  max(mismatch_0.01[, 5], mismatch_0.05[, 4],
                    mismatch_0.05[, 2])
    } else {
        if(!is.null(mismatch_0.1)){
            Max <-  max(mismatch_0.01[, 5], mismatch_0.1[, 4],
                         mismatch_0.1[, 2])
        } else {
            Max <-  max(mismatchMatrix[, 4], mismatchMatrix[, 2])
        }
    }
  }

  # define if spatial or demographic
  if(spatial){
    x <- "spatial"
  }else{
    x <- "demographic"
  }

  outfileGraphic <- paste(outfile, x, "Mismatch ", popAttr, "_",
                           timeAttr, ".png", sep="")
  
  legendText <- NULL
  legendCol <- NULL
  legendLine <- NULL
  legendLineWidth <- NULL
  
   #save graphic
   png(outfileGraphic, width=1300, height=1300, res=144)
   
    plot(mismatchMatrix[, 1], mismatchMatrix[, 2], type="l", lwd=2,
         ylim=c(0, Max), xlab="Pairwise differences", ylab="Number of pairs",
         main=paste("Mismatch distribution (",x," expansion)\n",popAttr,sep=""))

      lines(mismatchMatrix[,1], mismatchMatrix[, 4], col="black")
      
      legendText[1] <- "observed"
      legendText[2] <- "expected"
      
      legendCol[1] <- "black"
      legendCol[2] <- "black"
      
      legendLine[1] <- 1
      legendLine[2] <- 1
      
      legendLineWidth[1] <- 2
      legendLineWidth[2] <- 1
      
      j <- 3
      if(!is.null(mismatch_0.01)){
        lines(mismatch_0.01[,1], mismatch_0.01[, 3],lty=2, col="blue")
        lines(mismatch_0.01[,1], mismatch_0.01[, 4],lty=2, col="blue")
        
        legendText[j] <- "99% CI"
        legendCol[j] <- "blue"
        legendLine[j] <- 2
        legendLineWidth[j] <- 1
        
        j <- j + 1
      }
      
      if(!is.null(mismatch_0.05)){
        lines(mismatch_0.05[,1], mismatch_0.05[, 3],lty=2, col="red3")
        lines(mismatch_0.05[,1], mismatch_0.05[, 4],lty=2, col="red3")

        legendText[j] <- "95% CI"
        legendCol[j] <- "red3"
        legendLine[j] <- 2
        legendLineWidth[j] <- 1

        j <- j + 1
      }
      
      if(!is.null(mismatch_0.1)){
        lines(mismatch_0.1[,1], mismatch_0.1[, 3],lty=2, col="green3")
        lines(mismatch_0.1[,1], mismatch_0.1[, 4],lty=2, col="green3")

        legendText[j] <- "90% CI"
        legendCol[j] <- "green3"
        legendLine[j] <- 2
        legendLineWidth[j] <- 1

        j <- j + 1
      }

      #print legend
      legend("topright", legendText, col = legendCol, lty=legendLine,
             lwd=legendLineWidth, bty="n")
              
              
   dev.off()
}
