% This file can be run using:
% $ octave --persist octave_fft.m

M = load('tip.dat');    % Matrix laden
x = M(:,1);
y = M(:,2);


fs  = 2000;                                 % Abtastfrequenz
N   = length(x);                             % Laenge der Vektoren
f   = 0:1/N*fs:(N-1)/N*fs;                   % Frequenzvektor

fty = fft(y);             % Fouriertransformation y

ps = 20.*log10((abs(fty)));
ps2 = 20.*log10((abs(fty).^2));


figure(1);                % Bildschirmplot
plot(f,y);         % Betragsspektrum x-Vektor
figure(2);                % Bildschirmplot
plot(f,ps);         % Betragsspektrum z-Vektor

%plot(f,Y);         % Betragsspektrum z-Vektor
xlabel('f/Hz');
ylabel('Amplitudenspektrum');

figure(3)
Y1=abs(fty)/max(abs(fty));
plot(f,Y1);

figure(4)
plot(f,20.*log10(Y1));

%print -Peps2 'octave_plot.eps'

