/**
 * @file testHL.cpp
 * @author Luis Molina-Tanco (lmtanco@uma.esº)
 * @brief 
 * @version 0.1
 * @date 2024-10-06
 * 
 * @copyright Copyright (c) 2024 Universidad de Málaga
 * 
 */

#include <HAHLSimulation/HearingLossSim.h>
#include <HAHLSimulation/GammatoneMultibandExpander.h>  

// 
auto calibration_dBs_SPL_for_minus_20_dBs_fs = 72;

int main(int argc, char const *argv[])
{
    HAHLSimulation::CHearingLossSim hlSim;
    Common::CEarPair<shared_ptr<HAHLSimulation::CGammatoneMultibandExpander>> gammatoneMultibandExpanders;
    return 0;
}