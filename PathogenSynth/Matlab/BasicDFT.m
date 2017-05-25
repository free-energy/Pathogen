pkg load signal

fs = 48000;


N = 512;


dt = 1/fs;
maxt = dt * (N - 1);

t = 0 : (dt) : maxt;


c = 0 * ones(1, N);
c(2) = 1;

Amp = 1.;
freq = 3000;
w = 2 * pi * freq;
phase = 2 * pi * 0.1651
testsine = Amp * sin(w * t + phase) + Amp * sin((w * t / 10) + phase*3.1);

%rect = pulstran(t, 50, 'rectpuls');
rect = rectpuls(t, dt*20);

T = 0 : 1 : (N-1);


%FFT size
x = c;
x = rect;

re = 0 * ones(1, N);
im = 0 * ones(1, N);

y = abs(fft(x, N));


n = (2 * pi) / N;
for i = 1:N/2-1
  for j = 1:N
    re(i) = re(i) + (x(j) * cos(n * i * j));
    im(i) = im(i) + (x(j) * -j * sin(n * i * j));
  endfor
  
endfor

y = abs(re + im);
y = y / max(y);
%y = y / y(1);
yph = angle(im+re);


X1 = [50, 60, 70 ,80, 90 ,100, 80, 60, 20, -20, -10, 20, 30, 50, 55, 100, 120, 150, 200, 100, 50, 0, -10, -10, -20, -40, -50, -60, -70, -80, 100, 200, 300 , 400 ];
X1 = upsample(X1, 10);

TX1 = 1 : 1 : length(X1);

subplot(3,1,1);
plot(TX1 , X1);

Y1 = 0 * ones(1, length(X1));

for i = 1: (length(X1) + length(y) - 1)
  Y1(i) = 0;
  
  for j = 1:length(X1)
    
    if( (j <= i) && (i - j + 1) <= length(y) )
      Y1(i) += X1(j) * y(i-j+1);
    end
    
  endfor
  
endfor

%Y1 = conv(X1, y);
T2 = 1 : 1 : length(Y1);

Y2 = fft(Y1);

subplot(3,1,2);
plot(T2 , Y1);


subplot(3,1,3);
plot(T2 , Y2);