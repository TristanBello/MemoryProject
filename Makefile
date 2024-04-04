all: musiqueVictoire musiqueDefaite client

PATH_CC=/home/tristan/Documents/Objet_Connecte/tools-master/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin
CCC=$(PATH_CC)/arm-linux-gnueabihf-gcc

TARGET_WPI=/home/tristan/Documents/Objet_Connecte/wpi/target-wpi

CFLAGS=-I$(TARGET_WPI)/include
LDFLAGS=-L$(TARGET_WPI)/lib


musiqueVictoire: musiqueVictoire.c 
	$(CCC) $(CFLAGS) $(LDFLAGS) $^ -o $@ -lwiringPi

musiqueDefaite: musiqueDefaite.c 
	$(CCC) $(CFLAGS) $(LDFLAGS) $^ -o $@ -lwiringPi

client: client.c
	$(CCC) $(CFLAGS) $(LDFLAGS) $^ -o $@ -lwiringPi
	
	
install : musiqueVictoire musiqueDefaite client
	sshpass -p pi scp musiqueVictoire musiqueDefaite client pi@192.168.97.139:/home/pi/Desktop/gpio.d/Jeu
	sshpass -p pi scp musiqueVictoire musiqueDefaite client pi@192.168.97.7:/home/pi/Desktop/gpio.d/Jeu


clean: 
	rm -rf musiqueVictoire musiqueDefaite client
	
	 
