stty -F /dev/ttyUSB0 115200 
echo -en "PDELAY_ON_DCDC2=500\n" > /dev/ttyUSB0 
sleep 1
echo -en "PDELAY_ON_LIN1=1000\n" > /dev/ttyUSB0
sleep 1
echo -en "PDELAY_ON_DCDC1=1500\n" > /dev/ttyUSB0
sleep 1
echo -en "PDELAY_ON_LIN2=2000\n" > /dev/ttyUSB0
sleep 1
echo -en "PDELAY_ON_EXTRA1=2500\n" > /dev/ttyUSB0
sleep 1
echo -en "PDELAY_ON_EXTRA2=0\n" > /dev/ttyUSB0
sleep 1
echo -en "PDELAY_ON_AMPA=0\n" > /dev/ttyUSB0
sleep 1
echo -en "PDELAY_ON_AMPB=0\n" > /dev/ttyUSB0
sleep 1

echo -en "PDELAY_OFF_DCDC2=2500\n" > /dev/ttyUSB0
sleep 1
echo -en "PDELAY_OFF_LIN1=2000\?" > /dev/ttyUSB0
sleep 1
echo -en "PDELAY_OFF_DCDC1=1500\n" > /dev/ttyUSB0
sleep 1
echo -en "PDELAY_OFF_LIN2=1000\n" > /dev/ttyUSB0
sleep 1
echo -en "PDELAY_OFF_EXTRA1=500\n" > /dev/ttyUSB0
sleep 1
echo -en "PDELAY_OFF_EXTRA2=2500\n" > /dev/ttyUSB0
sleep 1
echo -en "PDELAY_OFF_AMPA=2500\n" > /dev/ttyUSB0
sleep 1
echo -en "PDELAY_OFF_AMPB=2500\n" > /dev/ttyUSB0


