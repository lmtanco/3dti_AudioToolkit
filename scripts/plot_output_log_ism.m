function plot_output_log_multiple_9(linear)

% Load the output vectors
% Inside whe should have 
% outputLowShelfVector, oputptHighShelfVector, outputBand1Vector, outputBand2Vector, outputBand3Vector, outputBand4Vector, outputBand5Vector, outputBand6Vector, outputBand7Vector
outputs = open('../build/Debug/test-ism.mat')

% Sampling frequency
fs = 48000; % in Hz

% Compute the FFT of the vector
output_fft_left = fft(outputs.left);
output_fft_right = fft(outputs.right);

% Frequency axis for the FFT plots, up to Nyquist frequency
n = length(output_fft_left);
frequencies = (0:n-1)*(fs/n);

% Magnitude of the FFT up to Nyquist frequency and in dB or not depending on parameter linear
if linear
    % display message using linear scale
    disp("Using linear scale")
    output_fft_magnitude_left = abs(output_fft_left);
    output_fft_magnitude_right = abs(output_fft_right);
    mylabel = 'Magnitude (Linear)';
else
    % display message using logarithmic scale
    disp("Using logarithmic scale")
    output_fft_magnitude_left = 20*log10(abs(output_fft_left));
    output_fft_magnitude_right = 20*log10(abs(output_fft_right));
    mylabel = 'Magnitude in DB';
end

% Create a new figure
figure;

% Plot everything in the same figure
semilogx(frequencies, output_fft_magnitude_left, 'LineWidth', 2.5, 'Color', 'b');
hold on;
semilogx(frequencies, output_fft_magnitude_right, 'LineWidth', 2.5, 'Color', 'r');

legend( 'Left channel', 'Right channel', 'Location', 'southwest');
title('FFT of Output Vectors');
xlabel('Frequency (Hz - Log Scale)');
ylabel(mylabel);
xlim([0 fs/2]); % Limit the x-axis to the Nyquist frequency
xticks([0, 10, 100, 1000, 10000, fs/2]);
xticklabels({'', '10', '100', '1000', '10000', num2str(fs/2)});

% if in DB , add 'dB' to the y tick labels
if ~linear
    yticks = get(gca, 'ytick');
    yticklabels = cellstr(num2str(yticks(:)));
    for i = 1:length(yticklabels)
        yticklabels{i} = [yticklabels{i} ' dB'];
    end
    set(gca, 'yticklabel', yticklabels);
end

