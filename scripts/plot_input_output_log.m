% Sampling frequency
fs = 48000; % in Hz

% Compute the FFT of each vector
input_fft = fft(input_vector);
output_fft = fft(output_vector);

% Frequency axis for the FFT plots (only up to Nyquist frequency)
n = length(input_vector);
frequencies = (0:n-1)*(fs/n);
frequencies = frequencies(1:floor(n/2)); % Take only positive frequencies

% Magnitude of the FFT (only up to Nyquist frequency)
input_fft_magnitude = abs(input_fft(1:floor(n/2)));
output_fft_magnitude = abs(output_fft(1:floor(n/2)));

% Create a new figure
figure;

% Plot input_vector in the top-left subplot
subplot(2, 2, 1); % 2 rows, 2 columns, first subplot
plot(input_vector, 'LineWidth', 2.5);
title('Input Vector');
xlabel('Index');
ylabel('Value');

% Plot output_vector in the top-right subplot
subplot(2, 2, 2); % 2 rows, 2 columns, second subplot
plot(output_vector, 'LineWidth', 2.5);
title('Output Vector');
xlabel('Index');
ylabel('Value');

% Plot FFT of input_vector in the bottom-left subplot
subplot(2, 2, 3); % 2 rows, 2 columns, third subplot
semilogx(frequencies, input_fft_magnitude,'LineWidth', 2.5);
title('FFT of Input Vector');
xlabel('Frequency (Hz)');
ylabel('Magnitude (Log Scale)');
xlim([10 fs/2]); % Limit the x-axis to the Nyquist frequency

% Customize x-axis ticks and labels for better readability
xticks([10, 100, 1000, 10000, fs/2]);
xticklabels({'10', '100', '1000', '10000', num2str(fs/2)});

% Plot FFT of output_vector in the bottom-right subplot
subplot(2, 2, 4); % 2 rows, 2 columns, fourth subplot
semilogx(frequencies, output_fft_magnitude, 'LineWidth', 2.5);
title('FFT of Output Vector');
xlabel('Frequency (Hz)');
ylabel('Magnitude (Log Scale)');
xlim([10 fs/2]); % Limit the x-axis to the Nyquist frequency

% Customize x-axis ticks and labels for better readability
xticks([10, 100, 1000, 10000, fs/2]);
xticklabels({'10', '100', '1000', '10000', num2str(fs/2)});


