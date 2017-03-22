@echo off

set common_cl_flags=/Gm- /W3 /MP /EHsc /Zi /Od -DPTF_WINDOWS=1
set common_link_flags=/link /INCREMENTAL:NO

set out_pdb_name=game.%random%.pdb

set ptf_files=w:\src\win64_ptf.cpp w:\src\system.cpp
set game_files=w:\src\game.cpp w:\src\game_render.cpp w:\src\game_memory.cpp

pushd w:\build
del *.pdb
cl %common_cl_flags%  %ptf_files% %common_link_flags%
cl %common_cl_flags% -DPTF_COMPILING_GAME=1 %game_files% %common_link_flags% /DLL /out:game.dll /PDB:%out_pdb_name%
popd