#!/bin/sh                                                                                                                                                  
                                                                                                                                                           
#Az arduinorol erkezo uzeneteket feldolgozo progi
#Az Openhort rendszerben ez a kod fut a rooteren. Feladata a kommunikacio az arduino es a web kozott, es az arduinonak pontos idot szolgaltatni.
ntpdate 85.12.29.43     #Ido beallitasa
                                                                                                                                                                        
#A terminal beallitasa az uzenetek fogadasahoz es kiirasahoz:
stty -F /dev/ttyUSB0 cs8 9600 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts

#az adatok fileba irasa
lgMsg="/root/log/logMsg"	#Ide kerul minden beerkezo uzenet
lgFile="/root/log/log"		#Ide kerul ami tovabbitasra kerul a server fele
lgWget="/root/log/logWget"      #A wget logja.   

#Web Database
url="http://openhort.xn--rt-lia8e.hu/arduino_data/uploadData.php?"	#itt fut az uploadData.php, ami beirja az adatbazisokba az adatokat
uploadSensors="id=001&action=uploadSensors&data="			#Ha ez az url masodik fele akkor a Sensors DB-be ir
uploadActors="id=001&action=uploadActor&data="				#Ha ez a vege, akkor az Actors DB-be irja be

cat /dev/ttyUSB0 > $lgMsg &	#cat-val olvassuk be az USB-re erkezo adatokat. Ha a tail-val kozvetlenul probaljuk olvasni a ttyUSB0-t nem mukodik a dolog.                                                                                                                          
trap "kill $!" INT PIPE EXIT HUP        # $! cat pid-je, ha a valogato lehal, es ezt az INT...HUP szignalok mellett teszi, a trap magaval rantja a cat-ot

#Ido szinkronizacio
TZ_adjust=2      #Az ido szinkronizalo uzenetnel kell, az idoeltolodas erteke [h]
timeReq="Waiting for sync message"	#Ezt kuldi az arduino, ha idot szeretne szinkronizalni

#Az arduinorol erkezo adatokat belelokjuk egy ciklusba.
                                                                                                                                                                        
tail -f $lgMsg | while read adat; do                                                                                                                                    
        echo "EZT TALALTAM: $adat"      #Debug 

#grep-vel keresunk az uj sorokban kulcsszavakra, es a $? erteket hasznaljuk a feltetelekben ($? exit status ,0:minden rendben talalt valamit, $?>0 valami nem franko)

        echo "$adat" | grep "Waiting"	#Idot akar szinkronizalni?
        if [ $? = 0 ]; then                                                                                                                                            
                echo T$(($(date +%s)+60*60*$TZ_adjust)) > /dev/ttyUSB0  #A "T" A Time TIME_HEADER az arduinon. Valami ilyesmit var: T1234567890 T+UnixTime
                echo "Ido kikuldve!" >> "$lgFile"	#Debug
        else                                                                                                                                                            
	#Akkor ugy nez ki megsem az ido erdekelte okelmet...
                echo "$adat" | grep :	#ha van benne ":", akkor a webre kuldendo adatot talalt.
                if [ $? = 0 ]; then                                                                                                                 
                	echo "$adat" >> "$lgFile"	#Beirjuk a megfelelo logba a rooteren
                        echo "$adat" | grep "\."	#Ha van benne "." akkor az szenzorertek lesz, ha nincs, akkor beavatkozo szerv
                        if [ $? = 0 ]; then                                                                                                         
                                wget -O "$lgWget" "$url""$uploadSensors""$adat"
                        else                                                                                                                        
                                wget -O "$lgWget" "$url""$uploadActors""$adat"
                        fi
                fi
          
        fi
done

