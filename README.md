# MDTRA
## Molecular Dynamics Trajectory Reader &amp; Analyzer (MDTRA)
**Version 1.2**

MDTRA is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version. MDTRA is open-source software; the source code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

## Features
- manage trajectories as file streams;
- register data sources of different types: RMSD, distance, angle, dihedral angle, angle between sections, angle between planes, solvent-accessible surface area and occlusion;
- create result collectors from single or multiple data sources;
- scale and bias results on the plot, convert between different units;
- export plot to raster image files in common formats;
- export plot to vector PDF or PS files;
- perform calculation of common statistical parameters: mean values, range, variance etc.
- estimate Pearson correlation between different data sources;
- visually compare data sources using external PDB viewer (RasMol or VMD);
- save results and statistical parameters to text files;
- use RasMol-style selection syntax to define parts of molecules;
- perform distance search along two trajectories using custom significance criterion;
- perform search for meaningful torsion angle differences along two models;
- perform search for meaningful hydrogen bonds along the trajectory;
- plot Dickerson DNA parameters in batch mode;
- build, render and save 2D-RMSD map of selected part of molecule along the trajectory;
- create and export histograms;
- prepare structures for molecular dynamics simulations (e.g. create optimized water shell);
- create custom programs in built-in Lua scripting language;
- customize plot colors for best visual experience;
- support for different PDB formats, ability to specify own PDB-like format;
- exploit modern hardware capabilities (SSE, multithreading) to speed up calculations.

**Notice:** the Program performs hardware accelerated rendering using OpenGL rendering API. If you experience any problems related to plot rendering, make sure you have latest video card drivers installed.

## Tools Used
- Qt 4.8.6 (LGPL)
- Bison 1.28
- Flex 2.5.2

## More Information
- [Official Website](http://bison.niboch.nsc.ru/mdtra.html)
- [Win32 Binaries](http://bison.niboch.nsc.ru/download-mdtraw.html)
- [Linux Binaries](http://bison.niboch.nsc.ru/download-mdtral.html)
- [PDF Manual](http://bison.niboch.nsc.ru/pub/mdtra.pdf)

## Citation
**Popov, A.V.**, Vorobjev, Y.N., Zharkov, D.O. MDTRA: A molecular dynamics trajectory analyzer with a graphical user interface. J. Comput. Chem. 2013, 34, 319-325. DOI: [10.1002/jcc.23135](http://dx.doi.org/10.1002/jcc.23135).

