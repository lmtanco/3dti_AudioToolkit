function plot_output_testHL_03()

inputData = open('../build/Debug/testHL-03-input.mat');
outputData = open('../build/Debug/testHL-03-output.mat');

% Test assumes data is in 'inputVector' y 'outputVector'
inputSignal = inputData.inputVector;
outputSignal = outputData.outputVector;

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
plot(f_input, 10*log10(pxx_input), 'b'); % 'b' specifies the color blue
title_str = sprintf('Welch power spectral density estimation (Input) with %d ms segments, fft with %d length', segment_length/fs*1000, nfft);
title(title_str);
xlabel('Frequency (Hz)');
ylabel('Power/Frequency (dB/Hz)');
grid on;
ylim([-90, -50]);

% Plot the PSD of the output signal
subplot(3, 1, 2);
plot(f_output, 10*log10(pxx_output), 'r'); % 'r' specifies the color red
title_str = sprintf('Welch power spectral density estimation (Output) with %d ms segments, fft with %d length', segment_length/fs*1000, nfft);
title(title_str);
xlabel('Frequency (Hz)');
ylabel('Power/Frequency (dB/Hz)');
grid on;
ylim([-90, -50]);

% Calculate the transfer function (output/input)
transfer_function = pxx_output ./ pxx_input;

% Plot the transfer function
subplot(3, 1, 3);
plot(f_input, 10*log10(transfer_function), 'k'); % 'k' specifies the color black
title('Transfer Function (Output/Input)');
xlabel('Frequency (Hz)');
ylabel('Magnitude (dB)');
grid on;

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
