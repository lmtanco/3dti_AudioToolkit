function plot_output_testHL_01()

inputData = open('../build/Debug/testHL-01-input.mat');
outputData = open('../build/Debug/testHL-01-output.mat');

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
outputFFT = fft(outputSignal);

% Calcular el espectro de frecuencias
n = length(inputSignal);
f = (0:n-1)*(48000/n); % Asumiendo una frecuencia de muestreo de 48000 Hz

% Representar las señales en el dominio de la frecuencia
figure;

subplot(2, 1, 1);
plot(f, abs(inputFFT));
title('Señal de Entrada en el Dominio de la Frecuencia');
xlabel('Frecuencia (Hz)');
ylabel('Amplitud');

subplot(2, 1, 2);
plot(f, abs(outputFFT));
title('Señal de Salida en el Dominio de la Frecuencia');
xlabel('Frecuencia (Hz)');
ylabel('Amplitud');