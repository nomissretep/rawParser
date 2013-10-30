/*
 * rawParser.h
 *
 *  Created on: Aug 30, 2013
 *      Author: speters
 */



#ifndef RAWPARSER_H_
#define RAWPARSER_H_
#include <typeinfo>


#ifdef _OPENMP
//#define USE_OMP1
//#define USE_OMP
#include <omp.h>
#endif


#include "enums.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <complex>
#include <string>
#include "error_code.hpp"
#include "sMDH.h"
#include "Data.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "rawParser.h"
//#include "H5Cpp.h"
#include "inline_f.h"
using namespace std;
#ifndef H5_NO_NAMESPACE
//using namespace H5;
#endif



template<class T>
class rawParser {
public:
	rawParser(const std::string& fname); //Oeffnet den File
	virtual ~rawParser();

	//error_code Save(std::string output);
	error_code Read(); //Liest die Datei in den RAM


private:
	error_code CalcSize(); //kalkoliert die Grosse der Daten

	error_code ReadPFile();
	error_code ExtractProtocol(); //Extrahiert informationen aus dem Protocol
	error_code ParseProtocol (const bool verbose = true, const bool dry = false);

	error_code ReadMFile();

	error_code preParseMeas(const bool verbose = true);
	error_code preParseMDH (const bool verbose = true);

	error_code AllocateMatrices (const bool verbose = true);

	error_code ParseScan (Data<T> matrix,const size_t& c);
	error_code ParseMDH (const bool verbose = true);
	error_code ParseMeas(const bool verbose = true);

	error_code CleanUp(const bool verbose = true);


	std::ifstream m_ifs; //File der Ausgelsenden Datei
	error_code m_status; //Fehler Code
	size_t m_size; //Groesse der Gesammten Datei
	size_t m_dataSize; //Groesse der Messdaten;
	size_t m_fod; //Grosse des vorderen Protocol
	bool m_acqend;//sind die messdaten zuende
	char* m_pdata; //Protokoll Daten
	char* m_mdata; //Messdaten Daten
	bool m_palloc;//protocoll allociert
	bool m_malloc;//messdaten allociert
	std::string m_xprot;
	sMDH m_cur_mdh;//derzeiteige smdh
	//std::vector<size_t> m_meas_dims;//dimension der Matrix//TODO schauen ob man nicht weglassen kann da schon in Matrix steht
	Data<T> m_matrix; //matrix mit gespeicherten werten
	Data<T> m_sync; //sync-matrix mit gespeicherten werten
	size_t m_meas_col_bytes;//groesse der messdaten nach der eingabe
	size_t m_pos;//pos des lese zeigers
	bool m_initialised;//ist das program initialisiert
	bool m_verb;// Wird beschildert was vorgeht
#ifdef USE_OMP1
	int NumOfThreas;
	vector<int> ThreadPos;
	int ThreadNum;
	int ThreadSize;
#endif
};

template<class T>
rawParser<T>::rawParser(const string& fname) :
				m_ifs(fname.c_str(),ios::binary|ios::in), m_size(0),
				m_dataSize(0),m_fod(0), m_acqend(false), m_pdata(0), m_mdata(0),
				m_palloc(false), m_malloc(false),m_meas_col_bytes(0), m_pos(0),m_verb(true) {
				 /*m_cur_sync(0),*/
	if(m_ifs.fail())
		cout<<"Lesen ist Fehlgeschlagen"<<endl;
	// TODO Open the file
//	cout<<fname<<endl;
//	if(m_ifs.is_open())
//		cout<<"wurde geoeffnet"<<endl;
	//m_matrix()=new Data<T>();
	//m_meas_dims = std::vector<size_t>(MAX_ICE_DIM,(size_t)1);
	if (m_ifs.fail()) {
		cout << "Error opening file " << fname << endl;
		cerr << "Error opening file " << fname << endl;
		m_status = FILE_OPEN_FAILED;
	}
	m_initialised = true;
#ifdef USE_OMP1
	//cout<<" OMP verbesert "<< endl;
#endif
#ifdef USE_OMP
	NumOfThreas=omp_get_num_procs();
	ThreadPos=new vector<T>(NumOfThreads+1);
#endif

}

template<class T>
rawParser<T>::~rawParser() {

	this->CleanUp();
	m_sync.Clear();
	m_matrix.Clear();
}

/*template<>
error_code rawParser<float>::Save(H5std_string output) {
	//try {
		H5File* file = new H5File(output, H5F_ACC_TRUNC); //ueberspeichert vorhandene Datei
		cout<<"Datei geoeffnet"<<endl;
		hsize_t fdim[MAX_ICE_DIM];
		for (int i = 0; i < MAX_ICE_DIM; ++i)
			fdim[i] = m_matrix.dims[i];
		DataSpace fspace(MAX_ICE_DIM, fdim);
		cout<<"Groessen erstellt"<<endl;
		DataSet* dataset = new DataSet(
				file->createDataSet("dset_i", PredType::NATIVE_FLOAT,fspace));//nimmt vorhandenen Datensatz und benutzt ihn
		cout<<"Dataset erstellt"<<endl;
		dataset->write((char*) (&(m_matrix.data[0])), PredType::NATIVE_FLOAT);
		cout<<"Dataset geschrieben"<<endl;
		delete dataset;
		file->close();
		delete file;
		cout<<"abgeschlossen"<<endl;
	/*}
	// catch failure caused by the H5File operations
	catch (FileIException error) {
		error.printError();
		return HDF5_ERROR_FOPEN;
	}

	// catch failure caused by the DataSet operations
	catch (DataSetIException error) {
		error.printError();
		return HDF5_ERROR_FOPEN;
	}

	// catch failure caused by the DataSpace operations
	catch (DataSpaceIException error) {
		error.printError();
		return HDF5_ERROR_FOPEN;
	}*/
		/*return OK;

}

template<class T>
error_code rawParser<T>::Save(H5std_string output) {
	//try {
		H5File* file = new H5File(output, H5F_ACC_TRUNC); //ueberspeichert vorhandene Datei
		cout<<"Datei geoeffnet"<<endl;
		hsize_t fdim[MAX_ICE_DIM];
		for (int i = 0; i < MAX_ICE_DIM; ++i)
			fdim[i] = m_matrix.dims[i];
		DataSpace fspace(MAX_ICE_DIM, fdim);
		cout<<"Groessen erstellt"<<endl;
		DataSet* dataset = new DataSet(
				file->createDataSet("dset_i", PredType::NATIVE_FLOAT,fspace));//nimmt vorhandenen Datensatz und benutzt ihn
		cout<<"Dataset erstellt"<<endl;
		dataset->write((char*) (&(m_matrix.data[0])), PredType::NATIVE_FLOAT);
		cout<<"Dataset geschrieben"<<endl;
		delete dataset;
		file->close();
		delete file;
		cout<<"abgeschlossen"<<endl;
	/*}
	// catch failure caused by the H5File operations
	catch (FileIException error) {
		error.printError();
		return HDF5_ERROR_FOPEN;
	}

	// catch failure caused by the DataSet operations
	catch (DataSetIException error) {
		error.printError();
		return HDF5_ERROR_FOPEN;
	}

	// catch failure caused by the DataSpace operations
	catch (DataSpaceIException error) {
		error.printError();
		return HDF5_ERROR_FOPEN;
	}*/
		/*return OK;

}*/

template<class T>
error_code rawParser<T>::Read() {

	m_status = OK;
	/* Calculate data size */
	CalcSize();
//	cout<<fixed<<m_ifs.tellg()<<endl;
	/* Read protocol into buffer */
	ReadPFile();
#ifdef USE_OMP1
#pragma omp sections
	{
#pragma omp section
	{
#endif
	/* Exract protocol */
	ExtractProtocol();

	if (m_palloc) {
		delete[] m_pdata;
		m_palloc = false;
	}
#ifdef USE_OMP1
	}
#pragma omp section
	{
#endif
	/* Read messement into buffer */
	ReadMFile();

	/* Access matrix sizes */
//if (amode == SCAN_MEAS)
	preParseMeas(m_verb);
	/*else
	 ParseProtocol(this->m_verb);*/
#ifdef USE_OMP1
	}
	}
#endif
	/*for(int i=0;i<16;++i)
		cout<<m_matrix.dims[i]<<",";
	cout<<endl;*/
	/* Allocate matrices */
	AllocateMatrices();

	/* Parse data into matrices */
	ParseMeas();
	//cout<<"ende"<< scientific <<endl;
//	float* a=(float*) m_matrix.data;
//	for(int i=0;i<30;i+=1)
//			cout << i<< ":" << a[i] <<endl;
//	cout<<"Clean Up"<<endl;
	/* Free buffer */

	CleanUp();

	return m_status;

}

template<class T>
error_code rawParser<T>::CalcSize() {
	unsigned fod;
	char *fode=(char *) &fod;
	m_ifs.read(fode,sizeof(fod));// >> fod;
	m_fod = (size_t) fod;
	//cout<< m_fod <<endl;
	if (m_ifs.fail()) {
		cerr << "Reading error" << endl;
		m_status = FILE_READ_FAILED;
	}

	//cout << "\n Protocol:" << setprecision(1)<<fixed
//			<< ((-16.0 + (float) m_fod) / 1024.0) << "KB\n" << endl;
	m_ifs.seekg(-1, ios::end);
	m_size = m_ifs.tellg();
	m_dataSize = m_size - m_fod;
//	cout << " Measurement: " << setprecision(1)<<fixed
//			<< (((float) m_dataSize) / 1024.0 / 1024.0) << " MB\n" << endl;
	m_ifs.seekg(0);

#ifdef USE_OMP
	ThreadSize=m_dataSize/NumOfThreads;
#endif

	return m_status;
}

template<class T>
error_code rawParser<T>::ReadPFile() {
	// TODO Datei lesen und direkt in einem Vector speichern
	//Speicher allokieren
	m_pdata = new char[m_fod];
	//Daten in Speicher laden
	m_ifs.read(m_pdata, m_fod);
//	if(m_ifs.good())
//		cout<<"Kein fehler"<<endl;
	m_palloc=true;
	return m_status = OK;

}

template<class T>
error_code rawParser<T>::ExtractProtocol()/*const bool verbose = true)*/{

	m_status = OK;
	m_xprot.append(&m_pdata[19], m_fod);
	return m_status;

}

template<class T>
error_code rawParser<T>::ParseProtocol(const bool verbose /*= true*/, const bool dry /*= false*/) {

	m_status = OK;

	return m_status;

}

template<class T>
error_code rawParser<T>::ReadMFile() {
	// TODO Datei lesen und direkt in einem Vector speichern
	//Speicher allokieren
	m_mdata = new char[m_dataSize];
	m_malloc=true;
	//Daten in Speicher laden
	m_ifs.read(m_mdata, m_dataSize);
//	cout<<"Messwerte wurden gelesen"<<endl;
	return m_status = OK;

}

template<class T>
error_code rawParser<T>::preParseMeas(const bool verbose /*= true*/) {

	m_status = OK;
	m_acqend = false;
	m_pos = 0;
	//m_cur_sync = 0;

//	cout << " Parsing measurement (dry run!)...\n" << endl;
//	size_t sc = 0;


	while (true) {


#ifdef USE_OMP
	if(m_pos>=ThreadSize*ThreadNum)
	{
		ThreadPos[ThreadNum]=m_pos;
		ThreadNum++;
	}
#endif


		preParseMDH(verbose);
		if (m_acqend)	//Abfrage ob es die letzte mdh war
		{
#ifdef USE_OMP
			ThreadPos[ThreadNum]=m_dataSize;
#endif
//				cout<<"Ende erreicht"<<endl;
				break;
		}

		/*if (bit_set(m_cur_mdh.aulEvalInfoMask[0], SYNCDATA))
			cout<<"Sync Data erreicht"<<endl;
			break;
//ParseSyncData(verbose, dry);
		else*/
			m_pos += m_meas_col_bytes;

//		sc++;

	}

//	cout << " ... done.\n\n" << endl;

	return m_status;

}

template<class T>
error_code rawParser<T>::preParseMDH(const bool verbose /*= true*/) {

	m_status = OK;
	vector<long unsigned int>* matrix_dim;

	memcpy(&m_cur_mdh, &m_mdata[m_pos], sizeof(sMDH));	//copy wuerde es auc tun
	if (bit_set(m_cur_mdh.aulEvalInfoMask[0], ACQEND)) {
		m_acqend = true;
	//	cout<<"Ende erreicht"<<endl;
		return m_status;
	}
	if (bit_set(m_cur_mdh.aulEvalInfoMask[0],SYNCDATA)) {
		matrix_dim=&(m_sync.dims);
		//may jump a bit more for spezial syncdata bits
	}
	else
		matrix_dim=&(m_matrix.dims);

	if ((*matrix_dim)[0] == 1) {
//		cout<<"measdims[0]==1"<<endl;
		/*
		 * 		m_meas_dims[0] = (size_t) m_cur_mdh.ushSamplesInScan;
				m_meas_dims[6] = (size_t) m_cur_mdh.ushUsedChannels;
				ersetzt
		 */
		(*matrix_dim)[COL] = (size_t) m_cur_mdh.ushSamplesInScan;
		(*matrix_dim)[CHA] = (size_t) m_cur_mdh.ushUsedChannels;
//		cout<<"size set"<<endl;
	 } else {
		if ((*matrix_dim)[LIN] < (size_t) m_cur_mdh.sLC.ushLine + 1)
			(*matrix_dim)[LIN] = m_cur_mdh.sLC.ushLine + 1;
		if ((*matrix_dim)[SLC] < (size_t) m_cur_mdh.sLC.ushSlice + 1)
			(*matrix_dim)[SLC] = m_cur_mdh.sLC.ushSlice + 1;
		if ((*matrix_dim)[PAR] < (size_t) m_cur_mdh.sLC.ushPartition + 1)
			(*matrix_dim)[PAR] = m_cur_mdh.sLC.ushPartition + 1;
		if ((*matrix_dim)[SEG] < (size_t) m_cur_mdh.sLC.ushSeg + 1){
			(*matrix_dim)[SEG] = m_cur_mdh.sLC.ushSeg + 1;}
		if ((*matrix_dim)[ECO] < (size_t) m_cur_mdh.sLC.ushEcho + 1)
			(*matrix_dim)[ECO] = m_cur_mdh.sLC.ushEcho + 1;
		if ((*matrix_dim)[SET] < (size_t) m_cur_mdh.sLC.ushSet + 1)
			(*matrix_dim)[SET] = m_cur_mdh.sLC.ushSet + 1;
		if ((*matrix_dim)[PHS] < (size_t) m_cur_mdh.sLC.ushPhase + 1)
			(*matrix_dim)[PHS] = m_cur_mdh.sLC.ushPhase + 1;
		if ((*matrix_dim)[REP] < (size_t) m_cur_mdh.sLC.ushRepetition + 1)
			(*matrix_dim)[REP] = m_cur_mdh.sLC.ushRepetition + 1;
		if ((*matrix_dim)[IDA] < (size_t) m_cur_mdh.sLC.ushIda + 1)
			(*matrix_dim)[IDA] = m_cur_mdh.sLC.ushIda + 1;
		if ((*matrix_dim)[IDB] < (size_t) m_cur_mdh.sLC.ushIdb + 1)
			(*matrix_dim)[IDB] = m_cur_mdh.sLC.ushIdb + 1;
		if ((*matrix_dim)[IDC] < (size_t) m_cur_mdh.sLC.ushIdc + 1)
			(*matrix_dim)[IDC] = m_cur_mdh.sLC.ushIdc + 1;
		if ((*matrix_dim)[IDD] < (size_t) m_cur_mdh.sLC.ushIdd + 1)
			(*matrix_dim)[IDD] = m_cur_mdh.sLC.ushIdd + 1;
		if ((*matrix_dim)[IDE] < (size_t) m_cur_mdh.sLC.ushIde + 1)
			(*matrix_dim)[IDE] = m_cur_mdh.sLC.ushIde + 1;
	}
	m_pos += sizeof(sMDH);
	m_meas_col_bytes = sizeof(std::complex<T>) * (*matrix_dim)[0];	//wie gross ist der folgende Header
	return m_status;

}

template<class T>
error_code rawParser<T>::AllocateMatrices(const bool verbose /*= true*/) {

	m_status = OK;

//	cout<< " Allocating data matrices ..."<<endl;

//std::copy(m_meas_dims.begin(), m_meas_dims.end(), std::ostream_iterator<int>(std::cout));

	m_matrix.Allocate();

	m_sync.Allocate();

//	cout<<" ... done."<<endl;

	return m_status;

}

template<class T>
error_code rawParser<T>::ParseMeas(const bool verbose /*= true*/) {

	m_status = OK;
	m_acqend = false;
	m_pos = 0;
	//m_cur_sync = 0;

//	cout << " Parsing measurement ";

//	size_t sc = 0;
#ifdef USE_OMP
#pragma omp parallel for private (m_cur_mdh,matrix,m_pos,m_meas_col_bytes)
	for (int i=0;i<NumOfThreads;i++) {
	m_pos=ThreadPos[i];
	while(true){
		ParseMDH(verbose);
		if (m_acqend||ThreadPos[i+1]>=m_pos)	//Abfrage ob es die letzte mdh war
			break;

		if (bit_set(m_cur_mdh.aulEvalInfoMask[0], SYNCDATA))
			matrix=&m_sync;
		else
			matrix=&m_matrix;
//ParseSyncData(verbose, dry);
			ParseScan(matrix,verbose);

		//sc++;
	}
	}
#else
	while (true) {

		ParseMDH(verbose);
		if (m_acqend)	//Abfrage ob es die letzte mdh war
			break;

		if (bit_set(m_cur_mdh.aulEvalInfoMask[0], SYNCDATA))
			ParseScan(m_sync,verbose);
		else
			ParseScan(m_matrix,verbose);
//ParseSyncData(verbose, dry);

	//	sc++;

	}
#endif
	//cout << " ... done.\n\n" << endl;

	return m_status;

}

template<class T>
error_code rawParser<T>::ParseMDH(const bool verbose /*= true*/) {

	m_status = OK;

	memcpy(&m_cur_mdh, &m_mdata[m_pos], sizeof(sMDH));	//copy wuerde es auc tun

	if (bit_set(m_cur_mdh.aulEvalInfoMask[0], ACQEND)) {
		m_acqend = true;
		return m_status;
	}

	if (bit_set(m_cur_mdh.aulEvalInfoMask[0],SYNCDATA)) {
		//may jump a bit more for spezial syncdata bits
	}
	m_meas_col_bytes = sizeof(std::complex<T>) * m_cur_mdh.ushSamplesInScan;
	m_pos += sizeof(sMDH);
	return m_status;

}

template<class T>
error_code rawParser<T>::ParseScan(Data<T> matrix,const size_t& c) {

	m_status = OK;

	/*
	 size_t dpos =
	 m_cur_mdh.sLC.ushLine * m_meas_dims[LIN] +
	 m_cur_mdh.sLC.ushSlice * m_meas_dims[SLC] +
	 m_cur_mdh.sLC.ushPartition * m_meas_dims[PAR] +
	 m_cur_mdh.sLC.ushSeg * m_meas_dims[SEG] +
	 m_cur_mdh.sLC.ushEcho * m_meas_dims[ECO] +
	 m_cur_mdh.ushChannelId * m_meas_dims[CHA] +
	 m_cur_mdh.sLC.ushSet * m_meas_dims[SET] +
	 m_cur_mdh.sLC.ushPhase * m_meas_dims[PHS] +
	 m_cur_mdh.sLC.ushRepetition * m_meas_dims[REP] +
	 m_cur_mdh.sLC.ushIda * m_meas_dims[IDA] +
	 m_cur_mdh.sLC.ushIdb * m_meas_dims[IDB] +
	 m_cur_mdh.sLC.ushIdc * m_meas_dims[IDC] +
	 m_cur_mdh.sLC.ushIdd * m_meas_dims[IDD] +
	 m_cur_mdh.sLC.ushIde * m_meas_dims[IDE];

	 memcpy (&m_meas->At(dpos), &m_buffer[m_pos], m_meas_col_bytes);
	 */

	/*cout << "0," << m_cur_mdh.sLC.ushLine << "," << m_cur_mdh.sLC.ushSlice
			<< "," << m_cur_mdh.sLC.ushPartition << "," << m_cur_mdh.sLC.ushSeg
			<< "," << m_cur_mdh.sLC.ushEcho << "," << m_cur_mdh.ushChannelId
			<< "," << m_cur_mdh.sLC.ushSet << "," << m_cur_mdh.sLC.ushPhase
			<< "," << m_cur_mdh.sLC.ushRepetition << "," << m_cur_mdh.sLC.ushIda
			<< "," << m_cur_mdh.sLC.ushIdb << "," << m_cur_mdh.sLC.ushIdc << ","
			<< m_cur_mdh.sLC.ushIdd << "," << m_cur_mdh.sLC.ushIde << endl;*/
/*	for(int i=0;i<10;i++)
		cout<<((float)m_mdata[m_pos+i]);
		cout<<endl;
		char* a= &(m_mdata[m_pos]);
		cout<< a<<endl;*/

	matrix.insert( &(m_mdata[m_pos]), m_meas_col_bytes,&m_cur_mdh);
	m_pos += m_meas_col_bytes;
	return m_status;

}

template<class T>
error_code rawParser<T>::CleanUp(const bool verbose /*= true*/) {

	m_status = OK;

	if (m_initialised) {

		if (m_malloc) {
			delete (m_mdata);
			m_malloc = false;
		}

		if (m_palloc) {
			delete (m_pdata);
			m_palloc = false;
		}
		m_ifs.close();

//m_sync.Clear();
//m_noise.Clear();
//m_acs.Clear();

		m_initialised = false;
	}
	return m_status;

}

#endif /* RAWPARSER_H_ */
