function plot_output_testHL_03()

inputData = open('../build/Debug/testHL-04-input.mat');
outputData = open('../build/Debug/testHL-04-output.mat');
individualFilterOutputData = open('../build/Debug/testHL-04-filterOutputs.mat');

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
subplot(3, 1, 1);
f_input_khz = f_input / 1000;
plot(f_input_khz, 10*log10(pxx_input), 'b'); % 'b' specifies the color blue
title_str = sprintf('Welch power spectral density estimation (Input) with %d ms segments, fft with %d length', segment_length/fs*1000, nfft);
title(title_str);
xlabel('Frequency (kHz)');
ylabel('Power/Frequency (dB/Hz)');
grid on;
%ylim([-90, -50]);

% Plot the PSD of the output signal
subplot(3, 1, 2);
f_output_khz = f_output / 1000;
plot(f_output_khz, 10*log10(pxx_output), 'r', 'LineWidth', 2); % 'r' specifies the color red
title_str = sprintf('Welch power spectral density estimation (Output) with %d ms segments, fft with %d length', segment_length/fs*1000, nfft);
title(title_str);
xlabel('Frequency (kHz)');
ylabel('Power/Frequency (dB/Hz)');
grid on;
%ylim([-90, -50]);
hold on;

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
subplot(3, 1, 3);
% Use a thicker line for the transfer function
plot(f_input_khz, 10*log10(transfer_function), 'k', 'LineWidth', 2); % 'k' specifies the color black, 'LineWidth' sets the thickness
title('Transfer Function (Output/Input)');
xlabel('Frequency (kHz)');
ylabel('Magnitude (dB)');
grid on;
hold on;

% Plot the individual filter transfer functions
for i = 1:length(individualFilterOutputs)
    [pxx_filter, f_filter] = pwelch(individualFilterOutputs{i}, segment_length, noverlap, nfft, fs);
    transfer_function_filter = pxx_filter ./ pxx_input;
    plot(f_filter/1000, 10*log10(transfer_function_filter), 'Color', colormap(i, :)); % Use unique color from colormap
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% FIGURE 2: OCTAVE POWER SPECTRAL DENSITY %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

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

waitfor(gcf);

end
