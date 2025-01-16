# Summary of what each test does

Each test creates output files that can be read later by the corresponding matlab scripts. For example, [testHL-06.cpp](./testHL-06.cpp) outputs files that can be read by [plot_output_testHL_06.m](../../scripts/plot_output_testHL_06.m). 


## Gammatone tests

* [testHL-01.cpp](./testHL-01.cpp): loads a 4 second sweep and process it with a gammatone filter bank. The filter bank is grouped using group limits: ``{353.5533906f, 707.1067812f, 1414.213562f, 2449.489743f, 3464.101615f, 4898.979486f, 6928.20323f};``, which correspond to the separations between audiometry bands ``{250, 500, 1000, 2000, 3000, 4000, 6000, 8000}``
 
* [testHL-02.cpp](./testHL-02.cpp): loads a 4 second pink noise sample and processes it with a gammatone filter bank. 
  
* [testHL-03.cpp](./testHL-03.cpp): apparently equal to thestHL-02

* [testHL-04.cpp](./testHL-04.cpp): also equal to testHL-02 but outputs the individual processing of each filter in the bank.  There is more explanation of this test in [testHL-04.md](./testHL-04.md).

* [testHL-05.cpp](./testHL-05.cpp): equal to testHL-04.cpp but: 
  * uses different groupings (octave bands this time), where group limits are ``{176.776695296637f, 353.553390593274f,	707.106781186548f,	1414.21356237310f,	2828.42712474619f,	5656.85424949238f,	11313.7084989848f}``
  * outputs also information allowing to represent the bands later in matlab

* [testHL-08.cpp](./testHL-08.cpp): work in progress to test the new implementation of the gammatone filter bank, tries to get the same output as testHL-04.cpp but with the new, simpler class. This first test just tests the validity of the Setup function overloads. 

## Hearing Loss simulation tests

* [testHL-06.cpp](./testHL-06.cpp): this test creates a HearingLoss simulation object which use a Gammatone Multiband expander object for each ear. To the left ear we pass an audiometry test reflecting no loss, while to the right ear we pass an audiometry test reflecting a medium loss for two bands.  
```cpp
    // This are audiometry values for frequencies {250, 500, 1000, 2000, 3000, 4000, 6000, 8000}
    std::vector<float> audiometryCentralLoss =      {0,   0,    0,   50,   50,    0,    0,    0};
    hearingLossSim.SetFromAudiometry_dBHL(Common::T_ear::RIGHT, audiometryCentralLoss);

``` 
* The bands are the same as in testHL-01.cpp. The hearing loss object is used to process five inputs which have increasing power which are processed by the hearing loss simulator, to compare the outputs.  

* [testHL-07.cpp](./testHL-07.cpp): work in progress. 