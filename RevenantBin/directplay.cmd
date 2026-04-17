@echo off
IF NOT DEFINED PROCESSOR_ARCHITEW6432 (	
	%SYSTEMROOT%\system32\DISM.EXE /Online /enable-feature /FeatureName:"DirectPlay" /all /NoRestart
) ELSE (		
	%SYSTEMROOT%\sysNative\DISM.EXE /Online /enable-feature /FeatureName:"DirectPlay" /all /NoRestart
)