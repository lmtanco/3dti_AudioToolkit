function plot_output_testHL_03()

inputData = open('../build/Debug/testHL-05-input.mat');
outputData = open('../build/Debug/testHL-05-output.mat');
individualFilterOutputData = open('../build/Debug/testHL-05-filterOutputs.mat');
bandFilterOutputData = open('../build/Debug/testHL-05-bandOutputs.mat');

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

% Load individual band outputs in a cell array
% Each individual band is called bandOutputVector_i , i = 00, 01, 02, ...07
bandFilterOutputs = cell(1, 8);
for i = 0:7
    bandOutputName = sprintf('bandOutputVector_%02d', i);
    bandFilterOutputs{i+1} = bandFilterOutputData.(bandOutputName);
end

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
plot(f_input_khz, 10*log10(pxx_input), 'b'); % 'b' specifies the color blue
title_str = sprintf('Welch power spectral density estimation (Input) with %d ms segments, fft with %d length', segment_length/fs*1000, nfft);
title(title_str);
xlabel('Frequency (kHz)');
ylabel('Power/Frequency (dB/Hz)');
grid on;
%ylim([-90, -50]);

% Plot the PSD of the output signal
subplot(2, 1, 2);
f_output_khz = f_output / 1000;
plot(f_output_khz, 10*log10(pxx_output), 'r', 'LineWidth', 2); % 'r' specifies the color red
title_str = sprintf('Welch power spectral density estimation (Output) with %d ms segments, fft with %d length', segment_length/fs*1000, nfft);
title(title_str);
xlabel('Frequency (kHz)');
ylabel('Power/Frequency (dB/Hz)');
grid on;
%ylim([-90, -50]);
hold on;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% FIGURE 2: GAMMATONE FILTER BANK TRANSFER FUNCTION %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Plot the PSD of the individual filter outputs
% Create a colormap with a unique color for each filter
colormap = jet(length(individualFilterOutputs));

for i = 1:length(individualFilterOutputs)
    [pxx_filter, f_filter] = pwelch(individualFilterOutputs{i}, segment_length, noverlap, nfft, fs);
    plot(f_filter/1000, 10*log10(pxx_filter), 'Color', colormap(i, :)); % Use unique color from colormap
end

% Calculate the transfer function (output/input)
transfer_function = pxx_output ./ pxx_input;

% Plot the transfer function
figure;
% Change name of whole figure
set(gcf, 'Name', 'GAMMATONE FILTER BANK TRANSFER FUNCTION');
% Use a thicker line for the transfer function
semilogx(f_input, 10*log10(transfer_function), 'k', 'LineWidth', 2); % 'k' specifies the color black, 'LineWidth' sets the thickness
%title_str = sprintf('Gammatone Transfer Function spectral power distribution using Welch with %d ms segments, fft with %d length', segment_length/fs*1000, nfft);
title('Gammatone Filter Bank Transfer Function');
title(title_str);
xlabel('Frequency (Hz)');
ylabel('Magnitude (dB)');
set(gca, 'XTick', [125, 250, 500, 1000, 2000, 4000, 8000, 16000, 20000]); % Set more ticks on x-axis
grid on;
hold on;


% Plot the individual filter transfer functions
for i = 1:length(individualFilterOutputs)
    [pxx_filter, f_filter] = pwelch(individualFilterOutputs{i}, segment_length, noverlap, nfft, fs);
    transfer_function_filter = pxx_filter ./ pxx_input;
    semilogx(f_filter, 10*log10(transfer_function_filter), 'Color', colormap(i, :)); % Use unique color from colormap
end

% Save the last figure to a file
saveas(gcf, 'output/testHL_04.png'); % TODO: Make the working directory configurable as function parameter

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% FIGURE 3: OCTAVE POWER SPECTRAL DENSITY %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Poctave representation of the input signal
figure;
% Change name of whole figure
set(gcf, 'Name', 'INPUT / OUTPUT OCTAVE POWER SPECTRUM');

subplot(3, 1, 1);
freq_limit = fs/2; % Maximum visualization frequency in Hz
poctave(inputSignal, fs, 'FrequencyLimits', [30, freq_limit]);

% Change color of the bars in the plot a posteriori to blue
h = findobj(gca,'Type','Bar');
set(h, 'FaceColor', 'b');
title_str = sprintf('Octave power spectral density estimation, max freq: %d Hz, one band per octave (INPUT)', freq_limit);
title(title_str);

% Poctave representation of the output signal
subplot(3, 1, 2);
poctave(outputSignal, fs, 'FrequencyLimits', [30, freq_limit]);

% Change color of the bars in the plot a posteriori to red
h = findobj(gca,'Type','Bar');
set(h, 'FaceColor', 'r');
title_str = sprintf('Octave power spectral density estimation, max freq: %d Hz, one band per octave (OUTPUT)', freq_limit);
title(title_str);


% Poctave representation of the transfer function
subplot(3, 1, 3);
poctave(transfer_function, fs, 'FrequencyLimits', [30, freq_limit]);
% Change color of the bars in the plot a posteriori to black
h = findobj(gca,'Type','Bar');
set(h, 'FaceColor', 'k');
title_str = sprintf('Octave power spectral density estimation, max freq: %d Hz, one band per octave (TRANSFER F.)', freq_limit);
title(title_str);

% waitfor(gcf);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% FIGURE 4: BAND POWER SPECTRAL DENSITY %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
figure; 
bandcolormap = jet(length(bandFilterOutputs));

% Change name of whole figure
set(gcf, 'Name', 'BAND POWER SPECTRUM');
% Use a thicker line for the transfer function
semilogx(f_input, 10*log10(transfer_function), 'k', 'LineWidth', 2); % 'k' specifies the color black, 'LineWidth' sets the thickness
%title_str = sprintf('Band Transfer Function spectral power distribution using Welch with %d ms segments, fft with %d length', segment_length/fs*1000, nfft);
title_str = sprintf('Band Transfer Function spectral power distribution');
title(title_str);
xlabel('Frequency (Hz)');
ylabel('Magnitude (dB)');
set(gca, 'XTick', [125, 250, 500, 1000, 2000, 4000, 8000, 16000, 20000]); % Set more ticks on x-axis
grid on;
hold on;

% Plot the individual band transfer functions
for i = 1:length(bandFilterOutputs)
    [pxx_band, f_band] = pwelch(bandFilterOutputs{i}, segment_length, noverlap, nfft, fs);
    transfer_function_band = pxx_band ./ pxx_input;
    semilogx(f_band, 10*log10(transfer_function_band), 'Color', bandcolormap(i, :)); % Use unique color from colormap
end

% Add legend to the plot indicating the frequency bands 125, 250, 500, 1000, 2000, 4000, 8000, 16000
legend('Transfer Function', '125 Hz', '250 Hz', '500 Hz', '1000 Hz', '2000 Hz', '4000 Hz', '8000 Hz', '16000 Hz', 'Location', 'SouthWest');

% Save the last figure to a file
saveas(gcf, 'output/testHL_04b.png'); % TODO: Make the working directory configurable as function parameter

end
