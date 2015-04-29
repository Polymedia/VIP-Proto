####### Блок загрузки данных ####### 
горим = read.csv("KAZ_data-1.csv", sep=";")
горим

#возим = read.csv("C:/Users/tereshin/Desktop/фффффффф/1.csv", sep=";")
#возим

###### Блок объявления переменных ####### 

####- firstdWidget -#### 
qwwLeft <- c("26.03.15","27.03.15","28.03.15")
#qwAll <- c("Кол-во спасенных человек","Кол-во пожаров","Кол-во случаев горения","Кол-во аварий","Другие ЧС","Температура")
qwAll <- c("Saved","Fires","Burns","Accident","Other","Temputer")

#qwUP <-  c("Кол-во спасенных человек","Кол-во пожаров")
qwUP <-   c("Saved","Fires")

#qwDown <- c("Кол-во случаев горения","Кол-во аварий","Другие ЧС","Температура")
qwDown <- c("Burns","Accident","Other","Temputer")

qwwRight <- c("29.03.15","30.03.15","31.03.15","01.04.15")

expert <- c(43,28,39,41,33,20,15,13,58,81,79,69,-5,-6,-5,-5)
S<- c(1:4)
F <- c(1:4)
tmp1 <- matrix(0,6,3)
tempdata2 <- subset(горим, горим$Month == 3 & горим$Day == 26:28, select= c(Saved,Fire,Burn,Accident,Other,t..C))

####- secondWidget -####
listout1 <- c(rep("NA",7))
listout2 <- c(rep("NA",7))
listout3 <- c(rep("NA",7))
listout4 <- c(rep("NA",7))

####- thirdWidget -####
summtR <- 0

####- fourWidget -####
tempmonth1 <- subset(горим, горим$Month == 1, select= c(Fire,Burn))
monthsumF1 <- 0
monthsumB1 <- 0

tempmonth2 <- subset(горим, горим$Month == 2, select= c(Fire,Burn))
monthsumF2 <- 0
monthsumB2 <- 0

tempmonth3 <- subset(горим, горим$Month == 3, select= c(Fire,Burn))
monthsumF3 <- 0
monthsumB3 <- 0

#####################################################
## Расчет firstWidget

for (i in seq(1, nrow(tempdata2), by = 1)) {  for (j in seq(1, ncol(tempdata2), by = 1)){    tmp1[j,i]= tempdata2[[i,j]]  }  }

colnames(tmp1)=qwwLeft
row.names(tmp1)=qwAll

#if (exists("inputFromDashboard1")==T) { } else {   } 

if (exists("inputFromDashboard1")==T) {mt=inputFromDashboard1 } else { mt=as.data.frame(t(matrix(expert,4,4)));  colnames(mt)=qwwRight; row.names(mt)=qwDown}

mtR <- as.data.frame(matrix(0,2,4))
colnames(mtR)=qwwRight; row.names(mtR)=qwUP 

for (i in seq(1, nrow(tempdata2), by = 1)) {  for (j in seq(1, ncol(tempdata2), by = 1)){    tmp1[j,i]= tempdata2[[i,j]]    }  }

for (i in seq(1, 4, by = 1)) {  F[i]=(28.55 - 0.61 * mt[4,i]);  mtR[1,i]=round(F[i])}

 for (i in seq(1, 4, by = 1)) {  S[i]=(F[i]*0.0123+mt[2,i]*0.033+mt[3,i]*0.466+mt[4,i]*0.0684+1.324) ;  mtR[2,i]=round(S[i])}

MTR_R <- rbind(mtR,mt)
firstWidget <- cbind(tmp1,MTR_R)

#####################################################
## Расчет secondWidget

for (i in seq(1, 4, by = 1)) {  listout2[i+3] =  round(mtR[2,i])}

for (i in seq(1, 4, by = 1)) {  listout4[i+3] =  round(mtR[1,i])}

for (i in seq(1, 3, by = 1)) {  listout1[i] =  round(tmp1[2,i])}

for (i in seq(1, 3, by = 1)) {  listout3[i] =  round(tmp1[1,i])}

#secondWidget <- data.frame(listout1,listout2,listout3,listout4, row.names=c("26.03.2015","27.03.2015","28.03.2015","29.03.2015","30.03.2015","31.03.2015","01.04.2015"))
secondWidget <- data.frame(listout1,listout2,listout3,listout4, row.names=c("26.03.15","27.03.15","28.03.15","29.03.15","30.03.15","31.03.15","01.04.15"))

colnames(secondWidget) = c("Fire","Fire plan","Saved","Saved plan")

#####################################################
### Расчет thirdWidget

for (i in seq(1, 4, by = 1)) {  summtR= summtR + mtR[2,i]}
thirdWidget <- round(summtR,0)

#####################################################
## Расчет fourthWidget

for (i in seq(1, nrow(tempmonth1), by = 1)) {  monthsumF1 = monthsumF1 + tempmonth1[i,1];  monthsumB1 = monthsumB1 + tempmonth1[i,2]}

for (i in seq(1, nrow(tempmonth2), by = 1)) {  monthsumF2 = monthsumF2 + tempmonth2[i,1];  monthsumB2 = monthsumB2 + tempmonth2[i,2]}

for (i in seq(1, nrow(tempmonth3), by = 1)) {  monthsumF3 = monthsumF3 + tempmonth3[i,1];  monthsumB3 = monthsumB3 + tempmonth3[i,2]}

#fourthWidget <- data.frame( c(monthsumF1,monthsumF2,monthsumF3),c(monthsumB1,monthsumB2,monthsumB3), row.names=c("Январь","Февраль","Март"))
fourthWidget <- data.frame( c(monthsumF1,monthsumF2,monthsumF3),c(monthsumB1,monthsumB2,monthsumB3), row.names=c("Jan","Feb","Mar"))

#colnames(fourthWidget) = c("Кол-во пожаров","Кол-во случаев горения")
colnames(fourthWidget) = c("Fire","Burn")

#####################################################
## Расчет fifthWidget

fifthWidget = read.csv("D:/VIP/bin/RServer/1.csv", sep=";", header=TRUE)