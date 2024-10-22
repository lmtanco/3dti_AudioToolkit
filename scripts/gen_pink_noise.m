% Generate pink noise and save it to a wav file.
function gen_pink_noise(fprefix, fs, duration)

% process input parameters. 
fprintf("File name prefix: %s\n", fprefix)
fprintf("Sampling freq: %d\n", fs)
fprintf("Duration: %d\n", duration)
fname_complete = sprintf("%s_%d_%d_sec.wav", fprefix, fs, duration);
fprintf("File name is: %s\n", fname_complete)

% Generate mono pink noise
pn = pinknoise(fs*duration);

% Calculate the fft of pink noise
pn_fft = fft(pn);

% Calculate pn_fft to the minus one power
pn_fft_inv = 1./pn_fft;

% Output the dimensions of pn_fft and pn_fft_inv
size(pn_fft)
size(pn_fft_inv)

% Save the pink noise to a wav file
audiowrite(fname_complete, pn, 48000);

fprintf("Done!\n");

end 














