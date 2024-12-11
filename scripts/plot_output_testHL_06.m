function plot_output_testHL_06()

HL_DBS_SPL_FOR_0_DBS_FS=100;

inputData = open('../build/Debug/testHL-06-input.mat');
outputData = open('../build/Debug/testHL-06-output.mat');
individualFilterOutputData = open('../build/Debug/testHL-06-filterOutputs.mat');
bandIndicesData = open('../build/Debug/testHL-06-bandIndices.mat');
stereoOutputsData = open('../build/Debug/testHL-06-stereoOutputs.mat');

% Test assumes data is in 'inputVector' y 'outputVector'
inputSignal = inputData.inputVector;
outputSignal = outputData.outputVector;

% Load hearing loss data
inputSignal_x05 = stereoOutputsData.input_x05;
inputSignal_x1 = stereoOutputsData.input_x1;
inputSignal_x2 = stereoOutputsData.input_x2;
inputSignal_x4 = stereoOutputsData.input_x4;
inputSignal_x8 = stereoOutputsData.input_x8;
hearingLossLeftSignal_x05 = stereoOutputsData.hearingLoss_x05_left;
hearingLossRightSignal_x05 = stereoOutputsData.hearingLoss_x05_right;
hearingLossLeftSignal = stereoOutputsData.hearingLoss_x1_left;
hearingLossRightSignal = stereoOutputsData.hearingLoss_x1_right;
hearingLossLeftSignal_x2 = stereoOutputsData.hearingLoss_x2_left;
hearingLossRightSignal_x2 = stereoOutputsData.hearingLoss_x2_right;
hearingLossLeftSignal_x4 = stereoOutputsData.hearingLoss_x4_left;
hearingLossRightSignal_x4 = stereoOutputsData.hearingLoss_x4_right;
hearingLossLeftSignal_x8 = stereoOutputsData.hearingLoss_x8_left;
hearingLossRightSignal_x8 = stereoOutputsData.hearingLoss_x8_right;

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
inputSignal_x05 = double(inputSignal_x05);
inputSignal_x1 = double(inputSignal_x1);
inputSignal_x2 = double(inputSignal_x2);
inputSignal_x4 = double(inputSignal_x4);
inputSignal_x8 = double(inputSignal_x8);
hearingLossLeftSignal_x05 = double(hearingLossLeftSignal_x05);
hearingLossRightSignal_x05 = double(hearingLossRightSignal_x05);
hearingLossLeftSignal = double(hearingLossLeftSignal);
hearingLossRightSignal = double(hearingLossRightSignal);
hearingLossLeftSignal_x2 = double(hearingLossLeftSignal_x2);
hearingLossRightSignal_x2 = double(hearingLossRightSignal_x2);
hearingLossLeftSignal_x4 = double(hearingLossLeftSignal_x4);
hearingLossRightSignal_x4 = double(hearingLossRightSignal_x4);
hearingLossLeftSignal_x8 = double(hearingLossLeftSignal_x8);
hearingLossRightSignal_x8 = double(hearingLossRightSignal_x8);

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


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% FIGURE 3: HEARING LOSS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
figure;
% Change name of whole figure
set(gcf, 'Name', 'HEARING LOSS SIMULATION (RIGHT EAR 50 dB LOSS in bands 4 and 5)');
% Calculate power spectral density using pwelch
[pxx_hearingLossLeft_x05, f_hearingLossLeft_x05] = pwelch(hearingLossLeftSignal_x05, segment_length, noverlap, nfft, fs);
[pxx_hearingLossRight_x05, f_hearingLossRight_x05] = pwelch(hearingLossRightSignal_x05, segment_length, noverlap, nfft, fs);
[pxx_hearingLossLeft, f_hearingLossLeft] = pwelch(hearingLossLeftSignal, segment_length, noverlap, nfft, fs);
[pxx_hearingLossRight, f_hearingLossRight] = pwelch(hearingLossRightSignal, segment_length, noverlap, nfft, fs);
[pxx_hearingLossLeft_x2, f_hearingLossLeft_x2] = pwelch(hearingLossLeftSignal_x2, segment_length, noverlap, nfft, fs);
[pxx_hearingLossRight_x2, f_hearingLossRight_x2] = pwelch(hearingLossRightSignal_x2, segment_length, noverlap, nfft, fs);
[pxx_hearingLossLeft_x4, f_hearingLossLeft_x4] = pwelch(hearingLossLeftSignal_x4, segment_length, noverlap, nfft, fs);
[pxx_hearingLossRight_x4, f_hearingLossRight_x4] = pwelch(hearingLossRightSignal_x4, segment_length, noverlap, nfft, fs);
[pxx_hearingLossLeft_x8, f_hearingLossLeft_x8] = pwelch(hearingLossLeftSignal_x8, segment_length, noverlap, nfft, fs);
[pxx_hearingLossRight_x8, f_hearingLossRight_x8] = pwelch(hearingLossRightSignal_x8, segment_length, noverlap, nfft, fs);

% Calculate power spectral density of the input signals
[pxx_input_x05, f_input_x05] = pwelch(inputSignal_x05, segment_length, noverlap, nfft, fs);
[pxx_input_x1, f_input_x1] = pwelch(inputSignal_x1, segment_length, noverlap, nfft, fs);
[pxx_input_x2, f_input_x2] = pwelch(inputSignal_x2, segment_length, noverlap, nfft, fs);
[pxx_input_x4, f_input_x4] = pwelch(inputSignal_x4, segment_length, noverlap, nfft, fs);
[pxx_input_x8, f_input_x8] = pwelch(inputSignal_x8, segment_length, noverlap, nfft, fs);

% Calculate transfer function of each ear
transfer_function_hearingLossLeft_x05 = pxx_hearingLossLeft_x05 ./ pxx_input_x05;
transfer_function_hearingLossRight_x05 = pxx_hearingLossRight_x05 ./ pxx_input_x05;
transfer_function_hearingLossLeft = pxx_hearingLossLeft ./ pxx_input_x1;
transfer_function_hearingLossRight = pxx_hearingLossRight ./ pxx_input_x1;
transfer_function_hearingLossLeft_x2 = pxx_hearingLossLeft_x2 ./ pxx_input_x2;
transfer_function_hearingLossRight_x2 = pxx_hearingLossRight_x2 ./ pxx_input_x2; 
transfer_function_hearingLossLeft_x4 = pxx_hearingLossLeft_x4 ./ pxx_input_x4;
transfer_function_hearingLossRight_x4 = pxx_hearingLossRight_x4 ./ pxx_input_x4;
transfer_function_hearingLossLeft_x8 = pxx_hearingLossLeft_x8 ./ pxx_input_x8;
transfer_function_hearingLossRight_x8 = pxx_hearingLossRight_x8 ./ pxx_input_x8;

% Plot the input signal in the time domain and the transfer function of the right ear
subplot(2, 2, 1);
time_axis = (0:length(inputSignal)-1)/fs;
hold on;

plot(time_axis, inputSignal_x8, 'k', 'LineWidth', 1);
plot(time_axis, inputSignal_x4, 'b', 'LineWidth', 1);
plot(time_axis, inputSignal_x2, 'g', 'LineWidth', 1);
plot(time_axis, inputSignal_x1, 'r', 'LineWidth', 1);
plot(time_axis, inputSignal_x05, 'm', 'LineWidth', 1);

title('Input signal in time domain');
xlabel('Time (s)');
ylabel('Amplitude');
ylim([-1, 1]); % % Fix vertical limit to [1,-1]
%yticks([0, 20, 40, 60, 80, 100]);
grid on;
legend('Input x8', 'Input x4', 'Input x2', 'Input x1', 'Input x0.5');

% Add envelope of signal in DB in figure
% Calculate the envelope of the input signal
subplot(2,2,2);
inputSignalEnvelope_x05 = envelope(inputSignal_x05, 150,'rms');
inputSignalEnvelope_x05_DB = 20*log10(abs(inputSignalEnvelope_x05)) + HL_DBS_SPL_FOR_0_DBS_FS;
inputSignalEnvelope_x1 = envelope(inputSignal_x1, 150,'rms');
inputSignalEnvelope_x1_DB = 20*log10(abs(inputSignalEnvelope_x1)) + HL_DBS_SPL_FOR_0_DBS_FS;
inputSignalEnvelope_x2 = envelope(inputSignal_x2, 150,'rms');
inputSignalEnvelope_x2_DB = 20*log10(abs(inputSignalEnvelope_x2)) + HL_DBS_SPL_FOR_0_DBS_FS;
inputSignalEnvelope_x4 = envelope(inputSignal_x4, 150,'rms');
inputSignalEnvelope_x4_DB = 20*log10(abs(inputSignalEnvelope_x4)) + HL_DBS_SPL_FOR_0_DBS_FS;
inputSignalEnvelope_x8 = envelope(inputSignal_x8, 150,'rms');
inputSignalEnvelope_x8_DB = 20*log10(abs(inputSignalEnvelope_x8)) + HL_DBS_SPL_FOR_0_DBS_FS;
plot(time_axis, inputSignalEnvelope_x05_DB, 'm', 'LineWidth', 1);
hold on;
plot(time_axis, inputSignalEnvelope_x1_DB, 'r', 'LineWidth', 1);
plot(time_axis, inputSignalEnvelope_x2_DB, 'g', 'LineWidth', 1);
plot(time_axis, inputSignalEnvelope_x4_DB, 'b', 'LineWidth', 1);
plot(time_axis, inputSignalEnvelope_x8_DB, 'k', 'LineWidth', 1);
title('Envelope of the input signals in time domain with HL-DBS-SPL-FOR-0-DBS-FS = 100');
xlabel('Time (s)');
ylabel('Amplitude (dB)');
ylim([0, 100]); % % Fix vertical limit to [0, 100]
%yticks([0, 20, 40, 60, 80, 100]);
grid on;
legend('Input x0.5', 'Input x1', 'Input x2', 'Input x4', 'Input x8');

subplot(2,2,3);


semilogx(f_input_khz, 10*log10(transfer_function_hearingLossLeft), 'r', 'LineWidth', 2); % 'r' specifies the color red
hold on;
semilogx(f_input_khz, 10*log10(transfer_function_hearingLossLeft_x05), 'm', 'LineWidth', 2); % 'm' specifies the color magenta
semilogx(f_input_khz, 10*log10(transfer_function_hearingLossLeft_x2), 'g', 'LineWidth', 2); % 'g' specifies the color green
semilogx(f_input_khz, 10*log10(transfer_function_hearingLossLeft_x4), 'b', 'LineWidth', 2); % 'b' specifies the color blue
semilogx(f_input_khz, 10*log10(transfer_function_hearingLossLeft_x8), 'k', 'LineWidth', 2); % 'k' specifies the color black
title('Hearing Loss Left Ear (no loss)');
xlabel('Frequency (kHz)');
ylabel('Magnitude (dB)');
set(gca, 'XTick', [0.125, 0.25, 0.5, 1, 2, 4, 8, 16]);
xlim([0, fs/2]/1000);
grid on;

% Represent again the octave bands at 125, 250, 500, 1000, 2000, 4000, 8000 and 16000 Hz using the patch function
% The patch function is used to represent the octave bands in the plot
% figure;
% hold on;
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

subplot(2, 2, 4);

semilogx(f_input_khz, 10*log10(transfer_function_hearingLossRight), 'r', 'LineWidth', 2); % 'r' specifies the color red
hold on;
semilogx(f_input_khz, 10*log10(transfer_function_hearingLossRight_x05), 'm', 'LineWidth', 2); % 'm' specifies the color magenta
semilogx(f_input_khz, 10*log10(transfer_function_hearingLossRight_x2), 'g', 'LineWidth', 2); % 'g' specifies the color green
semilogx(f_input_khz, 10*log10(transfer_function_hearingLossRight_x4), 'b', 'LineWidth', 2); % 'b' specifies the color blue
semilogx(f_input_khz, 10*log10(transfer_function_hearingLossRight_x8), 'k', 'LineWidth', 2); % 'k' specifies the color black
title('Hearing Loss Right Ear(50 dB loss in bands 4 and 5)');
xlabel('Frequency (kHz)');
ylabel('Magnitude (dB)');
set(gca, 'XTick', [0.125, 0.25, 0.5, 1, 2, 4, 8, 16]);
xlim([0, fs/2]/1000);
grid on;

% Represent again the octave bands at 125, 250, 500, 1000, 2000, 4000, 8000 and 16000 Hz using the patch function
% The patch function is used to represent the octave bands in the plot
% figure;
% hold on;
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


waitfor(gcf);

end