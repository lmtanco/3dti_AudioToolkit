function plot_output_testHL_02()

% Assuming a sampling frequency of 48000 Hz 
fs = 48000;

inputData = open('../build/Debug/testHL-02-input.mat');
outputData = open('../build/Debug/testHL-02-output.mat');

% Test assumes datais in 'inputVector' y 'outputVector'
inputSignal = inputData.inputVector;
outputSignal = outputData.outputVector;

% Represent time domain signals
figure;
set(gcf, 'Name', 'INPUT / OUTPUT SIGNALS IN TIME DOMAIN');
time_axis = (0:length(inputSignal)-1)/fs;

subplot(2, 1, 1);
plot(time_axis, inputSignal);
title('Input signal in time domain');
xlabel('Time (s)');
ylabel('Amplitude');
ylim([-1, 1]); % % Fix vertical limit to [1,-1]

subplot(2, 1, 2);
plot(time_axis, outputSignal, 'r');
title('Output signal in time domain');
xlabel('Time (s)');
ylabel('Amplitude');
% % Fix vertical limit to [1,-1]
ylim([-1, 1]);

% Plot input signal in the frequency domain using pwelch
figure;
% Change name of whole figure
set(gcf, 'Name', 'INPUT POWER SPECTRUM');

% Pwelch representation of the input signal
segment_length = round(0.01 * fs); % Segment length (adjustable, e.g., 100 ms)
noverlap = round(0.5 * segment_length); % Overlap between segments (adjustable)
nfft = 512; % Number of points for the FFT (adjustable)
subplot(2, 1, 1);
inputSignal = double(inputSignal);
pwelch(inputSignal, segment_length, noverlap, nfft, fs);
title_str = sprintf('Welch power spectral density estimation with %d ms segments, fft with %d length', segment_length/fs*1000, nfft);
title(title_str);
grid on;

% Poctave representation of the input signal
subplot(2, 1, 2);
freq_limit = fs/2; % Maximum visualization frequency in Hz
poctave(inputSignal, fs, 'FrequencyLimits', [30, freq_limit]);
title_str = sprintf('Octave power spectral density estimation, max freq: %d Hz, one band per octave', freq_limit);
title(title_str);

% Plot output signal in the frequency domain using pwelch
figure;
% Change name of whole figure 
set(gcf, 'Name', 'OUTPUT POWER SPECTRUM');
subplot(2, 1, 1);
outputSignal = double(outputSignal);
pwelch(outputSignal, segment_length, noverlap, nfft, fs);
title_str = sprintf('Welch power spectral density estimation with %d ms segments, fft with %d length', segment_length/fs*1000, nfft);
title(title_str);
grid on;
% Use same limits as input signal for vertical axis
ylim([-90, -50]);
h = findobj(gca,'Type','Line');
set(h, 'Color', 'r');

% Poctave representation of the output signal
subplot(2, 1, 2);
freq_limit = fs/2; % Maximum visualization frequency in Hz
poctave(outputSignal, fs, 'FrequencyLimits', [30, freq_limit]);

% Change color of the bars in the plot a posteriori to red
h = findobj(gca,'Type','Bar');
set(h, 'FaceColor', 'r');
title_str = sprintf('Octave power spectral density estimation, max freq: %d Hz, one band per octave', freq_limit);
title(title_str);
waitfor(gcf);

end
