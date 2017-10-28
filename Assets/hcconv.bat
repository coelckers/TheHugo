alchemy winhugo.lbm -r -o
cut winhugo.raw winhugo.dat 800 128000
px hcconv
ar a ..\source\hugo.chr hugo.chr
del winhugo.raw
del winhugo.dat
del hugo.chr
