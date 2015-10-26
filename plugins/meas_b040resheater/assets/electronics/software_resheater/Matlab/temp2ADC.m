function [ theta_adc ] = temp2ADC( theta )
% convert a temperature in °C in ADC units

theta_adc=10*theta; %(2731.7+10.51*theta);

end

