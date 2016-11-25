#
# MDTRA Makefile for x86 Linux
#

EXENAME=mdtra

ARCH=i686
OPTIMIZE=2

# make sure this is the correct compiler for your system
CC=g++
NVCC=/usr/local/cuda/bin/nvcc
CP=cp
STRIP=strip -x

EXE_SRCDIR=../src
EXE_RESDIR=../res
EXE_DSTDIR=../output
EXE_OBJDIR=./obj-$(ARCH)

MK_QTBIN:=/home/xaerox/Qt/4.8.7-32/bin/
MK_QTINC:=/home/xaerox/Qt/4.8.7-32/include
MK_MOC=$(MK_QTBIN)moc
MK_UIC=$(MK_QTBIN)uic
MK_RCC=$(MK_QTBIN)rcc

SHARED_SRCDIR=../src/shared

BASE_CFLAGS= -DLINUX -DNDEBUG

#full optimization
SHCFLAGS=-m32 -Wall -O$(OPTIMIZE) -mtune=$(ARCH) -march=pentium -ffast-math -funroll-loops \
		 -fomit-frame-pointer -fexpensive-optimizations -fno-strict-aliasing
CFLAGS=$(BASE_CFLAGS) $(SHCFLAGS)

INCLUDEDIRS= -I$(SHARED_SRCDIR) -I$(MK_QTINC) -I/usr/local/cuda/include
LIBRARYDIRS=

LDFLAGS=-ldl -lGL -lGLU lua.so libcudart.so libQtCore.so.4 libQtGui.so.4 libQtOpenGL.so.4 -static-libstdc++ -static-libgcc -lm

DO_CC=$(CC) $(CFLAGS) $(INCLUDEDIRS) -o $@ -c $<

#############################################################################
# SETUP AND BUILD
#############################################################################

$(EXE_OBJDIR)/%.o: $(EXE_SRCDIR)/%.cpp
	$(DO_CC)

OBJ = \
	$(EXE_OBJDIR)/mdtra_2D_RMSD_Dialog.o \
	$(EXE_OBJDIR)/mdtra_2D_RMSD_Plot.o \
	$(EXE_OBJDIR)/mdtra_colors.o \
	$(EXE_OBJDIR)/mdtra_compact_pdb.o \
	$(EXE_OBJDIR)/mdtra_configFile.o \
	$(EXE_OBJDIR)/mdtra_cpuid.o \
	$(EXE_OBJDIR)/mdtra_cuda.o \
	$(EXE_OBJDIR)/mdtra_dataSourceDialog.o \
	$(EXE_OBJDIR)/mdtra_distanceSearch.o \
	$(EXE_OBJDIR)/mdtra_distanceSearchDialog.o \
	$(EXE_OBJDIR)/mdtra_distanceSearchResultsDialog.o \
	$(EXE_OBJDIR)/mdtra_dnaDataMiningDialog.o \
	$(EXE_OBJDIR)/mdtra_forceSearch.o \
	$(EXE_OBJDIR)/mdtra_forceSearchDialog.o \
	$(EXE_OBJDIR)/mdtra_forceSearchResultsDialog.o \
	$(EXE_OBJDIR)/mdtra_formatDialog.o \
	$(EXE_OBJDIR)/mdtra_genericPlot.o \
	$(EXE_OBJDIR)/mdtra_gpuInfoDialog.o \
	$(EXE_OBJDIR)/mdtra_hbSearch.o \
	$(EXE_OBJDIR)/mdtra_hbSearchDialog.o \
	$(EXE_OBJDIR)/mdtra_hbSearchResultsDialog.o \
	$(EXE_OBJDIR)/mdtra_histogramDialog.o \
	$(EXE_OBJDIR)/mdtra_histogramPlot.o \
	$(EXE_OBJDIR)/mdtra_inputTextDialog.o \
	$(EXE_OBJDIR)/mdtra_labelDialog.o \
	$(EXE_OBJDIR)/mdtra_main.o \
	$(EXE_OBJDIR)/mdtra_mainWindow.o \
	$(EXE_OBJDIR)/mdtra_messageDialog.o \
	$(EXE_OBJDIR)/mdtra_multiDataSourceDialog.o \
	$(EXE_OBJDIR)/mdtra_multiResultDataSourceDialog.o \
	$(EXE_OBJDIR)/mdtra_occluderDialog.o \
	$(EXE_OBJDIR)/mdtra_pca.o \
	$(EXE_OBJDIR)/mdtra_pcaDialog.o \
	$(EXE_OBJDIR)/mdtra_pcaResultsDialog.o \
	$(EXE_OBJDIR)/mdtra_pdb.o \
	$(EXE_OBJDIR)/mdtra_pdb_format.o \
	$(EXE_OBJDIR)/mdtra_pdbCanvas.o \
	$(EXE_OBJDIR)/mdtra_pdbRenderer.o \
	$(EXE_OBJDIR)/mdtra_pipe.o \
	$(EXE_OBJDIR)/mdtra_plot.o \
	$(EXE_OBJDIR)/mdtra_plotDataFilterDialog.o \
	$(EXE_OBJDIR)/mdtra_preferencesDialog.o \
	$(EXE_OBJDIR)/mdtra_prepWaterShellDialog.o \
	$(EXE_OBJDIR)/mdtra_prog_compiler.o \
	$(EXE_OBJDIR)/mdtra_prog_interpreter.o \
	$(EXE_OBJDIR)/mdtra_prog_state.o \
	$(EXE_OBJDIR)/mdtra_prog_syntaxHighlight.o \
	$(EXE_OBJDIR)/mdtra_progressDialog.o \
	$(EXE_OBJDIR)/mdtra_project.o \
	$(EXE_OBJDIR)/mdtra_render_pdb.o \
	$(EXE_OBJDIR)/mdtra_resultDataSourceDialog.o \
	$(EXE_OBJDIR)/mdtra_resultDialog.o \
	$(EXE_OBJDIR)/mdtra_SAS.o \
	$(EXE_OBJDIR)/mdtra_secure_crt_impl.o \
	$(EXE_OBJDIR)/mdtra_select.o \
	$(EXE_OBJDIR)/mdtra_selectionDialog.o \
	$(EXE_OBJDIR)/mdtra_streamDialog.o \
	$(EXE_OBJDIR)/mdtra_streamMaskDialog.o \
	$(EXE_OBJDIR)/mdtra_threads.o \
	$(EXE_OBJDIR)/mdtra_torsionSearch.o \
	$(EXE_OBJDIR)/mdtra_torsionSearchDialog.o \
	$(EXE_OBJDIR)/mdtra_torsionSearchResultsDialog.o \
	$(EXE_OBJDIR)/mdtra_userTypeDialog.o \
	$(EXE_OBJDIR)/mdtra_utils.o \
	$(EXE_OBJDIR)/mdtra_waitDialog.o \
	$(EXE_OBJDIR)/mdtra_select_grammar_parser.o \
	$(EXE_OBJDIR)/mdtra_select_tokens_lexer.o \
	$(EXE_OBJDIR)/moc_mdtra_2D_RMSD_Dialog.o \
	$(EXE_OBJDIR)/moc_mdtra_2D_RMSD_Plot.o \
	$(EXE_OBJDIR)/moc_mdtra_customTableWidgetItems.o \
	$(EXE_OBJDIR)/moc_mdtra_dataSourceDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_distanceSearchDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_distanceSearchResultsDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_dnaDataMiningDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_forceSearchDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_forceSearchResultsDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_formatDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_genericPlot.o \
	$(EXE_OBJDIR)/moc_mdtra_gpuInfoDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_hbSearchDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_hbSearchResultsDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_histogramDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_histogramPlot.o \
	$(EXE_OBJDIR)/moc_mdtra_inputTextDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_labelDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_mainWindow.o \
	$(EXE_OBJDIR)/moc_mdtra_messageDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_multiDataSourceDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_multiResultDataSourceDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_occluderDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_pcaDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_pcaResultsDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_pdbCanvas.o \
	$(EXE_OBJDIR)/moc_mdtra_pdbRenderer.o \
	$(EXE_OBJDIR)/moc_mdtra_plot.o \
	$(EXE_OBJDIR)/moc_mdtra_plotDataFilterDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_preferencesDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_prepWaterShellDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_progressDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_waitDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_resultDataSourceDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_resultDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_selectionDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_streamDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_streamMaskDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_torsionSearchDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_torsionSearchResultsDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_userTypeDialog.o \
	$(EXE_OBJDIR)/moc_mdtra_prog_syntaxHighlight.o \
	$(EXE_OBJDIR)/mdtra_cuda_SAS.cu.obj \
	$(EXE_OBJDIR)/qrc_$(EXENAME).o
	
$(EXENAME) : neat $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBRARYDIRS) $(LDFLAGS)
	$(STRIP) $(EXENAME)
	$(CP) $(EXENAME) $(EXE_DSTDIR)/$(EXENAME)

neat:
	-mkdir -p $(EXE_OBJDIR)
	-bison --no-lines -d -o $(EXE_SRCDIR)/mdtra_select_grammar_parser.c $(EXE_SRCDIR)/mdtra_select_grammar.y
	-mv -f $(EXE_SRCDIR)/mdtra_select_grammar_parser.c $(EXE_SRCDIR)/mdtra_select_grammar_parser.cpp
	-flex -o $(EXE_SRCDIR)/mdtra_select_tokens_lexer.cpp $(EXE_SRCDIR)/mdtra_select_tokens.l
	-$(MK_UIC) $(EXE_SRCDIR)/dataSourceDialog.ui -o $(EXE_SRCDIR)/ui_dataSourceDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/distanceSearchDialog.ui -o $(EXE_SRCDIR)/ui_distanceSearchDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/distanceSearchResultsDialog.ui -o $(EXE_SRCDIR)/ui_distanceSearchResultsDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/dnaDataMiningDialog.ui -o $(EXE_SRCDIR)/ui_dnaDataMiningDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/forceSearchDialog.ui -o $(EXE_SRCDIR)/ui_forceSearchDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/forceSearchResultsDialog.ui -o $(EXE_SRCDIR)/ui_forceSearchResultsDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/formatDialog.ui -o $(EXE_SRCDIR)/ui_formatDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/gpuInfoDialog.ui -o $(EXE_SRCDIR)/ui_gpuInfoDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/hbSearchDialog.ui -o $(EXE_SRCDIR)/ui_hbSearchDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/hbSearchResultsDialog.ui -o $(EXE_SRCDIR)/ui_hbSearchResultsDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/histogramDialog.ui -o $(EXE_SRCDIR)/ui_histogramDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/inputTextDialog.ui -o $(EXE_SRCDIR)/ui_inputTextDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/labelDialog.ui -o $(EXE_SRCDIR)/ui_labelDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/messageDialog.ui -o $(EXE_SRCDIR)/ui_messageDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/multiDataSourceDialog.ui -o $(EXE_SRCDIR)/ui_multiDataSourceDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/multiResultDataSourceDialog.ui -o $(EXE_SRCDIR)/ui_multiResultDataSourceDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/occluderDialog.ui -o $(EXE_SRCDIR)/ui_occluderDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/pcaDialog.ui -o $(EXE_SRCDIR)/ui_pcaDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/pcaResultsDialog.ui -o $(EXE_SRCDIR)/ui_pcaResultsDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/plotDataFilter.ui -o $(EXE_SRCDIR)/ui_plotDataFilter.h
	-$(MK_UIC) $(EXE_SRCDIR)/preferencesDialog.ui -o $(EXE_SRCDIR)/ui_preferencesDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/prepWaterShellDialog.ui -o $(EXE_SRCDIR)/ui_prepWaterShellDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/progressDialog.ui -o $(EXE_SRCDIR)/ui_progressDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/waitDialog.ui -o $(EXE_SRCDIR)/ui_waitDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/resultDataSourceDialog.ui -o $(EXE_SRCDIR)/ui_resultDataSourceDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/resultDialog.ui -o $(EXE_SRCDIR)/ui_resultDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/rmsd2dDialog.ui -o $(EXE_SRCDIR)/ui_rmsd2dDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/selectionDialog.ui -o $(EXE_SRCDIR)/ui_selectionDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/streamDialog.ui -o $(EXE_SRCDIR)/ui_streamDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/streamMaskDialog.ui -o $(EXE_SRCDIR)/ui_streamMaskDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/torsionSearchDialog.ui -o $(EXE_SRCDIR)/ui_torsionSearchDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/torsionSearchResultsDialog.ui -o $(EXE_SRCDIR)/ui_torsionSearchResultsDialog.h
	-$(MK_UIC) $(EXE_SRCDIR)/userTypeDialog.ui -o $(EXE_SRCDIR)/ui_userTypeDialog.h
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_2D_RMSD_Dialog.h -o $(EXE_SRCDIR)/moc_mdtra_2D_RMSD_Dialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_2D_RMSD_Plot.h -o $(EXE_SRCDIR)/moc_mdtra_2D_RMSD_Plot.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_customTableWidgetItems.h -o $(EXE_SRCDIR)/moc_mdtra_customTableWidgetItems.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_dataSourceDialog.h -o $(EXE_SRCDIR)/moc_mdtra_dataSourceDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_distanceSearchDialog.h -o $(EXE_SRCDIR)/moc_mdtra_distanceSearchDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_distanceSearchResultsDialog.h -o $(EXE_SRCDIR)/moc_mdtra_distanceSearchResultsDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_dnaDataMiningDialog.h -o $(EXE_SRCDIR)/moc_mdtra_dnaDataMiningDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_forceSearchDialog.h -o $(EXE_SRCDIR)/moc_mdtra_forceSearchDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_forceSearchResultsDialog.h -o $(EXE_SRCDIR)/moc_mdtra_forceSearchResultsDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_formatDialog.h -o $(EXE_SRCDIR)/moc_mdtra_formatDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_genericPlot.h -o $(EXE_SRCDIR)/moc_mdtra_genericPlot.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_gpuInfoDialog.h -o $(EXE_SRCDIR)/moc_mdtra_gpuInfoDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_hbSearchDialog.h -o $(EXE_SRCDIR)/moc_mdtra_hbSearchDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_hbSearchResultsDialog.h -o $(EXE_SRCDIR)/moc_mdtra_hbSearchResultsDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_histogramDialog.h -o $(EXE_SRCDIR)/moc_mdtra_histogramDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_histogramPlot.h -o $(EXE_SRCDIR)/moc_mdtra_histogramPlot.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_inputTextDialog.h -o $(EXE_SRCDIR)/moc_mdtra_inputTextDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_labelDialog.h -o $(EXE_SRCDIR)/moc_mdtra_labelDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_mainWindow.h -o $(EXE_SRCDIR)/moc_mdtra_mainWindow.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_messageDialog.h -o $(EXE_SRCDIR)/moc_mdtra_messageDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_multiDataSourceDialog.h -o $(EXE_SRCDIR)/moc_mdtra_multiDataSourceDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_multiResultDataSourceDialog.h -o $(EXE_SRCDIR)/moc_mdtra_multiResultDataSourceDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_occluderDialog.h -o $(EXE_SRCDIR)/moc_mdtra_occluderDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_pcaDialog.h -o $(EXE_SRCDIR)/moc_mdtra_pcaDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_pcaResultsDialog.h -o $(EXE_SRCDIR)/moc_mdtra_pcaResultsDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_pdbCanvas.h -o $(EXE_SRCDIR)/moc_mdtra_pdbCanvas.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_pdbRenderer.h -o $(EXE_SRCDIR)/moc_mdtra_pdbRenderer.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_plot.h -o $(EXE_SRCDIR)/moc_mdtra_plot.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_plotDataFilterDialog.h -o $(EXE_SRCDIR)/moc_mdtra_plotDataFilterDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_preferencesDialog.h -o $(EXE_SRCDIR)/moc_mdtra_preferencesDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_prepWaterShellDialog.h -o $(EXE_SRCDIR)/moc_mdtra_prepWaterShellDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_progressDialog.h -o $(EXE_SRCDIR)/moc_mdtra_progressDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_waitDialog.h -o $(EXE_SRCDIR)/moc_mdtra_waitDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_resultDataSourceDialog.h -o $(EXE_SRCDIR)/moc_mdtra_resultDataSourceDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_resultDialog.h -o $(EXE_SRCDIR)/moc_mdtra_resultDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_selectionDialog.h -o $(EXE_SRCDIR)/moc_mdtra_selectionDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_streamDialog.h -o $(EXE_SRCDIR)/moc_mdtra_streamDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_streamMaskDialog.h -o $(EXE_SRCDIR)/moc_mdtra_streamMaskDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_torsionSearchDialog.h -o $(EXE_SRCDIR)/moc_mdtra_torsionSearchDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_torsionSearchResultsDialog.h -o $(EXE_SRCDIR)/moc_mdtra_torsionSearchResultsDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_userTypeDialog.h -o $(EXE_SRCDIR)/moc_mdtra_userTypeDialog.cpp
	-$(MK_MOC) $(EXE_SRCDIR)/mdtra_prog_syntaxHighlight.h -o $(EXE_SRCDIR)/moc_mdtra_prog_syntaxHighlight.cpp
	-$(MK_RCC) $(EXE_RESDIR)/$(EXENAME).qrc -o $(EXE_SRCDIR)/qrc_$(EXENAME).cpp
	-$(NVCC) -ccbin /usr/bin/gcc-4.4 -c -m 32 -arch=compute_10 -code=sm_10 $(BASE_CFLAGS) -O$(OPTIMIZE) -Xcompiler "$(SHCFLAGS)" -o $(EXE_OBJDIR)/mdtra_cuda_SAS.cu.obj $(EXE_SRCDIR)/mdtra_cuda_SAS.cu
	
clean:
	-rm -f $(OBJ)
	-rm -f $(EXENAME)
spotless: clean
	-rm -r $(EXE_OBJDIR)
