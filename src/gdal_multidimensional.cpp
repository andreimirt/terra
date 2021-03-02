#include "spatRaster.h"

#if GDAL_VERSION_MAJOR >= 3 && GDAL_VERSION_MINOR >= 1

#include "proj.h"


//#include "cpl_conv.h" // for CPLMalloc()
//#include "cpl_string.h"
//#include "ogr_spatialref.h"

#include "gdal_priv.h"
#include "gdal.h"


bool SpatRaster::constructFromFileMulti(std::string fname, std::string sub, std::vector<size_t> xyz) {

    GDALAllRegister();
    auto poDataset = std::unique_ptr<GDALDataset>(
        GDALDataset::Open(fname.c_str(), GDAL_OF_MULTIDIM_RASTER ));
    if( !poDataset ) {
		setError("not a good dataset");
        return false;
    }

	auto poRootGroup = poDataset->GetRootGroup();
    if( !poRootGroup ) {
		setError("no roots");
		return false;
    }

	//std::string var = "HWAM";
    auto poVar = poRootGroup->OpenMDArray(sub.c_str());
    if( !poVar )   {
		setError("cannot find: " + sub);
		return false;
    }

    size_t nValues = 1;
    std::vector<size_t> anCount;

    for( const auto poDim: poVar->GetDimensions() ) {
        anCount.push_back(static_cast<size_t>(poDim->GetSize()));
        nValues *= anCount.back();
    }

	//std::vector<size_t> xyz = {0,1,2};
	SpatRasterSource s;
	s.multidim = false;
	s.mdims = anCount;
	s.ncol=anCount[xyz[0]];
	s.nrow=anCount[xyz[1]];
	s.nlyr=anCount[xyz[2]];
	setSource(s);
	
	for (size_t i=0; i<anCount.size(); i++){
		Rcpp::Rcout << anCount[i] << std::endl;
	}
	
	Rcpp::Rcout << nValues << std::endl;
	addWarning("Good!");
	return true;
}


/*
read
    std::vector<double> values(nValues);
    poVar->Read(std::vector<GUInt64>{0,0,0}.data(),
                anCount.data(),
                nullptr, // step: defaults to 1,1,1 
                nullptr, // stride: default to row-major convention
                GDALExtendedDataType::Create(GDT_Float64),
                &values[0]);

			
	return values;
*/

  
#else  

bool SpatRaster::constructFromFileMulti(std::string fname, std::vector<int> subds, std::vector<std::string> subdsname) {
	setError("multidim is not supported by GDAL < 3.1")
	return false;
}


#endif

