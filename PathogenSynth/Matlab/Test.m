pkg load signal

fs = 48000;


maxt = 0.01;
dt = 1/fs;



t = 0 : dt : maxt;

N = length(t);

rect = rectpuls(t, 5);


c = 0 * ones(1, N);
c(2) = 1;

Amp = 1.;
freq = 3000;
w = 2 * pi * freq;
testsine = Amp * sin(w * t);

T = 1 : 1 : N;


%FFT size
x = c;
x = testsine;

re = 0 * ones(1, N);
im = 0 * ones(1, N);

y = abs(fft(x, N));


n = (2 * pi) / N;
for i = 1:N/2
  for j = 1:N
    re(i) = re(i) + (x(j) * cos(n * i * j));
    im(i) = im(i) + (x(j) * -j * sin(n * i * j));
  endfor
  
endfor

y = abs(re + im);

subplot(4,1,1);
plot(T , x);

subplot(4,1,2);
plot(T , re);

subplot(4,1,3);
plot(T , im);

subplot(4,1,4);
plot(T , y);