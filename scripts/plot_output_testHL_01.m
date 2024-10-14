function plot_output_testHL_01()

inputData = open('../build/Debug/testHL-01-input.mat');
outputData = open('../build/Debug/testHL-01-output.mat');
load('inversesweep_Hsweep_4seg.mat');

figure; 
%plot Hsweep


% Asumimos que los datos están almacenados en variables llamadas 'inputVector' y 'outputVector'
inputSignal = inputData.inputVector;
outputSignal = outputData.outputVector;

% Representar las señales en el dominio del tiempo
figure;

subplot(2, 1, 1);
plot(inputSignal);
title('Señal de Entrada en el Dominio del Tiempo');
xlabel('Muestras');
ylabel('Amplitud');

subplot(2, 1, 2);
plot(outputSignal);
title('Señal de Salida en el Dominio del Tiempo');
xlabel('Muestras');
ylabel('Amplitud');

% Calcular la FFT de ambas señales
inputFFT = fft(inputSignal);

% Aplicar la inversa de la respuesta en frecuencia de la  entrada a la señal de salida
outputFFT = fft(outputSignal).*(Hsweep');

% Calcular el espectro de frecuencias
n_input = length(inputSignal);
f_input = (0:n_input-1)*(48000/n_input); % Asumiendo una frecuencia de muestreo de 48000 Hz
n_output = length(outputSignal);
f_output = (0:n_output-1)*(48000/n_output); % Asumiendo una frecuencia de muestreo de 48000 Hz

%Visualizar sólo las mitades delos espectros
half_n_input = floor(n_input/2);
half_n_output = floor(n_output/2);


% Plot Hsweep
figure;
plot(f_input(1:half_n_input), abs(Hsweep(1:half_n_input)));
title('Inversa de la Respuesta en Frecuencia Grabada');
xlabel('Frecuencia (Hz)');
ylabel('Amplitud');
xlim([0, 24000]); % Limitar el eje x a la mitad de la frecuencia de muestreo


% Representar las señales en el dominio de la frecuencia
figure;



subplot(2, 1, 1);
plot(f_input(1:half_n_input), abs(inputFFT(1:half_n_input)));
title('Señal de Entrada en el Dominio de la Frecuencia');
xlabel('Frecuencia (Hz)');
ylabel('Amplitud');

subplot(2, 1, 2);
plot(f_output(1:half_n_output), abs(outputFFT(1:half_n_output)));
title('Señal de Salida en el Dominio de la Frecuencia');
xlabel('Frecuencia (Hz)');
ylabel('Amplitud');

%figure;
%ir = ifft(outputFFT, [], 1, 'symmetric');
%plot(ir);

%Representar la inversa de la respuesta en frecuencia de la señal de entrada y salida otra vez
% figure;

% subplot(2, 1, 1);
% inputSignalAgain = ifft(inputFFT, [], 1, 'symmetric');
% plot(inputSignalAgain);
% title('Inversa de la Respuesta en Frecuencia de la Señal de Entrada');
% xlabel('Muestras');
% ylabel('Amplitud');

% subplot(2, 1, 2);
% outputSignalAgain = ifft(outputFFT, [], 1, 'symmetric');
% plot(outputSignalAgain);
% title('Inversa de la Respuesta en Frecuencia de la Señal de Salida');
% xlabel('Muestras');
% ylabel('Amplitud');

% Esperar a que el usuario cierre las ventanas de las gráficas
waitfor(gcf);

end
