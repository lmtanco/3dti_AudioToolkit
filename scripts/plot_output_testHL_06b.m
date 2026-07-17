function plot_output_testHL_06b()
% Forensic comparison (issue #17 et al.): octave-aligned band limits ("oct")
% vs the band limits used in the validation experiment ("exp"), with the
% test-06 audiometry: L = no loss, R = {0,0,0,50,50,0,0,0}.

fs = 48000;
cfgData = open('../build/Debug/testHL-06b-bandConfig.mat');
outData = open('../build/Debug/testHL-06b-stereoOutputs.mat');

% Print the configured expander parameters for both configurations
printConfig(cfgData, 'oct', 'OCTAVE-ALIGNED band limits (no index mismatch)');
printConfig(cfgData, 'exp', 'EXPERIMENT band limits (band-index mismatch)');

% Parameters for pwelch
segment_length = round(0.01 * fs);
noverlap = round(0.5 * segment_length);
nfft = 512;

gains = {'x05','x1','x2','x4','x8'};
gainColors = {'m','r','g','b','k'};
ears = {'left','right'};
cfgs = {'oct','exp'};
cfgTitles = {'octave-aligned limits','experiment limits'};

figure('Name', 'TF comparison: octave-aligned vs experiment band limits');
plotIdx = 0;
for e = 1:2
    for c = 1:2
        plotIdx = plotIdx + 1;
        subplot(2, 2, plotIdx);
        hold on;
        for g = 1:numel(gains)
            in  = double(outData.(['input_' gains{g}]));
            out = double(outData.(['hearingLoss_' gains{g} '_' ears{e} '_' cfgs{c}]));
            [pin, f]  = pwelch(in,  segment_length, noverlap, nfft, fs);
            [pout, ~] = pwelch(out, segment_length, noverlap, nfft, fs);
            plot(f/1000, 10*log10(pout ./ pin), gainColors{g}, 'LineWidth', 1.5);
        end
        set(gca, 'XScale', 'log');
        set(gca, 'XTick', [0.125, 0.25, 0.5, 1, 2, 4, 8, 16]);
        xlim([0.02, fs/2/1000]);
        grid on;
        title(sprintf('%s ear - %s', ears{e}, cfgTitles{c}));
        xlabel('Frequency (kHz)');
        ylabel('Magnitude (dB)');
        if plotIdx == 1
            legend(gains, 'Location', 'southwest');
        end
        drawGroupBands(cfgData, cfgs{c}, fs);
    end
end

waitfor(gcf);
end

function printConfig(d, suffix, titleStr)
gc = double(d.(['groupBandCenters_left_' suffix]));
rl = double(d.(['groupRatios_left_' suffix]));
tl = double(d.(['groupThresholds_left_' suffix]));
gl = double(d.(['groupGainsDB_left_' suffix]));
rr = double(d.(['groupRatios_right_' suffix]));
tr = double(d.(['groupThresholds_right_' suffix]));
gr = double(d.(['groupGainsDB_right_' suffix]));
af = double(d.(['audiometryFrequencies_' suffix]));
av_l = double(d.audiometryValues_left);
av_r = double(d.audiometryValues_right);
fprintf('\n=== %s ===\n', titleStr);
fprintf('%11s %5s %5s | %5s %12s | %8s %12s %10s | %8s %12s %10s\n', ...
    'audiom(Hz)', 'audL', 'audR', 'group', 'center(Hz)', 'ratioL', 'thrL(dBFS)', 'gainL(dB)', 'ratioR', 'thrR(dBFS)', 'gainR(dB)');
for i = 1:numel(gc)
    fprintf('%11.0f %5.0f %5.0f | %5d %12.1f | %8.3f %12.2f %10.2f | %8.3f %12.2f %10.2f\n', ...
        af(i), av_l(i), av_r(i), i, gc(i), rl(i), tl(i), gl(i), rr(i), tr(i), gr(i));
end
fprintf('Octave bands (Hz):          '); fprintf('%8.0f', double(d.octaveBandFrequencies)); fprintf('\n');
fprintf('Octave attenuations L (dB): '); fprintf('%8.2f', double(d.(['octaveAttenuations_left_' suffix]))); fprintf('\n');
fprintf('Octave attenuations R (dB): '); fprintf('%8.2f', double(d.(['octaveAttenuations_right_' suffix]))); fprintf('\n');
end

function drawGroupBands(d, suffix, fs)
limits = double(d.(['bandLimits_' suffix]));
centers = double(d.(['groupBandCenters_left_' suffix]));
edges = [20, limits(:)', fs/2] / 1000;
yl = ylim;
colors = turbo(numel(centers));
for i = 1:numel(centers)
    x = [edges(i), edges(i+1), edges(i+1), edges(i)];
    y = [yl(1), yl(1), yl(2), yl(2)];
    patch('XData', x, 'YData', y, 'FaceColor', colors(i,:), 'FaceAlpha', 0.15, ...
        'EdgeColor', 'none', 'HandleVisibility', 'off');
    text(sqrt(edges(i)*edges(i+1)), yl(1) + 2, sprintf('%.0f', centers(i)), ...
        'FontSize', 7, 'HorizontalAlignment', 'center');
end
% Octave band centers (audiometry index space) as dotted lines
octaves = double(d.octaveBandFrequencies) / 1000;
for i = 1:numel(octaves)
    xline(octaves(i), ':k', 'HandleVisibility', 'off');
end
end
