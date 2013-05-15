v 20110115 2
C 40000 40000 0 0 0 title-B.sym
C 51600 44400 1 0 0 resistor-1.sym
{
T 51900 44800 5 10 0 0 0 0 1
device=RESISTOR
T 51700 44700 5 10 1 1 0 0 1
refdes=R1
T 52200 44700 5 10 1 1 0 0 1
value=10K
T 51600 44400 5 10 0 0 0 0 1
footprint=R025
}
C 46400 42400 1 0 0 ATmega8-1.sym
{
T 46500 47200 5 10 0 0 0 0 1
footprint=DIP28N
T 48700 47000 5 10 1 1 0 6 1
refdes=U1
T 46800 47000 5 10 1 1 0 0 1
device=ATmega328
}
C 52700 47400 1 0 0 vcc-1.sym
N 52900 44500 52500 44500 4
N 51600 44500 49000 44500 4
N 52900 43400 52900 47400 4
N 52900 43700 49000 43700 4
N 52900 43400 49000 43400 4
C 49800 49400 1 90 0 crystal-1.sym
{
T 49300 49600 5 10 0 0 90 0 1
device=CRYSTAL
T 50000 49800 5 10 1 1 0 0 1
refdes=U2
T 49100 49600 5 10 0 0 90 0 1
symversion=0.1
T 50500 49700 5 10 1 1 180 0 1
value=16MHz
T 49800 49400 5 10 0 0 0 0 1
footprint=CRYSTAL 300
}
C 47400 49100 1 0 0 gnd-1.sym
C 48200 49900 1 0 0 capacitor-1.sym
{
T 48400 50600 5 10 0 0 0 0 1
device=CAPACITOR
T 48500 50400 5 10 1 1 180 0 1
refdes=C1
T 48400 50800 5 10 0 0 0 0 1
symversion=0.1
T 49200 50400 5 10 1 1 180 0 1
value=22pF
T 48200 49900 5 10 0 0 0 0 1
footprint=ACY100
}
C 48200 49200 1 0 0 capacitor-1.sym
{
T 48400 49900 5 10 0 0 0 0 1
device=CAPACITOR
T 48500 49200 5 10 1 1 180 0 1
refdes=C2
T 48400 50100 5 10 0 0 0 0 1
symversion=0.1
T 49200 49200 5 10 1 1 180 0 1
value=22pF
T 48200 49200 5 10 0 0 0 0 1
footprint=ACY100
}
N 48200 50100 47500 50100 4
N 47500 50100 47500 49400 4
C 53100 42500 1 90 0 capacitor-1.sym
{
T 52400 42700 5 10 0 0 90 0 1
device=CAPACITOR
T 52600 43100 5 10 1 1 180 0 1
refdes=C3
T 52200 42700 5 10 0 0 90 0 1
symversion=0.1
T 52300 42600 5 10 1 1 0 0 1
value=0.1uF
T 53100 42500 5 10 0 0 0 0 1
footprint=ACY200
}
C 52800 42200 1 0 0 gnd-1.sym
C 55300 43900 1 90 0 connector2-1.sym
{
T 54300 44100 5 10 0 0 90 0 1
device=CONNECTOR_2
T 55100 43800 5 10 1 1 180 0 1
refdes=Sw1
T 55300 43900 5 10 0 0 90 0 1
footprint=CONNECTOR 1 2
}
C 55300 47900 1 270 0 connector2-1.sym
{
T 56300 47700 5 10 0 0 270 0 1
device=CONNECTOR_2
T 55500 48000 5 10 1 1 0 0 1
refdes=Batt
T 55300 47900 5 10 0 0 270 0 1
footprint=CONNECTOR 1 2
}
C 55700 45500 1 0 0 gnd-1.sym
C 54600 46300 1 0 0 vcc-1.sym
N 55100 45600 55100 46200 4
N 55100 46200 55500 46200 4
N 54800 45600 54800 46300 4
N 55800 46200 55800 45800 4
C 56100 50000 1 180 0 connector2-1.sym
{
T 55900 49000 5 10 0 0 180 0 1
device=CONNECTOR_2
T 56100 50200 5 10 1 1 180 0 1
refdes=SPKR
T 56100 50000 5 10 0 0 180 0 1
footprint=CONNECTOR 1 2
}
C 53900 48900 1 0 0 gnd-1.sym
N 54000 49200 54000 49500 4
N 54000 49500 54400 49500 4
C 52900 49700 1 0 0 input-2.sym
{
T 53500 50000 5 10 0 1 0 0 1
net=SPKR:1
T 53500 50400 5 10 0 0 0 0 1
device=none
T 53400 49800 5 10 1 1 0 7 1
value=SPKR
}
N 54300 49800 54400 49800 4
C 46100 46200 1 180 0 output-2.sym
{
T 45800 46400 5 10 0 1 180 0 1
net=SPKR:1
T 45900 45500 5 10 0 0 180 0 1
device=none
T 45200 46100 5 10 1 1 180 1 1
value=SPKR
}
N 47500 49400 48200 49400 4
N 49100 49400 50800 49400 4
N 49100 50100 50800 50100 4
C 52200 50200 1 180 0 input-2.sym
{
T 52100 50200 5 10 0 1 180 0 1
net=XTL1:1
T 51600 49500 5 10 0 0 180 0 1
device=none
T 51700 50100 5 10 1 1 180 7 1
value=XTL1
}
C 52200 49500 1 180 0 input-2.sym
{
T 52100 49500 5 10 0 1 180 0 1
net=XTL2:1
T 51600 48800 5 10 0 0 180 0 1
device=none
T 51700 49400 5 10 1 1 180 7 1
value=XTL2
}
C 46600 41700 1 270 0 output-2.sym
{
T 46600 40600 5 10 0 1 0 0 1
net=XTL2:1
T 47300 41500 5 10 0 0 270 0 1
device=none
T 46600 40700 5 10 1 1 0 1 1
value=XTL2
}
C 46100 41700 1 270 0 output-2.sym
{
T 46000 40600 5 10 0 1 0 0 1
net=XTL1:1
T 46800 41500 5 10 0 0 270 0 1
device=none
T 45900 40700 5 10 1 1 0 1 1
value=XTL1
}
N 46400 42800 46200 42800 4
N 46200 42800 46200 41700 4
N 46400 42600 46400 41700 4
N 46400 41700 46700 41700 4
C 43500 49100 1 90 0 resistor-variable-1.sym
{
T 42600 49900 5 10 0 0 90 0 1
device=VARIABLE_RESISTOR
T 43200 49900 5 10 1 1 180 0 1
refdes=R3
T 43200 49400 5 10 1 1 180 0 1
value=47K
}
C 42000 49100 1 90 0 resistor-variable-1.sym
{
T 41100 49900 5 10 0 0 90 0 1
device=VARIABLE_RESISTOR
T 41700 49900 5 10 1 1 180 0 1
refdes=R2
T 41700 49400 5 10 1 1 180 0 1
value=47K
}
C 45000 49100 1 90 0 resistor-variable-1.sym
{
T 44100 49900 5 10 0 0 90 0 1
device=VARIABLE_RESISTOR
T 44700 49900 5 10 1 1 180 0 1
refdes=R4
T 44700 49400 5 10 1 1 180 0 1
value=47K
}
C 46500 49100 1 90 0 resistor-variable-1.sym
{
T 45600 49900 5 10 0 0 90 0 1
device=VARIABLE_RESISTOR
T 46200 49900 5 10 1 1 180 0 1
refdes=R5
T 46200 49400 5 10 1 1 180 0 1
value=47K
}
N 46100 46100 46400 46100 4
C 41800 47500 1 0 0 pushbutton.sym
{
T 41800 47800 5 10 1 1 0 0 1
refdes=btn1
}
C 41800 46800 1 0 0 pushbutton.sym
{
T 41800 47100 5 10 1 1 0 0 1
refdes=btn2
}
C 41800 46100 1 0 0 pushbutton.sym
{
T 41800 46400 5 10 1 1 0 0 1
refdes=btn3
}
C 41800 45400 1 0 0 pushbutton.sym
{
T 41800 45700 5 10 1 1 0 0 1
refdes=btn4
}
C 41100 40700 1 0 0 gnd-1.sym
N 41200 41000 41200 47600 4
N 41200 45500 41800 45500 4
N 41800 46200 41200 46200 4
N 41800 46900 41200 46900 4
N 41800 47600 41200 47600 4
C 41800 44200 1 0 0 pushbutton.sym
{
T 41800 44500 5 10 1 1 0 0 1
refdes=btn5
}
C 41800 43500 1 0 0 pushbutton.sym
{
T 41800 43800 5 10 1 1 0 0 1
refdes=btn6
}
C 41800 42800 1 0 0 pushbutton.sym
{
T 41800 43100 5 10 1 1 0 0 1
refdes=btn7
}
C 41800 42100 1 0 0 pushbutton.sym
{
T 41800 42400 5 10 1 1 0 0 1
refdes=btn8
}
C 41800 41400 1 0 0 pushbutton.sym
{
T 41800 41700 5 10 1 1 0 0 1
refdes=btn9
}
N 41800 41500 41200 41500 4
N 41800 42200 41200 42200 4
N 41800 42900 41200 42900 4
N 41800 43600 41200 43600 4
N 41800 44300 41200 44300 4
N 42600 47600 44300 47600 4
N 44300 47600 44300 45700 4
N 44300 45700 46400 45700 4
N 46400 45500 43800 45500 4
N 43800 45500 43800 46900 4
N 43800 46900 42600 46900 4
N 42600 46200 43300 46200 4
N 43300 46200 43300 45300 4
N 43300 45300 46400 45300 4
N 42600 45500 42800 45500 4
N 42800 45500 42800 44900 4
N 42800 44900 45700 44900 4
N 45700 44900 45700 44000 4
N 45700 44000 46400 44000 4
N 42600 44300 45200 44300 4
N 45200 44300 45200 43800 4
N 45200 43800 46400 43800 4
N 42600 43600 46400 43600 4
N 46400 43400 43200 43400 4
N 43200 43400 43200 42900 4
N 43200 42900 42600 42900 4
N 42600 42200 43700 42200 4
N 43700 42200 43700 43200 4
N 43700 43200 46400 43200 4
N 46400 43000 44200 43000 4
N 44200 43000 44200 41500 4
N 44200 41500 42600 41500 4
C 41700 50200 1 0 0 vcc-1.sym
N 41900 50000 41900 50200 4
N 43400 50000 43400 50100 4
N 43400 50100 41900 50100 4
N 41900 50100 46400 50100 4
N 44900 50100 44900 50000 4
N 46400 50100 46400 50000 4
C 40900 49400 1 270 0 output-2.sym
{
T 40700 49100 5 10 0 1 270 0 1
net=ADC0:1
T 41600 49200 5 10 0 0 270 0 1
device=none
T 40700 48400 5 10 1 1 0 1 1
value=ADC0
}
N 41000 49400 41000 49600 4
N 41000 49600 41400 49600 4
C 42400 49400 1 270 0 output-2.sym
{
T 42200 49100 5 10 0 1 270 0 1
net=ADC1:1
T 43100 49200 5 10 0 0 270 0 1
device=none
T 42200 48400 5 10 1 1 0 1 1
value=ADC1
}
C 43900 49400 1 270 0 output-2.sym
{
T 43700 49100 5 10 0 1 270 0 1
net=ADC2:1
T 44600 49200 5 10 0 0 270 0 1
device=none
T 43700 48400 5 10 1 1 0 1 1
value=ADC2
}
C 45400 49400 1 270 0 output-2.sym
{
T 45200 49100 5 10 0 1 270 0 1
net=ADC3:1
T 46100 49200 5 10 0 0 270 0 1
device=none
T 45200 48400 5 10 1 1 0 1 1
value=ADC3
}
N 42500 49400 42500 49600 4
N 42500 49600 42900 49600 4
N 44000 49400 44000 49600 4
N 44000 49600 44400 49600 4
N 45500 49400 45500 49600 4
N 45500 49600 45900 49600 4
C 41800 48700 1 0 0 gnd-1.sym
C 43300 48700 1 0 0 gnd-1.sym
C 44800 48700 1 0 0 gnd-1.sym
C 46300 48700 1 0 0 gnd-1.sym
N 41900 49000 41900 49100 4
N 43400 49000 43400 49100 4
N 44900 49000 44900 49100 4
N 46400 49000 46400 49100 4
C 52100 46600 1 180 0 input-2.sym
{
T 51500 46300 5 10 0 1 180 0 1
net=ADC1:1
T 51500 45900 5 10 0 0 180 0 1
device=none
T 51600 46500 5 10 1 1 180 7 1
value=ADC1
}
C 52100 46100 1 180 0 input-2.sym
{
T 51500 45800 5 10 0 1 180 0 1
net=ADC2:1
T 51500 45400 5 10 0 0 180 0 1
device=none
T 51600 46000 5 10 1 1 180 7 1
value=ADC2
}
C 52100 45600 1 180 0 input-2.sym
{
T 51500 45300 5 10 0 1 180 0 1
net=ADC3:1
T 51500 44900 5 10 0 0 180 0 1
device=none
T 51600 45500 5 10 1 1 180 7 1
value=ADC3
}
C 52100 47100 1 180 0 input-2.sym
{
T 51500 46800 5 10 0 1 180 0 1
net=ADC0:1
T 51500 46400 5 10 0 0 180 0 1
device=none
T 51600 47000 5 10 1 1 180 7 1
value=ADC0
}
N 49000 45100 50400 45100 4
N 50400 45100 50400 45500 4
N 50400 45500 50700 45500 4
N 49000 45300 50000 45300 4
N 50000 45300 50000 46000 4
N 50000 46000 50700 46000 4
N 49000 45500 49600 45500 4
N 49600 45500 49600 46500 4
N 49600 46500 50700 46500 4
N 49000 45700 49200 45700 4
N 49200 45700 49200 47000 4
N 49200 47000 50700 47000 4