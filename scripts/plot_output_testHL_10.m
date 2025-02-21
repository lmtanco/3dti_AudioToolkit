function plot_output_testHL_10()

inputData = open('../build/Debug/testHL-10-input.mat');
outputData1 = open('../build/Debug/testHL-10-output1.mat');
outputData2 = open('../build/Debug/testHL-10-output2.mat');

% Test assumes data is in 'inputVector', 'outputVector1' and 'outputVector2'
inputSignal = inputData.inputVector;
outputSignal1 = outputData1.outputVector1;
outputSignal2 = outputData2.outputVector2;

% Parameters for pwelch
segment_length = round(0.01 * 48000); % Segment length (adjustable, e.g., 100 ms)
noverlap = round(0.5 * segment_length); % Overlap between segments (adjustable)
nfft = 512; % Number of points for the FFT (adjustable)
fs = 48000; % Sampling frequency

% Convert signals to double
inputSignal = double(inputSignal);
outputSignal1 = double(outputSignal1);
outputSignal2 = double(outputSignal2);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% FIGURE 1: POWER SPECTRAL DENSITY USING PWELCH %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Calculate the PSD using pwelch
[pxx_input, f_input] = pwelch(inputSignal, segment_length, noverlap, nfft, fs);
[pxx_output1, f_output1] = pwelch(outputSignal1, segment_length, noverlap, nfft, fs);
[pxx_output2, f_output2] = pwelch(outputSignal2, segment_length, noverlap, nfft, fs);

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

% Plot the PSD of the output signal 1
subplot(3, 1, 2);
f_output1_khz = f_output1 / 1000;
plot(f_output1_khz, 10*log10(pxx_output1), 'r', 'LineWidth', 2); % 'r' specifies the color red
title_str = sprintf('Welch power spectral density estimation (Output 1) with %d ms segments, fft with %d length', segment_length/fs*1000, nfft);
title(title_str);
xlabel('Frequency (kHz)');
ylabel('Power/Frequency (dB/Hz)');
grid on;
%ylim([-90, -50]);

% Plot the PSD of the output signal 2
subplot(3, 1, 3);
f_output2_khz = f_output2 / 1000;
plot(f_output2_khz, 10*log10(pxx_output2), 'g', 'LineWidth', 2); % 'g' specifies the color green
title_str = sprintf('Welch power spectral density estimation (Output 2) with %d ms segments, fft with %d length', segment_length/fs*1000, nfft);
title(title_str);
xlabel('Frequency (kHz)');
ylabel('Power/Frequency (dB/Hz)');
grid on;
%ylim([-90, -50]);

% Calculate the transfer function (output/input)
transfer_function1 = pxx_output1 ./ pxx_input;
transfer_function2 = pxx_output2 ./ pxx_input;


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% FIGURE 2: GAMMATONE TRANSFER FUNCTION %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Plot the transfer function
figure;
% Change name of whole figure
set(gcf, 'Name', 'GAMMATONE FILTER BANK TRANSFER FUNCTION');
% Use a thicker line for the transfer function
plot(f_input_khz, 10*log10(transfer_function1), 'k', 'LineWidth', 2); % 'k' specifies the color black, 'LineWidth' sets the thickness
hold on;
plot(f_input_khz, 10*log10(transfer_function2)+0.1, 'm', 'LineWidth', 2); % 'm' specifies the color magenta, 'LineWidth' sets the thickness
title_str = sprintf('Gammatone Transfer Function spectral power distribution using Welch with %d ms segments, fft with %d length', segment_length/fs*1000, nfft);
title(title_str);
xlabel('Frequency (kHz)');
ylabel('Magnitude (dB)');
grid on;
% Create a legend
legend('With Grouping', 'NO Grouping', 'Location', 'NorthWest');    

% Save the last figure to a file
saveas(gcf, 'output/testHL_10.png'); % TODO: Make the working directory configurable as function parameter

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% FIGURE 3: OCTAVE POWER SPECTRAL DENSITY %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%% Poctave representation of the input signal
%figure;
%% Change name of whole figure
%set(gcf, 'Name', 'INPUT / OUTPUT OCTAVE POWER SPECTRUM');
%
%subplot(3, 1, 1);
%freq_limit = fs/2; % Maximum visualization frequency in Hz
%poctave(inputSignal, fs, 'FrequencyLimits', [30, freq_limit]);
%
%% Change color of the bars in the plot a posteriori to blue
%h = findobj(gca,'Type','Bar');
%set(h, 'FaceColor', 'b');
%title_str = sprintf('Octave power spectral density estimation, max freq: %d Hz, one band per octave (INPUT)', freq_limit);
%title(title_str);
%
%% Poctave representation of the output signal 1
%subplot(3, 1, 2);
%poctave(outputSignal1, fs, 'FrequencyLimits', [30, freq_limit]);
%
%% Change color of the bars in the plot a posteriori to red
%h = findobj(gca,'Type','Bar');
%set(h, 'FaceColor', 'r');
%title_str = sprintf('Octave power spectral density estimation, max freq: %d Hz, one band per octave (OUTPUT 1)', freq_limit);
%title(title_str);
%
%% Poctave representation of the output signal 2
%subplot(3, 1, 3);
%poctave(outputSignal2, fs, 'FrequencyLimits', [30, freq_limit]);
%
%% Change color of the bars in the plot a posteriori to green
%h = findobj(gca,'Type','Bar');
%set(h, 'FaceColor', 'g');
%title_str = sprintf('Octave power spectral density estimation, max freq: %d Hz, one band per octave (OUTPUT 2)', freq_limit);
%title(title_str);
%
%waitfor(gcf);

end
