$ Closed cube surface mesh (legal manifold) for TetGen input
GRID,1,,0,0,0
GRID,2,,1,0,0
GRID,3,,1,1,0
GRID,4,,0,1,0
GRID,5,,0,0,1
GRID,6,,1,0,1
GRID,7,,1,1,1
GRID,8,,0,1,1
$ bottom z=0
CTRIA3,1,1,1,3,2
CTRIA3,2,1,1,4,3
$ top z=1
CTRIA3,3,1,5,6,7
CTRIA3,4,1,5,7,8
$ front y=0
CTRIA3,5,1,1,2,6
CTRIA3,6,1,1,6,5
$ back y=1
CTRIA3,7,1,4,8,7
CTRIA3,8,1,4,7,3
$ left x=0
CTRIA3,9,1,1,5,8
CTRIA3,10,1,1,8,4
$ right x=1
CTRIA3,11,1,2,3,7
CTRIA3,12,1,2,7,6
ENDDATA
