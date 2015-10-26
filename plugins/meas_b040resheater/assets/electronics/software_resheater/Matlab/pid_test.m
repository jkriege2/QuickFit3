clear;


% Simulationsschritte:
steps=100000;
% Zeiteinheit [s/schritt] für Simulation:
deltaT=1/10;
% Update-Intervall für PID [sim-timesteps]:
PID_deltaT=10;

% zu heizende Masse [g]:
mass=150;
% spezifische Wärmekapazität [J/(g*K)]
cm = 0.48;
% oberfläche des zu heizenden Elements [m^2]
surface = 2*22e-3*22e-3+4*40e-3*22e-3;

% Heizspannung [V]:
Uheat = 9;
% Heizwiderstand [Ohm]:
Rheat=4.7;
% resolution of PWM [bit]
PWMbits=8;

% Start Temperatur [°C]
ThetaStart=20;
% Target Temperature [°C]
ThetaTarget=37;
% Zeitverzögerung [s]
TempDelay=10;

% PID P-factor
PID_Pfactor=10;
% PID I-factor
PID_Ifactor=0;
% PID D-factor
PID_Dfactor=100;
% PID I-limit
PID_Ilimit=100000;

TempDelaySteps=round(TempDelay/deltaT);
disp(['Heater:   Pmax=' num2str(Uheat*Uheat/Rheat) 'W    Imax=' num2str(Uheat/Rheat) 'A']);
Theta(1:steps)=ThetaStart;
PWM(1:steps)=0;
PPWM(1:steps)=0;
PWMmax=2^PWMbits-1;

PID_E(1:steps)=0;
PID_I(1:steps)=0;
PID_D(1:steps)=0;
PID_TERM(1:steps)=0;

treal=((1:steps)-1).*deltaT;

for t=2:steps
   
    if t>TempDelaySteps
        ThetaMeasured=Theta(t-TempDelaySteps);
    else
        ThetaMeasured=ThetaStart;
    end
    PPWM(t-1)=Uheat^2/Rheat*PWM(t-1)/PWMmax;
    % use Stefan Boltzmann law for loss
    DeltaQ=(PPWM(t-1) - surface*5.67e-8*(Theta(t)+273)^4)*deltaT;
    Theta(t)=Theta(t-1)+DeltaQ/(cm*mass);
    
    if mod(t,PID_deltaT)==0
        PID_E(t)=temp2ADC(ThetaTarget)-temp2ADC(ThetaMeasured);
        PID_I(t)=PID_I(t-1)+PID_E(t);
        PID_D(t)=-(PID_E(t-1)-PID_E(t))/2;
        if PID_I(t)>PID_Ilimit 
            PID_I(t)=PID_Ilimit;
        elseif PID_I(t)<-PID_Ilimit
            PID_I(t)=-PID_Ilimit;
        end

        PID_TERM(t)=PID_Pfactor*PID_E(t)+PID_Dfactor*PID_D(t)+PID_Ifactor*PID_I(t);

        PWM(t)=round(PID_TERM(t));
        if PWM(t)>PWMmax
            PWM(t)=PWMmax;
        elseif PWM(t)<0
            PWM(t)=0;
        end
    else
        PWM(t)=PWM(t-1);
        PID_E(t)=PID_E(t-1);
        PID_I(t)=PID_I(t-1);
        PID_D(t)=PID_D(t-1);
        PID_TERM(t)=PID_TERM(t-1);
    end
    
end
plotsmax=3;
plotrange=1:(steps-1);
subplot(plotsmax,1,1);
plot(treal(plotrange), PID_TERM(plotrange));
xlabel('time [s]');
ylabel('PID term');
subplot(plotsmax,1,2);
plot(treal(plotrange), PWM(plotrange));
xlabel('time [s]');
ylabel('P_{PWM} [W]');
subplot(plotsmax,1,3);
plot(treal(plotrange), Theta(plotrange));
xlabel('time [s]');
ylabel('temperature [°C]');



