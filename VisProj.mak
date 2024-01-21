# VisProj.mak
#
# Makefile to build the BetaVisSDK projects

!if "$(OUTDIR)" == ""
OUTDIR  = .\Temp
!endif

TARGET  = Build$(VISBUILD)


target ::  $(OUTDIR)\$(TARGET).snt


clean:
    if exist $(OUTDIR)\$(TARGET).snt del $(OUTDIR)\$(TARGET).snt


# We use $(TARGET).tmp to make sure that $(TARGET).snt is always rebuilt.

$(OUTDIR)\$(TARGET).tmp :
    @echo $(TARGET) >> $(OUTDIR)\$(TARGET).tmp

$(OUTDIR)\$(TARGET).snt :  $(OUTDIR)\$(TARGET).tmp
    echo $(TARGET) > $(OUTDIR)\$(TARGET).snt
	@del $(OUTDIR)\$(TARGET).tmp


