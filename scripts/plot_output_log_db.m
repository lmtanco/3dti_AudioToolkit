function plot_output_log_db(output_vector)

% Sampling frequency
fs = 48000; % in Hz

% Compute the FFT of the vector
output_fft = fft(output_vector);

% Frequency axis for the FFT plots (up to Nyquist frequency)
n = length(output_vector)
frequencies = (0:n-1)*(fs/n);

% Magnitude of the FFT (up to Nyquist frequency)
output_fft_magnitude = abs(output_fft);

% Magnitude of the FFT in dB
output_fft_magnitude_db = 20*log10(output_fft_magnitude);


% Create a new figure
figure;

% Plot input_vector in the top subplot
subplot(2, 1, 1); % 2 rows, 1 columns, first subplot
plot(output_vector, 'LineWidth', 2.5);
title('Output Vector');
xlabel('Index');
ylabel('Value');

% Plot FFT of output_vector in the bottom subplot
subplot(2, 1, 2); % 2 rows, 1 columns, second subplot
semilogx(frequencies, output_fft_magnitude_db, 'LineWidth', 2.5);
% plot(frequencies, output_fft_magnitude, 'LineWidth', 2.5);  
title('FFT of Output Vector');
xlabel('Frequency (Hz - Log Scale)');
ylabel('Magnitude ');
xlim([0 fs/2]); % Limit the x-axis to the Nyquist frequency

% Customize x-axis ticks and labels for better readability
xticks([0, 10, 100, 1000, 10000, fs/2]);
xticklabels({'', '10', '100', '1000', '10000', num2str(fs/2)});

