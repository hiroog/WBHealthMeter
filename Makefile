
TARGET_ARC= wbhealthmeter112.zip


TARGET_EXE32=WBHealthMeter.exe
TARGET_EXE64=WBHealthMeter64.exe
SRC_EXE32=Win32\Release\WBHealthMeter.exe
SRC_EXE64=x64\Release\WBHealthMeter.exe

SFILES=WBHealthMeter.txt \
	$(TARGET_EXE32) \
	$(TARGET_EXE64)

.PHONY: top

top:
	@echo start wbhakari.sln
	@echo nmake zip


.PHONY: zip

zip: $(TARGET_ARC)

$(TARGET_ARC): $(SFILES)
	7z a -tzip $(TARGET_ARC) $(SFILES)


$(TARGET_EXE32): $(SRC_EXE32)
	copy $(SRC_EXE32) $(TARGET_EXE32)

$(TARGET_EXE64): $(SRC_EXE64)
	copy $(SRC_EXE64) $(TARGET_EXE64)

