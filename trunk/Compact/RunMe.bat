@echo off
copy /b ..\include\Shiny.h + ..\include\ShinyConfig.h + ..\include\ShinyPrereqs.h + ..\include\ShinyData.h + ..\include\ShinyTools.h + ..\include\ShinyNode.h + ..\include\ShinyZone.h + ..\include\ShinyNodePool.h + ..\include\ShinyOutput.h + ..\include\ShinyManager.h + ..\include\ShinyMacros.h Shiny.h
copy /b ..\src\ShinyManager.cpp + ..\src\ShinyNode.cpp + ..\src\ShinyNodePool.cpp + ..\src\ShinyOutput.cpp + ..\src\ShinyTools.cpp Shiny.cpp
pause