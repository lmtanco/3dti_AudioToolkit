% Generate pink noise and save it to a wav file.
function gen_pink_noise(fprefix, fs, duration, power_per_octave)

% process input parameters. 
fprintf("File name prefix: %s\n", fprefix)
fprintf("Sampling freq: %d\n", fs)
fprintf("Duration: %d\n", duration)
fname_complete = sprintf("%s_%d_%d_sec_%d.wav", fprefix, fs, duration, power_per_octave);
fprintf("File name is: %s\n", fname_complete)

% Generate mono pink noise
pn = pinknoise(fs*duration);

% Apply the power per octave
pn = pn * power_per_octave;

% Save the pink noise to a wav file
audiowrite(fname_complete, pn, 48000);

% Plot the pink noise 
figure;

% Use pwelch to calculate and plot the power spectral density
subplot(2, 1, 1);
segment_length = round(0.01 * fs); % Segment length (adjustable, e.g., 100 ms)
noverlap = round(0.5 * segment_length); % Overlap between segments (adjustable)
nfft = 512; % Number of points for the FFT (adjustable)
pwelch(pn, segment_length, noverlap, nfft, fs);
title_str = sprintf('Frequency response using %d ms segments, fft with %d length', segment_length/fs*1000, nfft);
title(title_str);
grid on;

% Plot the octave power using poctave
subplot(2, 1, 2);
freq_limit = fs/2; % Maximum visualization frequency in Hz
poctave(pn, fs, 'FrequencyLimits', [30, freq_limit]);
%title('Respuesta en Frecuencia Promediada en octavas');
%legend('Ruido rosa')
grid on;

fprintf("Done!\n");

end 














