#include "common.h"
#include "dvdread.h"

#include <dlfcn.h>

void *libHandle=0;

DvdreadFunctions_t f_dvdread;

void loadDvdread() {
	libHandle=dlopen("libdvdread.so",RTLD_LAZY | RTLD_GLOBAL);
	if (!libHandle) {
	    libHandle=dlopen("libdvdread.so.3",RTLD_LAZY | RTLD_GLOBAL);    
	}	
	if (!libHandle) {
	    libHandle=dlopen("libdvdread.so.2",RTLD_LAZY | RTLD_GLOBAL);    
	}
	f_dvdread.DVDOpen=dlsym(libHandle,"DVDOpen");
	f_dvdread.DVDOpenFile=dlsym(libHandle,"DVDOpenFile");
	f_dvdread.DVDClose=dlsym(libHandle,"DVDClose");
	f_dvdread.DVDCloseFile=dlsym(libHandle,"DVDCloseFile");
	f_dvdread.DVDReadBlocks=dlsym(libHandle,"DVDReadBlocks");
	f_dvdread.DVDFileSeek=dlsym(libHandle,"DVDFileSeek");
	f_dvdread.DVDReadBytes=dlsym(libHandle,"DVDReadBytes");
	f_dvdread.DVDFileSize=dlsym(libHandle,"DVDFileSize");
	f_dvdread.DVDDiscID=dlsym(libHandle,"DVDDiscID");
	f_dvdread.DVDUDFVolumeInfo=dlsym(libHandle,"DVDUDFVolumeInfo");
	f_dvdread.DVDISOVolumeInfo=dlsym(libHandle,"DVDISOVolumeInfo");
	f_dvdread.DVDUDFCacheLevel=dlsym(libHandle,"DVDUDFCacheLevel");
	f_dvdread.navRead_PCI=dlsym(libHandle,"navRead_PCI");
	f_dvdread.navRead_DSI=dlsym(libHandle,"navRead_DSI");


	f_dvdread.ifoOpen=dlsym(libHandle,"ifoOpen");
	f_dvdread.ifoOpenVMGI=dlsym(libHandle,"ifoOpenVMGI");
	f_dvdread.ifoOpenVTSI=dlsym(libHandle,"ifoOpenVTSI");
	f_dvdread.ifoClose=dlsym(libHandle,"ifoClose");
	f_dvdread.ifoRead_PTL_MAIT=dlsym(libHandle,"ifoRead_PTL_MAIT");
	f_dvdread.ifoRead_VTS_ATRT=dlsym(libHandle,"ifoRead_VTS_ATRT");
	f_dvdread.ifoRead_TT_SRPT=dlsym(libHandle,"ifoRead_TT_SRPT");
	f_dvdread.ifoRead_VTS_PTT_SRPT=dlsym(libHandle,"ifoRead_VTS_PTT_SRPT");

	f_dvdread.ifoRead_FP_PGC=dlsym(libHandle,"ifoRead_FP_PGC");
	f_dvdread.ifoRead_PGCIT=dlsym(libHandle,"ifoRead_PGCIT");
	f_dvdread.ifoRead_PGCI_UT=dlsym(libHandle,"ifoRead_PGCI_UT");
	f_dvdread.ifoRead_VTS_TMAPT=dlsym(libHandle,"ifoRead_VTS_TMAPT");
	f_dvdread.ifoRead_C_ADT=dlsym(libHandle,"ifoRead_C_ADT");
	f_dvdread.ifoRead_TITLE_C_ADT=dlsym(libHandle,"ifoRead_TITLE_C_ADT");
	f_dvdread.ifoRead_VOBU_ADMAP=dlsym(libHandle,"ifoRead_VOBU_ADMAP");
	f_dvdread.ifoRead_TITLE_VOBU_ADMAP=dlsym(libHandle,"ifoRead_TITLE_VOBU_ADMAP");
	f_dvdread.ifoRead_TXTDT_MGI=dlsym(libHandle,"ifoRead_TXTDT_MGI");

	f_dvdread.ifoFree_PTL_MAIT=dlsym(libHandle,"ifoFree_PTL_MAIT");
	f_dvdread.ifoFree_VTS_ATRT=dlsym(libHandle,"ifoFree_VTS_ATRT");
	f_dvdread.ifoFree_TT_SRPT=dlsym(libHandle,"ifoFree_TT_SRPT");
	f_dvdread.ifoFree_VTS_PTT_SRPT=dlsym(libHandle,"ifoFree_VTS_PTT_SRPT");
	f_dvdread.ifoFree_FP_PGC=dlsym(libHandle,"ifoFree_FP_PGC");
	f_dvdread.ifoFree_PGCIT=dlsym(libHandle,"ifoFree_PGCIT");
	f_dvdread.ifoFree_PGCI_UT=dlsym(libHandle,"ifoFree_PGCI_UT");
	f_dvdread.ifoFree_VTS_TMAPT=dlsym(libHandle,"ifoFree_VTS_TMAPT");
	f_dvdread.ifoFree_C_ADT=dlsym(libHandle,"ifoFree_C_ADT");
	f_dvdread.ifoFree_TITLE_C_ADT=dlsym(libHandle,"ifoFree_TITLE_C_ADT");
	f_dvdread.ifoFree_VOBU_ADMAP=dlsym(libHandle,"ifoFree_VOBU_ADMAP");
	f_dvdread.ifoFree_TITLE_VOBU_ADMAP=dlsym(libHandle,"ifoFree_TITLE_VOBU_ADMAP");
	f_dvdread.ifoFree_TXTDT_MGI=dlsym(libHandle,"ifoFree_TXTDT_MGI");

}

DvdreadFunctions_t *DvdreadF(void) {
   return &f_dvdread;
}

void unloadDvdread() {
	dlclose(libHandle);
}
