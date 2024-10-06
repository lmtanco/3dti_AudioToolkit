function plot_output_log_multiple_9(linear)

% Load the output vectors
% Inside whe should have 
% outputLowShelfVector, oputptHighShelfVector, outputBand1Vector, outputBand2Vector, outputBand3Vector, outputBand4Vector, outputBand5Vector, outputBand6Vector, outputBand7Vector
outputs = open('../build/Debug/test7-grapheq-filters.mat')

% Sampling frequency
fs = 48000; % in Hz

% Compute the FFT of the vector
output_fft = fft(outputs.outputVector);
output_fft_lowshelf = fft(outputs.outputLowShelf);
output_fft_highshelf = fft(outputs.outputHighShelf);
output_fft_band1 = fft(outputs.outputBand1);
output_fft_band2 = fft(outputs.outputBand2);
output_fft_band3 = fft(outputs.outputBand3);
output_fft_band4 = fft(outputs.outputBand4);
output_fft_band5 = fft(outputs.outputBand5);
output_fft_band6 = fft(outputs.outputBand6);
output_fft_band7 = fft(outputs.outputBand7);

% Frequency axis for the FFT plots, up to Nyquist frequency
n = length(output_fft_lowshelf);
frequencies = (0:n-1)*(fs/n);

% Magnitude of the FFT up to Nyquist frequency and in dB or not depending on parameter linear
if linear
    % display message using linear scale
    disp("Using linear scale")
    output_fft_magnitude = abs(output_fft);
    output_fft_magnitude_lowshelf = abs(output_fft_lowshelf);
    output_fft_magnitude_highshelf = abs(output_fft_highshelf);
    output_fft_magnitude_band1 = abs(output_fft_band1);
    output_fft_magnitude_band2 = abs(output_fft_band2);
    output_fft_magnitude_band3 = abs(output_fft_band3);
    output_fft_magnitude_band4 = abs(output_fft_band4);
    output_fft_magnitude_band5 = abs(output_fft_band5);
    output_fft_magnitude_band6 = abs(output_fft_band6);
    output_fft_magnitude_band7 = abs(output_fft_band7);
    mylabel = 'Magnitude (Linear)';
else
    % display message using logarithmic scale
    disp("Using logarithmic scale")
    output_fft_magnitude = 20*log10(abs(output_fft));
    output_fft_magnitude_lowshelf = 20*log10(abs(output_fft_lowshelf));
    output_fft_magnitude_highshelf = 20*log10(abs(output_fft_highshelf));
    output_fft_magnitude_band1 = 20*log10(abs(output_fft_band1));
    output_fft_magnitude_band2 = 20*log10(abs(output_fft_band2));
    output_fft_magnitude_band3 = 20*log10(abs(output_fft_band3));
    output_fft_magnitude_band4 = 20*log10(abs(output_fft_band4));
    output_fft_magnitude_band5 = 20*log10(abs(output_fft_band5));
    output_fft_magnitude_band6 = 20*log10(abs(output_fft_band6));
    output_fft_magnitude_band7 = 20*log10(abs(output_fft_band7));
    mylabel = 'Magnitude in DB';
end

% Create a new figure
figure;

% Plot everything in the same figure
semilogx(frequencies, output_fft_magnitude, 'LineWidth', 2.5, 'Color', 'k');
hold on;
semilogx(frequencies, output_fft_magnitude_lowshelf, 'LineWidth', 2.5, 'Color', 'r');
hold on;
semilogx(frequencies, output_fft_magnitude_highshelf, 'LineWidth', 2.5, 'Color', 'g');
hold on;
semilogx(frequencies, output_fft_magnitude_band1, 'LineWidth', 2.5, 'Color', 'b');
hold on;
semilogx(frequencies, output_fft_magnitude_band2, 'LineWidth', 2.5, 'Color', 'c');
hold on;
semilogx(frequencies, output_fft_magnitude_band3, 'LineWidth', 2.5, 'Color', 'm');
hold on;
semilogx(frequencies, output_fft_magnitude_band4, 'LineWidth', 2.5, 'Color', 'y');
hold on;
semilogx(frequencies, output_fft_magnitude_band5, 'LineWidth', 2.5, 'Color', 'k');
hold on;
semilogx(frequencies, output_fft_magnitude_band6, 'LineWidth', 2.5, 'Color', 'r');
hold on;
semilogx(frequencies, output_fft_magnitude_band7, 'LineWidth', 2.5, 'Color', 'g');


legend( 'Total', 'Lowshelf', 'Highshelf', 'Band1', 'Band2', 'Band3', 'Band4', 'Band5', 'Band6', 'Band7', 'REF', 'Location', 'northwest');
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

