library(randomForest)

dat<-read.csv("RF_datafile.csv", header=T)
#code probability cut-offs
for (i in 1:nrow(dat)) {
  
  if (dat$Epp[i] >=0.95) {
    dat$P0.95[i] <- "E"
  } else if (dat$Epp[i] <=0.05) {
    dat$P0.95[i] <- "B"
  } else 
    dat$P0.95[i] <- "NA"
  
  if (dat$Epp[i] >=0.9) {
    dat$P0.9[i] <- "E"
  } else if (dat$Epp[i] <=0.1) {
    dat$P0.9[i] <- "B"
  } else 
    dat$P0.9[i] <- "NA"
  
  if (dat$Epp[i] >=0.8) {
    dat$P0.8[i] <- "E"
  } else if (dat$Epp[i] <=0.2) {
    dat$P0.8[i] <- "B"
  } else 
    dat$P0.8[i] <- "NA" 
  
  if (dat$Epp[i] >=0.7) {
    dat$P0.7[i] <- "E"
  } else if (dat$Epp[i] <=0.3) {
    dat$P0.7[i] <- "B"
  } else 
    dat$P0.7[i] <- "NA" 
}

dat[dat=="-999"]<-NA

#beige
bat<-dat[,c(33,8:14)]
bat<-bat[!(bat$P0.9 =="NA"),]
bat<-na.omit(bat)
bat$P0.9<-as.factor(bat$P0.9)
table(bat$P0.9)

bat$Family<-as.factor(bat$Family)
bat$Zoogeographicprovince<-as.factor(bat$Zoogeographicprovince)

RF<-randomForest(P0.9 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF

#downsample
for (i in 1:100) {
  Btemp=bat[bat$P0.9=="B",]
  Esamp=bat[bat$P0.9=="E",]
  Bsamp<-Btemp[(sample(nrow(Btemp), size=17)),]
  
  bat_samp=rbind(Bsamp, Esamp)
  
  RF<-randomForest(P0.9 ~., data=bat_samp, importance=TRUE, ntree=1000, nperm=100)
  
  imp=RF$importance
  write.table(imp,file="RF_samp_imp_beige.csv", sep=",", append=T, col.names=!file.exists("RF_samp_imp_beige.csv"))
  
  err=RF$err.rate
  write.table(err, file="RF_samp_error_beige.csv", sep=",", row.names=FALSE, col.names=FALSE, append=T)
  
}

oob<-read.csv("RF_samp_error_beige.csv", header=FALSE)

mean(oob$V1)
mean(oob$V2)
mean(oob$V3)

#import RF_samp_imp_beige.csv
m<-aggregate(i[,4], list(i$var), mean)
write.csv(m, file="RF_mean_imp_beige.csv", quote=FALSE, row.names=FALSE)


#green+pink
bat<-dat[,c(33,15,16,19,20,21,23,24,25,27,28,29,31)]
bat<-bat[!(bat$P0.9 =="NA"),]
bat<-na.omit(bat)
bat$P0.9<-as.factor(bat$P0.9)
table(bat$P0.9)

bat$breeding<-as.factor(bat$breeding)
bat$dietaryniche<-as.factor(bat$dietaryniche)
bat$roosting<-as.factor(bat$roosting)

RF<-randomForest(P0.9 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF

#downsample
for (i in 1:100) {
  Btemp=bat[bat$P0.9=="B",]
  Esamp=bat[bat$P0.9=="E",]
  Bsamp<-Btemp[(sample(nrow(Btemp), size=7)),]
  
  bat_samp=rbind(Bsamp, Esamp)
  
  RF<-randomForest(P0.9 ~., data=bat_samp, importance=TRUE, ntree=1000, nperm=100)
  
  imp=RF$importance
  write.table(imp,file="RF_samp_imp_pinkgreen.csv", sep=",", append=T, col.names=!file.exists("RF_samp_imp_pinkgreen.csv"))
  
  err=RF$err.rate
  write.table(err, file="RF_samp_error_pinkgreen.csv", sep=",", row.names=FALSE, col.names=FALSE, append=T)
  
}

oob<-read.csv("RF_samp_error_pinkgreen.csv", header=FALSE)

mean(oob$V1)
mean(oob$V2)
mean(oob$V3)

#import RF_samp_imp_beige.csv
m<-aggregate(i[,4], list(i$var), mean)
write.csv(m, file="RF_mean_imp_beige.csv", quote=FALSE, row.names=FALSE)

#beige+pink
bat<-dat[,c(33,8:16,19,20,21,23,24,25,27,28,29,31)]
bat<-bat[!(bat$P0.9 =="NA"),]
bat<-na.omit(bat)
bat$P0.9<-as.factor(bat$P0.9)
table(bat$P0.9)

bat$breeding<-as.factor(bat$breeding)
bat$dietaryniche<-as.factor(bat$dietaryniche)
bat$roosting<-as.factor(bat$roosting)
bat$Family<-as.factor(bat$Family)
bat$Zoogeographicprovince<-as.factor(bat$Zoogeographicprovince)

RF<-randomForest(P0.9 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF

#downsample
for (i in 1:100) {
  Btemp=bat[bat$P0.9=="B",]
  Esamp=bat[bat$P0.9=="E",]
  Bsamp<-Btemp[(sample(nrow(Btemp), size=7)),]
  
  bat_samp=rbind(Bsamp, Esamp)
  
  RF<-randomForest(P0.9 ~., data=bat_samp, importance=TRUE, ntree=1000, nperm=100)
  
  imp=RF$importance
  write.table(imp,file="RF_samp_imp_pinkgreen.csv", sep=",", append=T, col.names=!file.exists("RF_samp_imp_pinkgreen.csv"))
  
  err=RF$err.rate
  write.table(err, file="RF_samp_error_pinkgreen.csv", sep=",", row.names=FALSE, col.names=FALSE, append=T)
  
}

oob<-read.csv("RF_samp_error_pinkgreen.csv", header=FALSE)

mean(oob$V1)
mean(oob$V2)
mean(oob$V3)

#import RF_samp_imp_beige.csv
m<-aggregate(i[,4], list(i$var), mean)
write.csv(m, file="RF_mean_imp_beige.csv", quote=FALSE, row.names=FALSE)

#######OLD AGAIN#########
#P = 0.95 predict model probs with all spatial variables
bat<-dat[,c(32,20:31)]
bat<-bat[!(bat$P0.95 =="NA"),]
bat<-na.omit(bat)
bat$P0.95<-as.factor(bat$P0.95)
table(bat$P0.95)
RF<-randomForest(P0.95 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF

#P = 0.9 predict model probs with all spatial variables
bat<-dat[,c(33,20:31)]
bat<-bat[!(bat$P0.9 =="NA"),]
bat<-na.omit(bat)
bat$P0.9<-as.factor(bat$P0.9)
table(bat$P0.9)
RF<-randomForest(P0.9 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF

#P = 0.8 predict model probs with all spatial variables
bat<-dat[,c(34,20:31)]
bat<-bat[!(bat$P0.8 =="NA"),]
bat<-na.omit(bat)
bat$P0.8<-as.factor(bat$P0.8)
table(bat$P0.8)
RF<-randomForest(P0.8 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF

#P = 0.7 predict model probs with all spatial variables
bat<-dat[,c(35,20:31)]
bat<-bat[!(bat$P0.7 =="NA"),]
bat<-na.omit(bat)
bat$P0.7<-as.factor(bat$P0.7)
table(bat$P0.7)
RF<-randomForest(P0.7 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF

#P = 0.95 predict model probs with change spatial variables
bat<-dat[,c(32,23,27,31)]
bat<-bat[!(bat$P0.95 =="NA"),]
bat<-na.omit(bat)
bat$P0.95<-as.factor(bat$P0.95)
table(bat$P0.95)
RF<-randomForest(P0.95 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF

#P = 0.9 predict model probs with change spatial variables
bat<-dat[,c(33,23,27,31)]
bat<-bat[!(bat$P0.9 =="NA"),]
bat<-na.omit(bat)
bat$P0.9<-as.factor(bat$P0.9)
table(bat$P0.9)
RF<-randomForest(P0.9 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF

#P = 0.8 predict model probs with change spatial variables
bat<-dat[,c(34,23,27,31)]
bat<-bat[!(bat$P0.8 =="NA"),]
bat<-na.omit(bat)
bat$P0.8<-as.factor(bat$P0.8)
table(bat$P0.8)
RF<-randomForest(P0.8 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF

#P = 0.7 predict model probs with change spatial variables
bat<-dat[,c(35,23,27,31)]
bat<-bat[!(bat$P0.7 =="NA"),]
bat<-na.omit(bat)
bat$P0.7<-as.factor(bat$P0.7)
table(bat$P0.7)
RF<-randomForest(P0.7 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF

#P = 0.95 predict model probs with change spatial variables + other
bat<-dat[,c(32,23,27,31,8:16,19)]
bat<-bat[!(bat$P0.95 =="NA"),]
bat<-na.omit(bat)
bat$P0.95<-as.factor(bat$P0.95)
table(bat$P0.95)
RF<-randomForest(P0.95 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF

#P = 0.9 predict model probs with change spatial variables + other
bat<-dat[,c(33,23,27,31,8:16,19)]
bat<-bat[!(bat$P0.9 =="NA"),]
bat<-na.omit(bat)
bat$P0.9<-as.factor(bat$P0.9)
table(bat$P0.9)
RF<-randomForest(P0.9 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF

#P = 0.8 predict model probs with change spatial variables + other
bat<-dat[,c(34,23,27,31,8:16,19)]
bat<-bat[!(bat$P0.8 =="NA"),]
bat<-na.omit(bat)
bat$P0.8<-as.factor(bat$P0.8)
table(bat$P0.8)
RF<-randomForest(P0.8 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF

#P = 0.7 predict model probs with change spatial variables + other
bat<-dat[,c(35,23,27,31,8:16,19)]
bat<-bat[!(bat$P0.7 =="NA"),]
bat<-na.omit(bat)
bat$P0.7<-as.factor(bat$P0.7)
table(bat$P0.7)
RF<-randomForest(P0.7 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF


#P = 0.95 DO data characteristics matter???
bat<-dat[,c(32,3:6)]
bat<-bat[!(bat$P0.95 =="NA"),]
bat<-na.omit(bat)
bat$P0.95<-as.factor(bat$P0.95)
table(bat$P0.95)
RF<-randomForest(P0.95 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF

#P = 0.8 DO data characteristics matter???
bat<-dat[,c(34,3:6)]
bat<-bat[!(bat$P0.8 =="NA"),]
bat<-na.omit(bat)
bat$P0.8<-as.factor(bat$P0.8)
table(bat$P0.8)
RF<-randomForest(P0.8 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF


#########
###OLD###
#########

dat$P0.9<-NA
dat$P0.8<-NA
dat$P0.7<-NA






#P = 0.9 predict model probs with change spatial variables
bat<-dat[,c(28,21,24,27)]
bat<-bat[!(bat$P0.9 =="NA"),]
bat<-na.omit(bat)
bat$P0.9<-as.factor(bat$P0.9)
table(bat$P0.9)
RF<-randomForest(P0.9 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF

#P = 0.9 predict model probs with all variables
bat<-dat[,c(28,10:27)]
bat<-bat[!(bat$P0.9 =="NA"),]
bat<-na.omit(bat)
bat$P0.9<-as.factor(bat$P0.9)
table(bat$P0.9)

bat$dietaryniche<-as.factor(bat$dietaryniche)
bat$breeding<-as.factor(bat$breeding)
bat$roostincaves<-as.factor(bat$roostincaves)
bat$roostintreesvegetation<-as.factor(bat$roostintreesvegetation)

RF<-randomForest(P0.9 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF

imp <- data.frame(importance(RF))
MDA<-imp[order(imp$MeanDecreaseAccuracy,decreasing = T),]
write.csv(MDA, file="MDA_0.9_allv.csv")

#P = 0.9 predict model probs with other variables
bat<-dat[,c(28,10:18)]
bat<-bat[!(bat$P0.9 =="NA"),]
bat<-na.omit(bat)
bat$P0.9<-as.factor(bat$P0.9)
table(bat$P0.9)

bat$dietaryniche<-as.factor(bat$dietaryniche)
bat$breeding<-as.factor(bat$breeding)
bat$roostincaves<-as.factor(bat$roostincaves)
bat$roostintreesvegetation<-as.factor(bat$roostintreesvegetation)

RF<-randomForest(P0.9 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF

imp <- data.frame(importance(RF))
MDA<-imp[order(imp$MeanDecreaseAccuracy,decreasing = T),]
write.csv(MDA, file="MDA_0.9_otherv.csv")

#P = 0.8 predict model probs with all spatial variables
bat<-dat[,c(29,19:27)]
bat<-bat[!(bat$P0.8 =="NA"),]
bat<-na.omit(bat)
bat$P0.8<-as.factor(bat$P0.8)
table(bat$P0.8)
RF<-randomForest(P0.8 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF

#P = 0.8 predict model probs with change spatial variables
bat<-dat[,c(29,21,24,27)]
bat<-bat[!(bat$P0.8 =="NA"),]
bat<-na.omit(bat)
bat$P0.8<-as.factor(bat$P0.8)
table(bat$P0.8)
RF<-randomForest(P0.8 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF

#P = 0.8 predict model probs with all variables
bat<-dat[,c(29,10:27)]
bat<-bat[!(bat$P0.8 =="NA"),]
bat<-na.omit(bat)
bat$P0.8<-as.factor(bat$P0.8)
table(bat$P0.8)

bat$dietaryniche<-as.factor(bat$dietaryniche)
bat$breeding<-as.factor(bat$breeding)
bat$roostincaves<-as.factor(bat$roostincaves)
bat$roostintreesvegetation<-as.factor(bat$roostintreesvegetation)

RF<-randomForest(P0.8 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF

#P = 0.8 predict model probs with other variables
bat<-dat[,c(29,10:18)]
bat<-bat[!(bat$P0.8 =="NA"),]
bat<-na.omit(bat)
bat$P0.8<-as.factor(bat$P0.8)
table(bat$P0.8)

bat$dietaryniche<-as.factor(bat$dietaryniche)
bat$breeding<-as.factor(bat$breeding)
bat$roostincaves<-as.factor(bat$roostincaves)
bat$roostintreesvegetation<-as.factor(bat$roostintreesvegetation)

RF<-randomForest(P0.8 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF


#P = 0.7 predict model probs with all spatial variables
bat<-dat[,c(30,19:27)]
bat<-bat[!(bat$P0.7 =="NA"),]
bat<-na.omit(bat)
bat$P0.7<-as.factor(bat$P0.7)
table(bat$P0.7)
RF<-randomForest(P0.7 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF

#P = 0.7 predict model probs with change spatial variables
bat<-dat[,c(30,21,24,27)]
bat<-bat[!(bat$P0.7 =="NA"),]
bat<-na.omit(bat)
bat$P0.7<-as.factor(bat$P0.7)
table(bat$P0.7)
RF<-randomForest(P0.7 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF

#P = 0.7 predict model probs with all variables
bat<-dat[,c(30,10:27)]
bat<-bat[!(bat$P0.7 =="NA"),]
bat<-na.omit(bat)
bat$P0.7<-as.factor(bat$P0.7)
table(bat$P0.7)

bat$dietaryniche<-as.factor(bat$dietaryniche)
bat$breeding<-as.factor(bat$breeding)
bat$roostincaves<-as.factor(bat$roostincaves)
bat$roostintreesvegetation<-as.factor(bat$roostintreesvegetation)

RF<-randomForest(P0.7 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF

#P = 0.7 predict model probs with other variables
bat<-dat[,c(30,10:18)]
bat<-bat[!(bat$P0.7 =="NA"),]
bat<-na.omit(bat)
bat$P0.7<-as.factor(bat$P0.7)
table(bat$P0.7)

bat$dietaryniche<-as.factor(bat$dietaryniche)
bat$breeding<-as.factor(bat$breeding)
bat$roostincaves<-as.factor(bat$roostincaves)
bat$roostintreesvegetation<-as.factor(bat$roostintreesvegetation)

RF<-randomForest(P0.7 ~., data=bat, importance=TRUE, ntree=1000, nperm=100)
RF


#downsampling P = 0.7 other variables
bat<-dat[,c(30,10:18)]
bat<-bat[!(bat$P0.7 =="NA"),]
bat<-na.omit(bat)
bat$P0.7<-as.factor(bat$P0.7)
table(bat$P0.7)

bat$dietaryniche<-as.factor(bat$dietaryniche)
bat$breeding<-as.factor(bat$breeding)
bat$roostincaves<-as.factor(bat$roostincaves)
bat$roostintreesvegetation<-as.factor(bat$roostintreesvegetation)

for (i in 1:100) {
  Bsamp=bat[bat$P0.7=="B",]
  Etemp=bat[bat$P0.7=="E",]
  Esamp<-Etemp[(sample(nrow(Etemp), size=14)),]
  
  bat_samp=rbind(Bsamp, Esamp)
  
  RF<-randomForest(P0.7 ~., data=bat_samp, importance=TRUE, ntree=1000, nperm=100)
  
  imp=RF$importance
  write.table(imp,file="RF_samp_imp_otherv.csv", sep=",", append=T, col.names=!file.exists("RF_samp_imp_otherv.csv"))
  
  err=RF$err.rate
  write.table(err, file="RF_samp_error_otherv.csv", sep=",", row.names=FALSE, col.names=FALSE, append=T)
  
}

oob<-read.csv("RF_samp_error_otherv.csv", header=FALSE)

mean(oob$V1)
mean(oob$V2)
mean(oob$V3)

#import RF_samp_imp_otherv.csv
m<-aggregate(i[,4], list(i$var), mean)
write.csv(m, file="RF_mean_imp_otherv.csv", quote=FALSE, row.names=FALSE)

#downsampling P = 0.7 all variables
bat<-dat[,c(30,10:27)]
bat<-bat[!(bat$P0.7 =="NA"),]
bat<-na.omit(bat)
bat$P0.7<-as.factor(bat$P0.7)
table(bat$P0.7)

bat$dietaryniche<-as.factor(bat$dietaryniche)
bat$breeding<-as.factor(bat$breeding)
bat$roostincaves<-as.factor(bat$roostincaves)
bat$roostintreesvegetation<-as.factor(bat$roostintreesvegetation)

for (i in 1:100) {
  Bsamp=bat[bat$P0.7=="B",]
  Etemp=bat[bat$P0.7=="E",]
  Esamp<-Etemp[(sample(nrow(Etemp), size=11)),]
  
  bat_samp=rbind(Bsamp, Esamp)
  
  RF<-randomForest(P0.7 ~., data=bat_samp, importance=TRUE, ntree=1000, nperm=100)
  
  imp=RF$importance
  write.table(imp,file="RF_samp_imp_allv.csv", sep=",", append=T, col.names=!file.exists("RF_samp_imp_allv.csv"))
  
  err=RF$err.rate
  write.table(err, file="RF_samp_error_allv.csv", sep=",", row.names=FALSE, col.names=FALSE, append=T)
  
}

oob<-read.csv("RF_samp_error_allv.csv", header=FALSE)

mean(oob$V1)
mean(oob$V2)
mean(oob$V3)

#import RF_samp_imp_allv.csv
m<-aggregate(i[,4], list(i$var), mean)
write.csv(m, file="RF_mean_imp_allv.csv", quote=FALSE, row.names=FALSE)

############
#####OLD####
############

#predict e or c with spatial data - 0.9 cut-off
RF_datafile<-RF_datafile[,c(69,55:63)]
names(RF_datafile)
RF_datafile<-na.omit(RF_datafile)
RF_datafile<-RF_datafile[!(RF_datafile$Model_2_0.9 =="na"),]
RF_datafile$Model_2_0.9<-as.character(RF_datafile$Model_2_0.9)
RF_datafile$Model_2_0.9<-as.factor(RF_datafile$Model_2_0.9)
RF<-randomForest(Model_2_0.9 ~., data=RF_datafile, importance=TRUE, ntree=1000, nperm=100)
RF

RF_datafile<-RF_datafile[,c(69,57, 60, 63)]
names(RF_datafile)
RF_datafile<-na.omit(RF_datafile)
RF_datafile<-RF_datafile[!(RF_datafile$Model_2_0.9 =="na"),]
RF_datafile$Model_2_0.9<-as.character(RF_datafile$Model_2_0.9)
RF_datafile$Model_2_0.9<-as.factor(RF_datafile$Model_2_0.9)
RF<-randomForest(Model_2_0.9 ~., data=RF_datafile, importance=TRUE, ntree=1000, nperm=100)
RF

#predict e or c with spatial data - 0.8 cut-off
RF_datafile<-RF_datafile[,c(70,55:63)]
names(RF_datafile)
RF_datafile<-na.omit(RF_datafile)
RF_datafile<-RF_datafile[!(RF_datafile$Model_2_0.8 =="na"),]
RF_datafile$Model_2_0.8<-as.character(RF_datafile$Model_2_0.8)
RF_datafile$Model_2_0.8<-as.factor(RF_datafile$Model_2_0.8)
RF<-randomForest(Model_2_0.8 ~., data=RF_datafile, importance=TRUE, ntree=1000, nperm=100)
RF

RF_datafile<-RF_datafile[,c(70,57, 60, 63)]
names(RF_datafile)
RF_datafile<-na.omit(RF_datafile)
RF_datafile<-RF_datafile[!(RF_datafile$Model_2_0.8 =="na"),]
RF_datafile$Model_2_0.8<-as.character(RF_datafile$Model_2_0.8)
RF_datafile$Model_2_0.8<-as.factor(RF_datafile$Model_2_0.8)
RF<-randomForest(Model_2_0.8 ~., data=RF_datafile, importance=TRUE, ntree=1000, nperm=100)
RF

#change characters to factors
RF_datafile$dietaryniche<-as.factor(RF_datafile$dietaryniche)
RF_datafile$breeding<-as.factor(RF_datafile$breeding)
RF_datafile$roostinbulidings<-as.factor(RF_datafile$roostinbulidings)
RF_datafile$roostincaves<-as.factor(RF_datafile$roostincaves)
RF_datafile$roostintreesvegetation<-as.factor(RF_datafile$roostintreesvegetation)

#predict model probs with other data
RF_datafile<-RF_datafile[,c(65,12,13,37,40,44,45,46)]
names(RF_datafile)
RF_datafile<-na.omit(RF_datafile)
RF<-randomForest(e_pp ~., data=RF_datafile, importance=TRUE, ntree=1000, nperm=100)
RF

#e or c with other data (0.9)
RF_datafile<-RF_datafile[,c(69,12,13,37,40,44,45,46)]
names(RF_datafile)
RF_datafile<-na.omit(RF_datafile)
RF_datafile<-RF_datafile[!(RF_datafile$Model_2_0.9 =="na"),]
RF_datafile$Model_2_0.9<-as.character(RF_datafile$Model_2_0.9)
RF_datafile$Model_2_0.9<-as.factor(RF_datafile$Model_2_0.9)
RF<-randomForest(Model_2_0.9 ~., data=RF_datafile, importance=TRUE, ntree=1000, nperm=100)
RF

#e or c with other data (0.8)
RF_datafile<-RF_datafile[,c(70,12,13,37,40,44,45,46)]
names(RF_datafile)
RF_datafile<-na.omit(RF_datafile)
RF_datafile<-RF_datafile[!(RF_datafile$Model_2_0.8 =="na"),]
RF_datafile$Model_2_0.8<-as.character(RF_datafile$Model_2_0.8)
RF_datafile$Model_2_0.8<-as.factor(RF_datafile$Model_2_0.8)
RF<-randomForest(Model_2_0.8 ~., data=RF_datafile, importance=TRUE, ntree=1000, nperm=100)
RF

#IMPORTANCE
write.table(RF$importance, file="RF_0.9_other_imp.txt", sep="\t", quote=FALSE)


##DOWNSAMPLING
RF_datafile<-RF_datafile[,c(69,12,13,37,40,44,45,46)]
names(RF_datafile)
RF_datafile<-na.omit(RF_datafile)
RF_datafile<-RF_datafile[!(RF_datafile$Model_2_0.9 =="na"),]
RF_datafile$Model_2_0.9<-as.character(RF_datafile$Model_2_0.9)
RF_datafile$Model_2_0.9<-as.factor(RF_datafile$Model_2_0.9)
table(RF_datafile$Model_2_0.9)

for (i in 1:100) {
  Csamp=RF_datafile[RF_datafile$Model_2_0.9=="c",]
  Etemp=RF_datafile[RF_datafile$Model_2_0.9=="e",]
  Esamp<-Etemp[(sample(nrow(Etemp), size=8)),]

  RF_datafile_samp=rbind(Csamp, Esamp)
  
  RF<-randomForest(Model_2_0.9 ~., data=RF_datafile, importance=TRUE, ntree=1000, nperm=100)
  
  imp=RF$importance
  write.table(imp,file="RF_samp_imp.csv", sep=",", append=T, col.names=!file.exists("RF_samp_imp.csv"))
  
  err=RF$err.rate
  write.table(err, file="RF_samp_error.csv", sep=",", row.names=FALSE, col.names=FALSE, append=T)
  
}

oob<-read.csv("RF_samp_error.csv", header=FALSE)

mean(oob$V1)
mean(oob$V2)
mean(oob$V3)

#import RF_samp_imp.csv
names(i)<-c("var","c","e","MDA")

m<-aggregate(i[,4], list(i$var), mean)
write.table(m, file="RF_mean_imp.txt", sep="\t", quote=FALSE, row.names=FALSE)


#ttest
e<-subset(RF_datafile, Model_2_0.8=="e")
c<-subset(RF_datafile, Model_2_0.8=="c")
t.test(e$AdultBodyMass_g, c$AdultBodyMass_g)

#REGRESSION
for (i in 1:nrow(RF_datafile)) {
  if (RF_datafile$Model_2_0.9[i] == "e"){
    RF_datafile$ModelLevel[i]<-1
  } else {
    RF_datafile$ModelLevel[i]<-2
  }}

l<-lm(ModelLevel ~ AdultBodyMass_g, data=RF_datafile)
summary(l)

##########OLD#############
names(RF_datafile)

apply(RF_datafile_x, 2, function(RF_datafile_x)length(unique(RF_datafile_x)))
lapply(RF_datafile_x, class)

#just env data
RF_datafile_x<-RF_datafile[,c(10,55:63)]
names(RF_datafile_x)
RF_datafile_x<-na.omit(RF_datafile_x)
RF<-randomForest(Model ~., data=RF_datafile_x, importance=TRUE, ntree=1000, nperm=100)
RF

RF_datafile_x<-RF_datafile[,c(10,57, 60, 63)]
names(RF_datafile_x)
RF_datafile_x<-na.omit(RF_datafile_x)
RF<-randomForest(Model ~., data=RF_datafile_x, importance=TRUE, ntree=1000, nperm=100)
RF

#predcit model C, E, or B
RF_datafile_x<-RF_datafile[,c(10,12,13,37,40,44,45,46,57,60,63)]
names(RF_datafile_x)
RF_datafile_x<-na.omit(RF_datafile_x)
RF<-randomForest(Model ~., data=RF_datafile_x, importance=TRUE, ntree=1000, nperm=100)
RF

table(RF_datafile_x$Model)

for (i in 1:100) {
  C=RF_datafile_x[RF_datafile_x$Model=="C",]
  B=RF_datafile_x[RF_datafile_x$Model=="B",]
  Bsamp<-B[(sample(nrow(B), size=13)),]
  E=RF_datafile_x[RF_datafile_x$Model=="E",]
  Esamp<-E[(sample(nrow(E), size=13)),]
  
  RF_datafile_y=rbind(C, Bsamp, Esamp)
  
  RF <- randomForest(Model ~ ., data=RF_datafile_y, importance=TRUE, ntree=1000)
  
  imp=RF$importance
  write.table(imp,file="RF_imp.csv", sep=",", row.names=FALSE, append=T)
  
  err=RF$err.rate
  write.table(err, file="RF_error.csv", sep=",", row.names=FALSE, col.names=FALSE, append=T)
  
}  

oob<-read.csv("RF_error.csv", header=FALSE)
mean(oob$V1)

#Explore other things
RF_datafile_x<-RF_datafile[,c(7:10,12,13,37,40,44,45,46,57,60,63)]
RF_datafile_x$constant.size<-as.numeric(as.character(RF_datafile_x$constant.size))
RF_datafile_x<-na.omit(RF_datafile_x)

l<-lm(constant.size ~ areaChange_t500, data=RF_datafile_x)
summary(l)
l<-lm(constant.size ~ areaChange_t700, data=RF_datafile_x)
summary(l)
l<-lm(constant.size ~ areaChange_t900, data=RF_datafile_x)
summary(l)

RF_datafile_x$Holocene.bottleneck<-as.numeric(as.character(RF_datafile_x$Holocene.bottleneck))
RF_datafile_x<-na.omit(RF_datafile_x)

l<-lm(Holocene.bottleneck ~ areaChange_t500, data=RF_datafile_x)
summary(l)
l<-lm(Holocene.bottleneck ~ areaChange_t700, data=RF_datafile_x)
summary(l)
l<-lm(Holocene.bottleneck ~ areaChange_t900, data=RF_datafile_x)
summary(l)

RF_datafile_x$Holocene.expansion<-as.numeric(as.character(RF_datafile_x$Holocene.expansion))
RF_datafile_x<-na.omit(RF_datafile_x)

l<-lm(Holocene.expansion ~ areaChange_t500, data=RF_datafile_x)
summary(l)
l<-lm(Holocene.expansion ~ areaChange_t700, data=RF_datafile_x)
summary(l)
l<-lm(Holocene.expansion ~ areaChange_t900, data=RF_datafile_x)
summary(l)

for (i in 1:nrow(RF_datafile_x)) {
if (RF_datafile_x$Model[i] == "C"){
  RF_datafile_x$ModelLevel[i]<-1
} else if (RF_datafile_x$Model[i] == "E"){
  RF_datafile_x$ModelLevel[i]<-2
} else {
  RF_datafile_x$ModelLevel[i]<-3
}}

l<-lm(ModelLevel ~ areaChange_t500, data=RF_datafile_x)
summary(l)
l<-lm(ModelLevel ~ areaChange_t700, data=RF_datafile_x)
summary(l)
l<-lm(ModelLevel ~ areaChange_t900, data=RF_datafile_x)
summary(l)

##FIRST ROUND
#predict constant size
RF_datafile_x<-RF_datafile[,c(7,11,12,36,39,43,44,45,56,59,62)]
names(RF_datafile_x)
RF_datafile_x$constant.size<-as.numeric(as.character(RF_datafile_x$constant.size))
RF_datafile_x<-na.omit(RF_datafile_x)

RF<-randomForest(constant.size ~., data=RF_datafile_x, importance=TRUE, ntree=1000, nperm=100)
RF


RF_datafile_x<-RF_datafile[,c(7,56,59,62)]
names(RF_datafile_x)
RF_datafile_x$constant.size<-as.numeric(as.character(RF_datafile_x$constant.size))
RF_datafile_x<-na.omit(RF_datafile_x)

RF<-randomForest(constant.size ~., data=RF_datafile_x, importance=TRUE, ntree=1000, nperm=100)
RF


RF_datafile_x<-RF_datafile[,c(7, 11:34, 36,37,39, 43:45, 47:52, 54:62)]
names(RF_datafile_x)
RF_datafile_x$constant.size<-as.numeric(as.character(RF_datafile_x$constant.size))
RF_datafile_x<-na.omit(RF_datafile_x)

RF<-randomForest(constant.size ~., data=RF_datafile_x, importance=TRUE, ntree=1000, nperm=100)
RF

#predict holocene expansion
RF_datafile_x<-RF_datafile[,c(8,11,12,36,39,43,44,45,56,59,62)]
names(RF_datafile_x)
RF_datafile_x$Holocene.expansion<-as.numeric(as.character(RF_datafile_x$Holocene.expansion))
RF_datafile_x<-na.omit(RF_datafile_x)

RF<-randomForest(Holocene.expansion ~., data=RF_datafile_x, importance=TRUE, ntree=1000, nperm=100)
RF


RF_datafile_x<-RF_datafile[,c(8, 11:34, 36,37,39, 43:45, 47:52, 54:62)]
names(RF_datafile_x)
RF_datafile_x$Holocene.expansion<-as.numeric(as.character(RF_datafile_x$Holocene.expansion))
RF_datafile_x<-na.omit(RF_datafile_x)

RF<-randomForest(Holocene.expansion ~., data=RF_datafile_x, importance=TRUE, ntree=1000, nperm=100)
RF


#predict holocene bottleneck
RF_datafile_x<-RF_datafile[,c(9,11,12,36,39,43,44,45,56,59,62)]
names(RF_datafile_x)
RF_datafile_x$Holocene.bottleneck<-as.numeric(as.character(RF_datafile_x$Holocene.bottleneck))
RF_datafile_x<-na.omit(RF_datafile_x)

RF<-randomForest(Holocene.bottleneck ~., data=RF_datafile_x, importance=TRUE, ntree=1000, nperm=100)
RF

RF_datafile_x<-RF_datafile[,c(9, 11:34, 36,37,39, 43:45, 47:52, 54:62)]
names(RF_datafile_x)
RF_datafile_x$Holocene.bottleneck<-as.numeric(as.character(RF_datafile_x$Holocene.bottleneck))
RF_datafile_x<-na.omit(RF_datafile_x)

RF<-randomForest(Holocene.bottleneck ~., data=RF_datafile_x, importance=TRUE, ntree=1000, nperm=100)
RF



##MDA results

mean<-aggregate(imps$MeanDecreaseAccuracy, list(imps$Variable), mean)
min<-aggregate(imps$MeanDecreaseAccuracy, list(imps$Variable), min)
max<-aggregate(imps$MeanDecreaseAccuracy, list(imps$Variable), max)
head(mean)
head(min)
head(max)
n<-merge(mean,min,by="Group.1")
n<-merge(n,max,by="Group.1")
names(n)<-c("Variable","mean","min","max")
head(n)
write.csv(n, file="pinkgreen_avg_mda.csv", row.names=FALSE)
