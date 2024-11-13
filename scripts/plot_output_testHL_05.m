function plot_output_testHL_05()

inputData = open('../build/Debug/testHL-05-input.mat');
outputData = open('../build/Debug/testHL-05-output.mat');
individualFilterOutputData = open('../build/Debug/testHL-05-filterOutputs.mat');
bandIndicesData = open('../build/Debug/testHL-05-bandIndices.mat');

% Test assumes data is in 'inputVector' y 'outputVector'
inputSignal = inputData.inputVector;
outputSignal = outputData.outputVector;

% Load individual filter outputs in a cell array
% Each individual filter is called filterOutputVector_i , i = 00, 01, 02, ...41
individualFilterOutputs = cell(1, 42);
for i = 0:41
    filterOutputName = sprintf('filterOutputVector_%02d', i);
    individualFilterOutputs{i+1} = individualFilterOutputData.(filterOutputName);
end

% Load band indices
bandIndicesStart = bandIndicesData.bandIndicesStart;
bandIndicesEnd = bandIndicesData.bandIndicesEnd;

% Parameters for pwelch
segment_length = round(0.01 * 48000); % Segment length (adjustable, e.g., 100 ms)
noverlap = round(0.5 * segment_length); % Overlap between segments (adjustable)
nfft = 512; % Number of points for the FFT (adjustable)
fs = 48000; % Sampling frequency

% Convert signals to double
inputSignal = double(inputSignal);
outputSignal = double(outputSignal);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% FIGURE 1: POWER SPECTRAL DENSITY USING PWELCH %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Calculate the PSD using pwelch
[pxx_input, f_input] = pwelch(inputSignal, segment_length, noverlap, nfft, fs);
[pxx_output, f_output] = pwelch(outputSignal, segment_length, noverlap, nfft, fs);

% Plot the PSD of the input signal
figure;
% Change name of whole figure
set(gcf, 'Name', 'INPUT / OUTPUT POWER SPECTRUM');
subplot(2, 1, 1);
f_input_khz = f_input / 1000;
%plot(f_input_khz, 10*log10(pxx_input), 'b'); % 'b' specifies the color blue
semilogx(f_input_khz, 10*log10(pxx_input), 'b', 'LineWidth', 2); % 'b' specifies the color blue
title_str = sprintf('Welch power spectral density estimation (Input) with %d ms segments, fft with %d length', segment_length/fs*1000, nfft);
title(title_str);
xlabel('Frequency (kHz)');
ylabel('Power/Frequency (dB/Hz)');
grid on;
%ylim([-90, -50]);

% Plot the PSD of the output signal
subplot(2, 1, 2);
f_output_khz = f_output / 1000;
%plot(f_output_khz, 10*log10(pxx_output), 'r', 'LineWidth', 2); % 'r' specifies the color red
semilogx(f_output_khz, 10*log10(pxx_output), 'r', 'LineWidth', 2); % 'r' specifies the color red
title_str = sprintf('Welch power spectral density estimation (Output) with %d ms segments, fft with %d length', segment_length/fs*1000, nfft);
title(title_str);
xlabel('Frequency (kHz)');
ylabel('Power/Frequency (dB/Hz)');
grid on;
%ylim([-90, -50]);
hold on;

% Plot the PSD of the individual filter outputs
% Create a colormap with a unique color for each band
numBands = length(bandIndicesStart);
colormap = turbo(numBands);

for band = 1:numBands
    for i = bandIndicesStart(band)+1:bandIndicesEnd(band)+1
        [pxx_filter, f_filter] = pwelch(individualFilterOutputs{i}, segment_length, noverlap, nfft, fs);
        plot(f_filter/1000, 10*log10(pxx_filter), 'Color', colormap(band, :)); % Use unique color from colormap
    end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% FIGURE 2: TRANSFER FUNCTION %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Calculate the transfer function (output/input)
transfer_function = pxx_output ./ pxx_input;

% Plot the transfer function
figure;
% Change name of whole figure
set(gcf, 'Name', 'GAMMATONE FILTER BANK TRANSFER FUNCTION');
% Use a thicker line for the transfer function
%plot(f_input_khz, 10*log10(transfer_function), 'k', 'LineWidth', 2); % 'k' specifies the color black, 'LineWidth' sets the thickness
semilogx(f_input_khz, 10*log10(transfer_function), 'k', 'LineWidth', 2); % 'k' specifies the color black, 'LineWidth' sets the thickness
title_str = sprintf('Gammatone Transfer Function spectral power distribution using Welch with %d ms segments, fft with %d length', segment_length/fs*1000, nfft);
title(title_str);
xlabel('Frequency (kHz)');
ylabel('Magnitude (dB)');
% Change the x-axis to show the frequency band centers 250, 500, 1000, 2000, 4000, 8000 and 16000 Hz
% Change the x-axis to show the frequency band centers 125, 250, 500, 1000, 2000, 4000, 8000 and 16000 Hz and also their separation at those values times sqrt(2), in KHz
set(gca, 'XTick', [0.125, 0.25, 0.5, 1, 2, 4, 8, 16]);
% Change the xlimits to show up to fs/2
xlim([0, fs/2]/1000);
grid on;
hold on;

% Plot the individual filter transfer functions
for band = 1:numBands
    for i = bandIndicesStart(band)+1:bandIndicesEnd(band)+1
        [pxx_filter, f_filter] = pwelch(individualFilterOutputs{i}, segment_length, noverlap, nfft, fs);
        transfer_function_filter = pxx_filter ./ pxx_input;
        plot(f_filter/1000, 10*log10(transfer_function_filter), 'Color', colormap(band, :)); % Use unique color from colormap
    end
end

% Represent the octave bands at 125, 250, 500, 1000, 2000, 4000, 8000 and 16000 Hz using the patch function
% The patch function is used to represent the octave bands in the plot
% figure;
% hold on;

% Define the octave bands. FIXME: Read from data generated by test. 
octave_bands = [125, 250, 500, 1000, 2000, 4000, 8000, 16000];
% Define the colors for the octave bands using the colormap above
octave_colors = colormap(1:numBands, :)

% Define the limits of the octave bands
octave_bands_limits = zeros(2, length(octave_bands));
for i = 1:length(octave_bands)
    octave_bands_limits(1, i) = octave_bands(i) / sqrt(2);
    octave_bands_limits(2, i) = octave_bands(i) * sqrt(2);
end

% Divide octave_bands_limits by 1000 to convert to kHz
octave_bands_limits = octave_bands_limits / 1000;

% Define the limits of the plot
xlimits = get(gca, 'XLim');
ylimits = get(gca, 'YLim');

% add text to the patches that reads "band %d: %d kHz, band, center frequency"
for i = 1:length(octave_bands)
    text(octave_bands(i)/1000, ylimits(1) + 5, sprintf('Band %d:', i));
    text(octave_bands(i)/1000, ylimits(1) + 2, sprintf(' %d Hz', octave_bands(i)));
end

% Plot the octave bands
for i = 1:length(octave_bands)
    x = [octave_bands_limits(1, i), octave_bands_limits(2, i), octave_bands_limits(2, i), octave_bands_limits(1, i)];
    y = [ylimits(1), ylimits(1), ylimits(2), ylimits(2)];
    patch('XData', x, 'YData', y, 'FaceColor', octave_colors(i, :), 'FaceAlpha', 0.2, 'EdgeColor', octave_colors(i, :));
end






%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% FIGURE 3: OCTAVE POWER SPECTRAL DENSITY %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% % Poctave representation of the input signal
% figure;
% % Change name of whole figure
% set(gcf, 'Name', 'INPUT / OUTPUT OCTAVE POWER SPECTRUM');

% subplot(3, 1, 1);
% freq_limit = fs/2; % Maximum visualization frequency in Hz
% poctave(inputSignal, fs, 'FrequencyLimits', [250, freq_limit]);

% % Change color of the bars in the plot a posteriori to blue
% h = findobj(gca,'Type','Bar');
% set(h, 'FaceColor', 'b');
% title_str = sprintf('INPUT Octave power spectral density estimation, max freq: %d Hz, one band per octave (INPUT)', freq_limit);
% title(title_str);

% % Poctave representation of the output signal
% subplot(3, 1, 2);
% poctave(outputSignal, fs, 'FrequencyLimits', [250, freq_limit]);

% % Change color of the bars in the plot a posteriori to red
% h = findobj(gca,'Type','Bar');
% set(h, 'FaceColor', 'r');
% title_str = sprintf('OUTPUT Octave power spectral density estimation, max freq: %d Hz, one band per octave (OUTPUT)', freq_limit);
% title(title_str);


% % Poctave representation of the transfer function
% subplot(3, 1, 3);
% %poctave(transfer_function, fs, 'FrequencyLimits', [30, freq_limit]);
% poctave(transfer_function, fs, 'FrequencyLimits', [250, freq_limit]);
% % Change color of the bars in the plot a posteriori to black
% h = findobj(gca,'Type','Bar');
% set(h, 'FaceColor', 'k');
% title_str = sprintf('TFOctave power spectral density estimation, max freq: %d Hz, one band per octave (TRANSFER F.)', freq_limit);
% title(title_str);

waitfor(gcf);

end
