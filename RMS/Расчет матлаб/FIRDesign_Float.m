clc
clear

fs = 32E3;
Ts = 1/fs;
N = 3200;
f = logspace(-1,2,4000);

w0 = kaiser(N,10.8);
w1 = chebwin(N,80);

w0 = w0/sum(w0);
w1 = w1/sum(w1);

file = fopen('KaiserWin1_Float.h','w');
fprintf(file,'const float kaiser_win_coeff = %1.15e;\n',1/sum(w0));
fprintf(file,'const float kaiser_win[] = { \n');
fprintf(file,'%1.15e, \n',w0(1:end));
fprintf(file,'}; \n');

sum(w0)
sum(w1)

file = fopen('ChebyWin1_Float.h','w');
fprintf(file,'const float cheby_win_coeff = %1.15e;\n',1/sum(w1));
fprintf(file,'const float cheby_win[] = { \n');
fprintf(file,'%1.15e, \n',w1(1:end));
fprintf(file,'}; \n');

K0 = freqz(w0,1,f,fs);
K1 = freqz(w1,1,f,fs);

figure('Position',[3 50 1254 600])
hAxes=axes('Position',[0.07 0.11 0.9 0.86]);
set(gca,'FontName','Arial Cyr','FontSize',15,'LineWidth',2,'XTick',0:5:100)
hold on, grid on, box on
hline = plot(f,20*log10(abs(K0)),f,20*log10(abs(K1)),'LineWidth',2);

ylabel('A(f), ไม')
xlabel('f, ร๖')

xlim([0 100])
ylim([-120 0])
