

function [gains,phases] = swanalc(t,f,B,A)
% SWANALC - Perform COMPLEX sine-wave analysis on the 
%           digital filter having transfer function 
%           H(z) = B(z)/A(z)
  
ampin = 1;      % input signal amplitude
phasein = 0;    % input signal phase

N = length(f);       % number of test frequencies
gains = zeros(1,N);  % pre-allocate amp-response array
phases = zeros(1,N); % pre-allocate phase-response array

if length(A)==1, ntransient=length(B)-1, else
  error('Need to set transient response duration here');
end

for k=1:length(f)    % loop over analysis frequencies
  s = ampin*e.^(j*2*pi*f(k)*t+phasein); % test sinusoid
  y = filter(B,A,s); % run it through the filter
  yss = y(ntransient+1:length(y)); % chop off transient
  ampout = mean(abs(yss)); % avg instantaneous amplitude
  gains(k) = ampout/ampin; % amplitude response sample
  sss = s(ntransient+1:length(y)); % align with yss
  phases(k) = mean(mod2pi(angle(yss.*conj(sss))));
end
