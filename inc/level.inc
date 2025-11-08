
CONST LEVEL_2P=3
CONST LEVEL_3P=2
CONST LEVEL_4P=2

sub Level.load(num%)
  LOCAL Integer lvl
  LOCAL String tmp$,cmd$
  LOCAL Integer arg1,arg2,arg3,arg4

  select case num%  
  case 1,2
    lvl=int(RND()*LEVEL_2P+1)
    tmp$="P2_"+str$(lvl,2,0,"0")
    Level.setTwoPanzer
  case 3
    lvl=int(RND()*LEVEL_3P+1)
    tmp$="P3_"+str$(lvl,2,0,"0")
    Level.setThreePanzer
  case 4
    lvl=int(RND()*LEVEL_4P+1)
    tmp$="P4_"+str$(lvl,2,0,"0")
    Level.setFourPanzer
  end select

  restore tmp$
  read tmp$
  Map.load "lvl/"+tmp$+".map"

  do while cmd$<>"END"
    read cmd$
    select case cmd$
      case "O":  'Object
        read arg1,arg2,arg3,arg4
        Map.addObject(arg1,arg2,arg3,arg4)
    end select
  loop
end sub

sub Level.setTwoPanzer()
  LOCAL Float x,y,w,h

  x=Playfield.X : y=Playfield.Y : w=Playfield.W : h=Playfield.H
  Panzer.set(0,x  +75,y+h/2, 90)
  Panzer.set(1,x+w-75,y+h/2,-90)
end sub

sub Level.setThreePanzer()
  LOCAL Float x,y,w,h

  x=Playfield.X : y=Playfield.Y : w=Playfield.W : h=Playfield.H
  Panzer.set(0,x  +75,y+h/2 ,  90)
  Panzer.set(1,x+w-75,y+  75,-120)
  Panzer.set(2,x+w-75,y+h-75, -60)
end sub

sub Level.setFourPanzer()
  LOCAL Float x,y,w,h

  x=Playfield.X : y=Playfield.Y : w=Playfield.W : h=Playfield.H
  Panzer.set(0,x  +75,y  +75, 120)
  Panzer.set(1,x  +75,y+h-75,  60)
  Panzer.set(2,x+w-75,y  +75,-120)
  Panzer.set(3,x+w-75,y+h-75, -60)
end sub

P2_01:
DATA "P2_01"
DATA "O",150,66,35,71
DATA "O",64,245,42,38
DATA "O",280,1,32,43
DATA "O",280,135,32,65
DATA "O",280,293,32,43
DATA "O",408,199,35,71
DATA "O",486,53,42,38
DATA "END"

P2_02:
DATA "P2_02"
DATA "O",54,48,37,43
DATA "O",54,245,37,43
DATA "O",210,152,39,32
DATA "O",280,0,32,43
DATA "O",280,98,32,39
DATA "O",280,199,32,39
DATA "O",280,293,32,43
DATA "O",343,152,39,32
DATA "O",503,48,37,43
DATA "O",503,246,37,43
DATA "END"

P2_03:
DATA "P2_03"
DATA "O",86,0,37,43
DATA "O",470,0,37,43
DATA "O",86,293,37,43
DATA "O",470,293,37,43
DATA "O",194,70,39,35
DATA "O",195,152,39,32
DATA "O",194,231,39,35
DATA "O",279,86,33,35
DATA "O",279,152,33,32
DATA "O",279,215,33,35
DATA "O",359,70,39,35
DATA "O",359,152,39,32
DATA "O",359,231,39,35
DATA "END"

P3_01:
DATA "P3_01"
DATA "O",102,32,37,43
DATA "O",198,66,35,39
DATA "O",280,98,32,39
DATA "O",280,199,32,39
DATA "O",198,231,35,39
DATA "O",102,261,37,43
DATA "O",391,32,35,43
DATA "O",391,261,35,43
DATA "O",439,152,105,32
DATA "END"

P3_02:
DATA "P3_02"
DATA "O",86,0,37,43
DATA "O",470,0,37,43
DATA "O",86,293,37,43
DATA "O",470,293,37,43
DATA "O",194,70,39,35
DATA "O",195,152,39,32
DATA "O",194,231,39,35
DATA "O",279,86,33,35
DATA "O",279,152,33,32
DATA "O",279,215,33,35
DATA "O",359,70,39,35
DATA "O",359,152,39,32
DATA "O",359,231,39,35
DATA "O",439,152,39,32
DATA "O",487,152,39,32
DATA "O",534,152,42,32
DATA "END"

P4_01:
DATA "P4_01"
DATA "O",33,152,136,32
DATA "O",166,293,35,43
DATA "O",166,0,35,43
DATA "O",280,33,32,89
DATA "O",280,215,32,89
DATA "O",391,0,35,43
DATA "O",391,293,35,43
DATA "O",423,152,136,32
DATA "END"

P4_02:
DATA "P4_02"
DATA "O",194,70,39,35
DATA "O",195,152,39,32
DATA "O",194,231,39,35
DATA "O",279,86,33,35
DATA "O",279,152,33,32
DATA "O",279,215,33,35
DATA "O",359,70,39,35
DATA "O",359,152,39,32
DATA "O",359,231,39,35
DATA "O",439,152,39,32
DATA "O",487,152,39,32
DATA "O",534,152,42,32
DATA "O",280,0,32,43
DATA "O",280,293,32,43
DATA "O",17,152,42,32
DATA "O",65,152,42,32
DATA "O",114,152,39,32
DATA "END"


