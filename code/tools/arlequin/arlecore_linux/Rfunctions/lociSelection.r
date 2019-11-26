################################################################################
# lociSelection function
#
# Author: Heidi Lischer
# Date: 8.2009
################################################################################

lociSelection <- function(pValList, FStatCiList, timeAttr, outfile, fStat){
    
  if(fStat == "FST"){
      fst <- 3
      fstP <- 4
  } else {
      fst <- 5
      fstP <- 6
  }
  
  MaxHet <- max(pValList[,2], na.rm=T)
  MaxF <- max(pValList[,fst], na.rm=T)
  if(max(FStatCiList[,8], na.rm=T) > MaxF){
    MaxF <- max(FStatCiList[,8], na.rm=T)
  }
  
  MinHet <- 0
  MinF <- 0
  
  if(min(pValList[,2], na.rm=T) < 0){
    MinHet <- min(pValList[,2], na.rm=T)
  }
  if(min(pValList[,fst], na.rm=T) < 0){
    MinF <- min(pValList[,fst], na.rm=T)
    
    if(min(FStatCiList[,2], na.rm=T) < MinF){
      MinF <- min(FStatCiList[,2], na.rm=T)
    }
  }            

  FST_h0.05 <- pValList[pValList[,fstP] > 0.05, c(2, fst)]
  FST_0.05 <- pValList[pValList[,fstP] <= 0.05 & pValList[,fstP] > 0.01, c(2, fst)]
  FST_0.01 <- pValList[pValList[,fstP] <= 0.01, c(2, fst)]
  lociNames_0.01 <- as.character(pValList[pValList[,fstP] <= 0.01, 1])
  

  # graphic---------------------------------------------------------------------
  outfileGraphic <- paste(outfile, "lociSelection_", fStat," ", timeAttr,".png",
                           sep="") 
                             
  #save graphic
   png(outfileGraphic, width = 1300, height = 1300, res=144)

      plot(FST_h0.05, cex=0.8, xlab="Heterozygosity", ylab="", main="",
            xlim=c(MinHet, MaxHet), ylim=c(MinF, MaxF), cex.axis=0.9)

      if(fStat == "FST"){
        mtext(text=expression(bold(Detection~of~loci~under~selection~from~genome~scans~based~on~F[ST])),
                 side=3, line=1.5, cex=1.2)
        mtext(text=expression(F[ST]), side=2, line=2.5, cex=1.2)
      } else {
        mtext(text=expression(bold(Detection~of~loci~under~selection~from~genome~scans~based~on~F[CT])),
                 side=3, line=1.5, cex=1.2)
        mtext(text=expression(F[CT]), side=2, line=2.5, cex=1.2)
      }

      if(length(FST_0.05) != 0){
         #FST_0.05_Upper <- FST_0.05[FST_0.05[,2] > 0.04,]
         #FST_0.05_Lower <- FST_0.05[FST_0.05[,2] <= 0.04,]
         #points(FST_0.05_Lower, pch=1, cex=0.8, col="black")
         #points(FST_0.05_Upper, pch=16, cex=0.8, col="blue")
         
         points(FST_0.05, pch=16, cex=0.8, col="blue")
      }
      if(length(FST_0.01) != 0){
         #FST_0.01_Upper <- FST_0.01[FST_0.01[,2] > 0.04,]
         #FST_0.01_Lower <- FST_0.01[FST_0.01[,2] <= 0.04,]
         #points(FST_0.01_Lower, pch=1, cex=0.8, col="black")
         #points(FST_0.01_Upper, pch=16, cex=0.8, col="red3")
         
        points(FST_0.01, pch=16, cex=0.8, col="red3")
      }
      
      lines(FStatCiList[,1], FStatCiList[,5])
      
      lines(FStatCiList[,1], FStatCiList[,4], lty=5)
      lines(FStatCiList[,1], FStatCiList[,6], lty=5)
      
      lines(FStatCiList[,1], FStatCiList[,3], lty=2, col="blue")
      lines(FStatCiList[,1], FStatCiList[,7], lty=2, col="blue")
      
      lines(FStatCiList[,1], FStatCiList[,2], lty=3, col="red3")
      lines(FStatCiList[,1], FStatCiList[,8], lty=3, col="red3")
    
      legend("topleft",
             c("", "", "", "50% quantile", "10% quantile", "5% quantile", "1% quantile"),
             lty=c(1, 1, 1, 1, 5, 2, 3),
             col=c("white", "white", "white", "black", "black", "blue", "red3"),
             cex=0.8,  bty="n")
            
      legend("topleft", legend=c("p > 5%", "p <= 5%", "p <= 1%"),
             pch=c(1, 16, 16), col=c("black", "blue", "red3"), cex=0.8, bty="n")
 
  dev.off()
  
  
  
  
  
  # graphic---------------------------------------------------------------------
  outfileGraphic <- paste(outfile, "lociSelection_Names_", fStat," ", timeAttr,".png",
                           sep="")


  #save graphic
  png(outfileGraphic, width = 1300, height = 1300, res=144)


      plot(FST_h0.05, cex=0.8, xlab="Heterozygosity", ylab="", main="",
            xlim=c(MinHet, MaxHet), ylim=c(MinF, MaxF), cex.axis=0.9)

      if(fStat == "FST"){
        mtext(text=expression(bold(Detection~of~loci~under~selection~from~genome~scans~based~on~F[ST])),
                 side=3, line=1.5, cex=1.2)
        mtext(text=expression(F[ST]), side=2, line=2.5, cex=1.2)
      } else {
        mtext(text=expression(bold(Detection~of~loci~under~selection~from~genome~scans~based~on~F[CT])),
                 side=3, line=1.5, cex=1.2)
        mtext(text=expression(F[CT]), side=2, line=2.5, cex=1.2)
      }

      if(length(FST_0.05) != 0){
         points(FST_0.05, pch=16, cex=0.8, col="blue")
      }
      if(length(FST_0.01) != 0){
        points(FST_0.01, pch=16, cex=0.7, col="red3")
        text(FST_0.01, lociNames_0.01, adj = c(-0.3,0.3), cex=0.75, col="red3")
      }

      lines(FStatCiList[,1], FStatCiList[,5])

      lines(FStatCiList[,1], FStatCiList[,4], lty=5)
      lines(FStatCiList[,1], FStatCiList[,6], lty=5)

      lines(FStatCiList[,1], FStatCiList[,3], lty=2, col="blue")
      lines(FStatCiList[,1], FStatCiList[,7], lty=2, col="blue")

      lines(FStatCiList[,1], FStatCiList[,2], lty=3, col="red3")
      lines(FStatCiList[,1], FStatCiList[,8], lty=3, col="red3")

      legend("topleft",
             c("", "", "", "50% quantile", "10% quantile", "5% quantile", "1% quantile"),
             lty=c(1, 1, 1, 1, 5, 2, 3),
             col=c("white", "white", "white", "black", "black", "blue", "red3"),
             cex=0.8,  bty="n")

      legend("topleft", legend=c("p > 5%", "p <= 5%", "p <= 1%"),
             pch=c(1, 16, 16), col=c("black", "blue", "red3"), cex=0.8, bty="n")

  dev.off()
}