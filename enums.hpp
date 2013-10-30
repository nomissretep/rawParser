/*
 * enums.hpp
 *
 *  Created on: Aug 30, 2013
 *      Author: speters
 */

#ifndef ENUMS_HPP_
#define ENUMS_HPP_

//#define bit_set(eim,eb) (eim&(1L<<eb))


enum IceDim {
	COL,
	LIN,
	CHA,
	SET,
	ECO,
	PHS,
	REP,
	SEG,
	PAR,
	SLC,
	IDA,
	IDB,
	IDC,
	IDD,
	IDE,
	AVE,
	MAX_ICE_DIM
};

enum EIM_BIT {

	ACQEND = 0, // 0: End
	RTFEEDBACK, // 1: Real-time feedback
	HPFEEDBACK, // 2: HP feedback
	ONLINE, // 3: Handle online
	OFFLINE, // 4: Handle offline
	SYNCDATA, // 5: Synchroneous data

	LASTSCANINCONCAT = 8, // 8: Last scan in concat

	RAWDATACORRECTION = 10, // 10: Correct raw data with correction factor
	LASTSCANINMEAS, // 11: Last scan in measurement
	SCANSCALEFACTOR, // 12: Specific additional scale factor
	SECONDHADAMARPULSE, // 13: 2nd RF excitation of HADAMAR
	REFPHASESTABSCAN, // 14: Reference phase stabilisation scan
	PHASESTABSCAN, // 15: Phase stabilisation scan
	D3FFT, // 16: Subject to 3D FFT
	SIGNREV, // 17: Sign reversal
	PHASEFFT, // 18: Perform PE FFT
	SWAPPED, // 19: Swapped phase/readout direction
	POSTSHAREDLINE, // 20: Shared line
	PHASCOR, // 21: Phase correction data
	PATREFSCAN, // 22: PAT reference data
	PATREFANDIMASCAN, // 23: PAT reference and imaging data
	REFLECT, // 24: Reflect line
	NOISEADJSCAN, // 25: Noise adjust scan
	SHARENOW, // 26: All lines are acquired from the actual and previous e.g. phases
	LASTMEASUREDLINE, // 27: Last measured line of all succeeding e.g. phases
	FIRSTSCANINSLICE, // 28: First scan in slice (needed for time stamps)
	LASTSCANINSLICE, // 29: Last scan in slice ( " " )
	TREFFECTIVEBEGIN, // 30: Begin time stamp for TReff (triggered measurement)
	TREFFECTIVEEND // 31: End time stamp for TReff (triggered measurement)

};

enum meas_mode {
	TSE, ADJ_MDS_FRE, ADJ_FRE_MDS, ADJ_FRE, DRY, EPI, UTE
};

enum alloc_mode {
	SCAN_MEAS, SCAN_PROT
};

#endif /* ENUMS_HPP_ */
