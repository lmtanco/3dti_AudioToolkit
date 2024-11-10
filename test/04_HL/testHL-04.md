# testHL-04.cpp

[testHL-04.cpp](./testHL-04.cpp) creates a Multiband Expander with parameters: 
* samplingRate = 48000
* iniFreq_Hz = 250 
* bandsNumber = 8
* filterGrouping = true

This is done with these two lines: 
  

```cpp
HAHLSimulation::CGammatoneMultibandExpander expander;
expander.Setup(48000, 250, 8, true);
```

This creates a Multiband Expander with 8 bands at 250, 500, 1000, 2000, 4000, 8000, 16000 and 32000.

Internally, in the expander `Setup` method, the GammatoneFilter bank is initialised with harcoded parameters:

```cpp
CGammatoneMultibandExpander::Setup(int samplingRate, float iniFreq_Hz, int bandsNumber, bool filterGrouping){
//...
gammatoneFilterBank.InitWithFreqRangeOverlap(20, 20000, 0.0, Common::CGammatoneFilterBank::EAR_MODEL_DEFAULT);
//...
}
```

The `EAR_MODEL_DEFAULT` corresponds to an `EAR_MODEL_GLASBERG`., which provides values for parameters `q=9.26449 `and `minbw=24.7`. This parameters are in turn used to calculate the number of filters in the bank:

$$
\text{num\_filters} = \frac{q \cdot (\ln(highFreq + q \cdot minbw)-\ln(lowFreq + q \cdot minbw))} {\text{stepfactor}}
$$

This gives `num_filters=41`, but the total number of Gammatones is for some reason an extra one: 42. Each of the gammatone filters is created and added to the bank:

```cpp
for(int i = 0; i <= num_filters; i++)
{
	double denominator = exp(i * stepfactor / ear_q);
	double center_freq = -ear_q_min_bw;
	center_freq += (_highFreq + ear_q_min_bw) / denominator;
	double bandwidth = GetERBOfHumanAuditoryFilter(center_freq, _earModel);
	shared_ptr<Common::CGammatoneFilter> filter = AddFilter(GAMMATONE_FILTER_ORDER, center_freq, bandwidth);
	//...
```

This gives the following center frequencies and bandwithds for the gammatone.

| filter | center frequency | bandwith |
| :----: | :--------------: | :-------: |
|   0   |      20000      | 2183.480 |
|   1   |    17930.233    | 1960.0718 |
|   2   |    16072.239    | 1759.521 |
|   3   |    14404.352    | 1579.491 |
|   4   |    12907.119    | 1417.881 |
|  ...  |       ...       |    ...    |
|   39   |      71.605      |  32.429  |
|   40   |      40.865      |  29.111  |
|   41   |      13.270      |  26.132  |

To access the individual gammatones we created a version of the toolkit that gives access to the internal bank: 

```cpp
    Common::CGammatoneFilterBank &filterBank = expander.GetGammatoneFilterBank();
```

Then we can process the input using each gammatone individually: 

```cpp
    auto numFilters = filterBank.GetNumFilters();
    std::vector<CMonoBuffer<float>> filterOutputs;
    for (auto i = 0; i < numFilters; i++)
    {
        CMonoBuffer<float> filterOutput(inputBuffer.size());
        auto filter = filterBank.GetFilter(i);
        filter->Process(inputBuffer, filterOutput);
        filterOutputs.push_back(filterOutput);
    }
```