@echo off
for %%f in (*.vert) do (
	glslangValidator.exe "%%~nf.vert"
)
for %%f in (*.geom) do (
	glslangValidator.exe "%%~nf.geom"
)
for %%f in (*.frag) do (
	glslangValidator.exe "%%~nf.frag"
)
