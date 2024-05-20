% Compute the FFT of each vector
input_fft = fft(input_vector);
output_fft = fft(output_vector);

% Frequency axis for the FFT plots
n = length(input_vector);
frequencies = (0:n-1)*(1/n);

% Create a new figure
figure;

% Plot input_vector in the top-left subplot
subplot(2, 2, 1); % 2 rows, 2 columns, first subplot
plot(input_vector);
title('Input Vector');
xlabel('Frame');
ylabel('Value');

% Plot output_vector in the top-right subplot
subplot(2, 2, 2); % 2 rows, 2 columns, second subplot
plot(output_vector);
title('Output Vector');
xlabel('Frame');
ylabel('Value');

% Plot FFT of input_vector in the bottom-left subplot
subplot(2, 2, 3); % 2 rows, 2 columns, third subplot
plot(frequencies, abs(input_fft));
title('FFT of Input Vector');
xlabel('Frequency');
ylabel('Magnitude');

% Plot FFT of output_vector in the bottom-right subplot
subplot(2, 2, 4); % 2 rows, 2 columns, fourth subplot
plot(frequencies, abs(output_fft));
title('FFT of Output Vector');
xlabel('Frequency');
ylabel('Magnitude');